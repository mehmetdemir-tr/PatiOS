#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdint.h>
#include <time.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <linux/fs.h>

#define EXT4_SUPER_MAGIC       0xEF53
#define EXT4_DYNAMIC_REV       1
#define EXT4_SB_OFFSET         1024
#define EXT4_BLOCK_SIZE        4096
#define EXT4_INODE_SIZE        256
#define EXT4_BLOCKS_PER_GROUP  32768
#define EXT4_INODE_RATIO       16384
#define EXT4_FIRST_INO         11
#define EXT4_ROOT_INO          2
#define EXT4_BAD_INO           1

#define EXT4_FEATURE_INCOMPAT_FILETYPE   0x0002
#define EXT4_FEATURE_INCOMPAT_EXTENTS    0x0040
#define EXT4_FEATURE_INCOMPAT_FLEX_BG    0x0200
#define EXT4_FEATURE_RO_COMPAT_SPARSE_SUPER 0x0001
#define EXT4_FEATURE_RO_COMPAT_LARGE_FILE   0x0002
#define EXT4_FEATURE_RO_COMPAT_GDT_CSUM     0x0010
#define EXT4_FEATURE_RO_COMPAT_DIR_NLINK    0x0020
#define EXT4_FEATURE_COMPAT_DIR_PREALLOC  0x0001
#define EXT4_FEATURE_COMPAT_EXT_ATTR      0x0008
#define EXT4_FEATURE_COMPAT_RESIZE_INODE  0x0010
#define EXT4_FEATURE_COMPAT_DIR_INDEX     0x0020

#define EXT4_S_IFDIR  0x4000
#define EXT4_FT_DIR   2

struct ext4_superblock {
    uint32_t s_inodes_count;
    uint32_t s_blocks_count_lo;
    uint32_t s_r_blocks_count_lo;
    uint32_t s_free_blocks_count_lo;
    uint32_t s_free_inodes_count_lo;
    uint32_t s_first_data_block;
    uint32_t s_log_block_size;
    uint32_t s_log_cluster_size;
    uint32_t s_blocks_per_group;
    uint32_t s_clusters_per_group;
    uint32_t s_inodes_per_group;
    uint32_t s_mtime;
    uint32_t s_wtime;
    uint16_t s_mnt_count;
    uint16_t s_max_mnt_count;
    uint16_t s_magic;
    uint16_t s_state;
    uint16_t s_errors;
    uint16_t s_minor_rev_level;
    uint32_t s_lastcheck;
    uint32_t s_checkinterval;
    uint32_t s_creator_os;
    uint32_t s_rev_level;
    uint16_t s_def_resuid;
    uint16_t s_def_resgid;
    uint32_t s_first_ino;
    uint16_t s_inode_size;
    uint16_t s_block_group_nr;
    uint32_t s_feature_compat;
    uint32_t s_feature_incompat;
    uint32_t s_feature_ro_compat;
    uint8_t  s_uuid[16];
    uint8_t  s_volume_name[16];
    uint8_t  s_last_mounted[64];
    uint32_t s_algorithm_usage_bitmap;
    uint8_t  s_prealloc_blocks;
    uint8_t  s_prealloc_dir_blocks;
    uint16_t s_reserved_gdt_blocks;
    uint8_t  s_journal_uuid[16];
    uint32_t s_journal_inum;
    uint32_t s_journal_dev;
    uint32_t s_last_orphan;
    uint32_t s_hash_seed[4];
    uint8_t  s_def_hash_version;
    uint8_t  s_jnl_backup_type;
    uint16_t s_desc_size;
    uint32_t s_default_mount_opts;
    uint32_t s_first_meta_bg;
    uint32_t s_mkfs_time;
    uint32_t s_jnl_blocks[17];
} __attribute__((packed));

struct ext4_group_descriptor {
    uint32_t bg_block_bitmap_lo;
    uint32_t bg_inode_bitmap_lo;
    uint32_t bg_inode_table_lo;
    uint16_t bg_free_blocks_count_lo;
    uint16_t bg_free_inodes_count_lo;
    uint16_t bg_used_dirs_count_lo;
    uint16_t bg_flags;
    uint32_t bg_exclude_bitmap_lo;
    uint16_t bg_block_bitmap_csum_lo;
    uint16_t bg_inode_bitmap_csum_lo;
    uint16_t bg_itable_unused_lo;
    uint16_t bg_checksum;
} __attribute__((packed));

struct ext4_inode {
    uint16_t i_mode;
    uint16_t i_uid;
    uint32_t i_size_lo;
    uint32_t i_atime;
    uint32_t i_ctime;
    uint32_t i_mtime;
    uint32_t i_dtime;
    uint16_t i_gid;
    uint16_t i_links_count;
    uint32_t i_blocks_lo;
    uint32_t i_flags;
    uint32_t i_osd1;
    uint8_t  i_block[60];
    uint32_t i_generation;
    uint32_t i_file_acl_lo;
    uint32_t i_size_high;
    uint32_t i_obso_faddr;
    uint8_t  i_osd2[12];
    uint16_t i_blocks_high;
    uint16_t i_file_acl_high;
    uint16_t i_uid_high;
    uint16_t i_gid_high;
    uint16_t i_checksum_lo;
    uint16_t i_reserved;
    uint8_t  i_extra[100];
} __attribute__((packed));

struct ext4_extent {
    uint32_t block;
    uint16_t len;
    uint16_t start_hi;
    uint32_t start_lo;
} __attribute__((packed));

struct ext4_dir_entry {
    uint32_t inode;
    uint16_t rec_len;
    uint8_t  name_len;
    uint8_t  file_type;
    char     name[];
} __attribute__((packed));

void generate_uuid(uint8_t *uuid) {
    int fd = open("/dev/urandom", O_RDONLY);
    if (fd < 0) {
        srand(time(NULL) ^ getpid());
        for (int i = 0; i < 16; i++) uuid[i] = rand() & 0xFF;
        return;
    }
    read(fd, uuid, 16);
    close(fd);
    uuid[6] = (uuid[6] & 0x0F) | 0x40;
    uuid[8] = (uuid[8] & 0x3F) | 0x80;
}

void write_block(int fd, uint64_t block_nr, void *data) {
    uint64_t offset = block_nr * EXT4_BLOCK_SIZE;
    lseek(fd, offset, SEEK_SET);
    write(fd, data, EXT4_BLOCK_SIZE);
}

int main(int argc, char *argv[]) {
    printf("PatiOS mkfs.ext4 (minimal)\n\n");

    char *device = NULL;
    int force = 0;

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-F") == 0 || strcmp(argv[i], "-f") == 0)
            force = 1;
        else if (argv[i][0] != '-')
            device = argv[i];
    }

    if (!device) {
        fprintf(stderr, "Kullanim: mkfs.ext4 [-F] <aygit>\n");
        return 1;
    }

    int fd = open(device, O_RDWR);
    if (fd < 0) {
        fprintf(stderr, "HATA: %s acilamadi!\n", device);
        return 1;
    }

    uint64_t device_size = 0;
    if (ioctl(fd, BLKGETSIZE64, &device_size) != 0) {
        fprintf(stderr, "HATA: Aygit boyutu alinamadi!\n");
        close(fd);
        return 1;
    }

    printf("Aygit: %s  Boyut: %lu MB\n",
           device, (unsigned long)(device_size / (1024 * 1024)));

    uint64_t blocks_total = device_size / EXT4_BLOCK_SIZE;
    if (blocks_total < 16) {
        fprintf(stderr, "HATA: Aygit cok kucuk!\n");
        close(fd);
        return 1;
    }

    uint32_t groups_count = (blocks_total + EXT4_BLOCKS_PER_GROUP - 1)
                            / EXT4_BLOCKS_PER_GROUP;
    uint32_t inodes_per_group = (EXT4_BLOCKS_PER_GROUP * EXT4_BLOCK_SIZE)
                                / EXT4_INODE_RATIO;
    if (inodes_per_group < 32) inodes_per_group = 32;
    uint64_t inodes_total = (uint64_t)inodes_per_group * groups_count;

    printf("Blok: %lu  Grup: %u  Inode: %lu\n",
           (unsigned long)blocks_total, groups_count,
           (unsigned long)inodes_total);

    if (!force) {
        printf("\nDevam? (e/E): ");
        char answer = getchar();
        if (answer != 'e' && answer != 'E') {
            printf("Iptal.\n");
            close(fd);
            return 1;
        }
    }

    uint8_t *zero_block = calloc(1, EXT4_BLOCK_SIZE);
    uint8_t *sb_block  = calloc(1, EXT4_BLOCK_SIZE);
    uint8_t *gdt_block = calloc(1, EXT4_BLOCK_SIZE);
    uint8_t *bitmap_block = calloc(1, EXT4_BLOCK_SIZE);
    uint8_t *inode_block = calloc(1, EXT4_BLOCK_SIZE);
    uint8_t *data_block = calloc(1, EXT4_BLOCK_SIZE);

    if (!zero_block || !sb_block || !gdt_block || !bitmap_block
        || !inode_block || !data_block) {
        fprintf(stderr, "HATA: Bellek yetersiz!\n");
        close(fd);
        return 1;
    }

    printf("\nDosya sistemi olusturuluyor...\n");

    for (uint64_t b = 0; b < 2048 && b < blocks_total; b++)
        write_block(fd, b, zero_block);

    printf("  [1] Superblock yaziliyor...\n");
    struct ext4_superblock *sb = (struct ext4_superblock *)(sb_block + EXT4_SB_OFFSET);
    memset(sb, 0, sizeof(struct ext4_superblock));

    sb->s_inodes_count         = inodes_total;
    sb->s_blocks_count_lo      = blocks_total;
    sb->s_r_blocks_count_lo    = blocks_total / 20;
    sb->s_free_blocks_count_lo = blocks_total;
    sb->s_free_inodes_count_lo = inodes_total;
    sb->s_first_data_block     = 0;
    sb->s_log_block_size       = 2;
    sb->s_log_cluster_size     = 2;
    sb->s_blocks_per_group     = EXT4_BLOCKS_PER_GROUP;
    sb->s_clusters_per_group   = EXT4_BLOCKS_PER_GROUP;
    sb->s_inodes_per_group     = inodes_per_group;
    sb->s_mtime                = 0;
    sb->s_wtime                = time(NULL);
    sb->s_mnt_count            = 0;
    sb->s_max_mnt_count        = 0;
    sb->s_magic                = EXT4_SUPER_MAGIC;
    sb->s_state                = 1;
    sb->s_errors               = 1;
    sb->s_minor_rev_level      = 0;
    sb->s_lastcheck            = sb->s_wtime;
    sb->s_checkinterval        = 0;
    sb->s_creator_os           = 0;
    sb->s_rev_level            = EXT4_DYNAMIC_REV;
    sb->s_def_resuid           = 0;
    sb->s_def_resgid           = 0;
    sb->s_first_ino            = EXT4_FIRST_INO;
    sb->s_inode_size           = EXT4_INODE_SIZE;
    sb->s_block_group_nr       = 0;
    sb->s_feature_compat       = EXT4_FEATURE_COMPAT_DIR_PREALLOC
                                | EXT4_FEATURE_COMPAT_EXT_ATTR
                                | EXT4_FEATURE_COMPAT_RESIZE_INODE
                                | EXT4_FEATURE_COMPAT_DIR_INDEX;
    sb->s_feature_incompat     = EXT4_FEATURE_INCOMPAT_FILETYPE
                                | EXT4_FEATURE_INCOMPAT_EXTENTS
                                | /*EXT4_FEATURE_INCOMPAT_FLEX_BG*/0;
    sb->s_feature_ro_compat    = EXT4_FEATURE_RO_COMPAT_SPARSE_SUPER
                                | EXT4_FEATURE_RO_COMPAT_LARGE_FILE
                                /*| EXT4_FEATURE_RO_COMPAT_GDT_CSUM*/
                                | EXT4_FEATURE_RO_COMPAT_DIR_NLINK;
    generate_uuid(sb->s_uuid);
    sb->s_desc_size            = sizeof(struct ext4_group_descriptor);
    sb->s_default_mount_opts   = 0;
    sb->s_first_meta_bg        = 0;
    sb->s_mkfs_time            = sb->s_wtime;

    write_block(fd, 0, sb_block);

    printf("  [2] Group Descriptor Table yaziliyor...\n");
    uint32_t itable_blocks = (inodes_per_group * EXT4_INODE_SIZE
                             + EXT4_BLOCK_SIZE - 1) / EXT4_BLOCK_SIZE;
    uint32_t meta_per_group_first = 2 + 1 + 1 + itable_blocks;
    uint32_t meta_per_group_other = 1 + 1 + itable_blocks;

    for (uint32_t g = 0; g < groups_count; g++) {
        struct ext4_group_descriptor *gd =
            (struct ext4_group_descriptor *)(gdt_block
                                             + g * sizeof(struct ext4_group_descriptor));

        uint32_t blocks_in_group = EXT4_BLOCKS_PER_GROUP;
        if (g == groups_count - 1) {
            uint32_t remaining = blocks_total
                                 - (uint64_t)g * EXT4_BLOCKS_PER_GROUP;
            blocks_in_group = remaining;
        }

        if (g == 0) {
            gd->bg_block_bitmap_lo = 2;
            gd->bg_inode_bitmap_lo = 3;
            gd->bg_inode_table_lo  = 4;
            uint32_t used_blocks = meta_per_group_first;
            uint32_t total = (g == groups_count - 1)
                             ? blocks_in_group : EXT4_BLOCKS_PER_GROUP;
            gd->bg_free_blocks_count_lo = total - used_blocks;
            gd->bg_free_inodes_count_lo = inodes_per_group - 2;
            gd->bg_used_dirs_count_lo   = 1;
            gd->bg_itable_unused_lo     = inodes_per_group - 2;
        } else {
            uint64_t group_offset = (uint64_t)g * EXT4_BLOCKS_PER_GROUP;
            gd->bg_block_bitmap_lo = group_offset;
            gd->bg_inode_bitmap_lo = group_offset + 1;
            gd->bg_inode_table_lo  = group_offset + 2;
            uint32_t used = meta_per_group_other;
            uint32_t total = (g == groups_count - 1)
                             ? blocks_in_group : EXT4_BLOCKS_PER_GROUP;
            gd->bg_free_blocks_count_lo = total - used;
            gd->bg_free_inodes_count_lo = inodes_per_group;
            gd->bg_used_dirs_count_lo   = 0;
            gd->bg_itable_unused_lo     = inodes_per_group;
        }

        gd->bg_flags               = 0;
        gd->bg_exclude_bitmap_lo   = 0;
        gd->bg_block_bitmap_csum_lo= 0;
        gd->bg_inode_bitmap_csum_lo= 0;
        gd->bg_checksum            = 0;
    }

    memcpy(sb_block + EXT4_SB_OFFSET + 1024,
           gdt_block, groups_count * sizeof(struct ext4_group_descriptor));
    write_block(fd, 0, sb_block);
    write_block(fd, 1, gdt_block);

    printf("  [3] Blok bitmap yaziliyor...\n");
    for (uint32_t g = 0; g < groups_count; g++) {
        memset(bitmap_block, 0, EXT4_BLOCK_SIZE);

        uint32_t blocks_in_group = EXT4_BLOCKS_PER_GROUP;
        if (g == groups_count - 1) {
            uint32_t remaining = blocks_total
                                 - (uint64_t)g * EXT4_BLOCKS_PER_GROUP;
            blocks_in_group = remaining;
        }

        uint32_t meta_blocks = (g == 0) ? meta_per_group_first
                                        : meta_per_group_other;
        for (uint32_t b = 0; b < meta_blocks && b < blocks_in_group; b++)
            bitmap_block[b / 8] |= (1 << (b % 8));
        for (uint32_t b = blocks_in_group; b < EXT4_BLOCKS_PER_GROUP; b++)
            bitmap_block[b / 8] |= (1 << (b % 8));

        uint64_t bitmap_blk = (g == 0) ? 2
            : ((uint64_t)g * EXT4_BLOCKS_PER_GROUP);
        write_block(fd, bitmap_blk, bitmap_block);
    }

    uint64_t total_meta = meta_per_group_first
                         + (uint64_t)meta_per_group_other * (groups_count - 1);
    sb->s_free_blocks_count_lo = blocks_total - total_meta;

    printf("  [4] Inode bitmap yaziliyor...\n");
    for (uint32_t g = 0; g < groups_count; g++) {
        memset(bitmap_block, 0, EXT4_BLOCK_SIZE);

        uint32_t inodes_in_group = inodes_per_group;
        if (g == groups_count - 1) {
            uint64_t total_inodes_so_far = (uint64_t)inodes_per_group * g;
            uint64_t remaining = inodes_total - total_inodes_so_far;
            if (remaining < inodes_in_group) inodes_in_group = remaining;
        }

        if (g == 0)
            bitmap_block[0] |= 0x06;

        uint64_t bitmap_blk = (g == 0) ? 3
            : ((uint64_t)g * EXT4_BLOCKS_PER_GROUP + 1);
        write_block(fd, bitmap_blk, bitmap_block);
    }

    sb->s_free_inodes_count_lo = inodes_total - 2;
    memcpy(sb_block + EXT4_SB_OFFSET, sb, sizeof(struct ext4_superblock));
    write_block(fd, 0, sb_block);

    printf("  [5] Inode tablosu yaziliyor...\n");
    struct ext4_inode *root_inode = (struct ext4_inode *)inode_block;
    root_inode->i_mode      = EXT4_S_IFDIR | 0755;
    root_inode->i_uid       = 0;
    root_inode->i_size_lo   = EXT4_BLOCK_SIZE;
    root_inode->i_atime     = sb->s_wtime;
    root_inode->i_ctime     = sb->s_wtime;
    root_inode->i_mtime     = sb->s_wtime;
    root_inode->i_dtime     = 0;
    root_inode->i_gid       = 0;
    root_inode->i_links_count = 2;
    root_inode->i_blocks_lo   = EXT4_BLOCK_SIZE / 512;
    root_inode->i_flags       = 0x80000;
    root_inode->i_osd1        = 0;

    uint16_t *ext_magic = (uint16_t *)(root_inode->i_block);
    ext_magic[0] = 0xF30A;
    ext_magic[1] = 1;
    ext_magic[2] = 1;
    ext_magic[3] = 0;

    uint32_t first_data_block = meta_per_group_first;
    struct ext4_extent *ext = (struct ext4_extent *)(root_inode->i_block + 12);
    ext->block    = 0;
    ext->len      = 1;
    ext->start_hi = 0;
    ext->start_lo = first_data_block;

    memcpy(inode_block + (EXT4_ROOT_INO - 1) * EXT4_INODE_SIZE,
           root_inode, sizeof(struct ext4_inode));

    uint64_t itable_start = 4;
    write_block(fd, itable_start, inode_block);
    for (uint32_t b = 1; b < itable_blocks; b++)
        write_block(fd, itable_start + b, zero_block);

    for (uint32_t g = 1; g < groups_count; g++) {
        uint64_t group_start = (uint64_t)g * EXT4_BLOCKS_PER_GROUP;
        uint64_t itable_start_g = group_start + 2;
        for (uint32_t b = 0; b < itable_blocks; b++)
            write_block(fd, itable_start_g + b, zero_block);
    }

    printf("  [6] Root dizin yaziliyor...\n");
    memset(data_block, 0, EXT4_BLOCK_SIZE);

    struct ext4_dir_entry *de = (struct ext4_dir_entry *)data_block;
    de->inode     = EXT4_ROOT_INO;
    de->name_len  = 1;
    de->file_type = EXT4_FT_DIR;
    de->rec_len   = 12;
    de->name[0]   = '.';

    de = (struct ext4_dir_entry *)(data_block + 12);
    de->inode     = EXT4_ROOT_INO;
    de->name_len  = 2;
    de->file_type = EXT4_FT_DIR;
    de->rec_len   = EXT4_BLOCK_SIZE - 12;
    de->name[0]   = '.';
    de->name[1]   = '.';

    write_block(fd, first_data_block, data_block);

    memcpy(sb_block + EXT4_SB_OFFSET, sb, sizeof(struct ext4_superblock));
    write_block(fd, 0, sb_block);

    ioctl(fd, BLKFLSBUF, 0);
    fsync(fd);
    close(fd);

    free(zero_block);
    free(sb_block);
    free(gdt_block);
    free(bitmap_block);
    free(inode_block);
    free(data_block);

    printf("\nTamamlandi! %lu blok, %lu inode.\n",
           (unsigned long)blocks_total, (unsigned long)inodes_total);
    return 0;
}
