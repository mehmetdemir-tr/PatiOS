import shutil
from colorama import Fore, init
import os, glob, sys, time, subprocess, tempfile
from Lang.lang import *

unzip_loc = "/usr/bin/unzip"
gcc_loc = "/aarch64-linux-musl-cross/bin/aarch64-linux-musl-gcc"

init(autoreset=True)

BUSYBOX_SKIP_COMPILE = [
    "shell.c",
    "çk__.c",
    "tar.c",
]

BUSYBOX_CMDS = [
    "sh","ash","cat","chmod","chown","cp","dd","df","echo",
    "grep","head","tail","find","sed","awk","vi","more","less",
    "kill","killall","ln","ls","mkdir","rmdir","rm","mv","touch",
    "mount","umount","ps","pidof","id","whoami","uptime",
    "uname","hostname","date","wc","sort","uniq","diff",
    "tr","cut","tee","od","hexdump","strings","seq","yes",
    "env","ping","ping6","ifconfig","route","udhcpc",
    "wget","nc","telnet","traceroute","nslookup",
    "tar","gzip","gunzip","zcat","xz","unzip","cpio",
    "poweroff","reboot","halt","sync",
    "mkfs.ext2","mkfs.vfat","fsck","blkid","fdisk",
    "insmod","rmmod","lsmod","modprobe",
    "mdev","crond","crontab","syslogd","klogd","logger",
    "clear","reset","stty","basename","dirname","realpath",
    "readlink","stat","file","time","timeout","nohup","nice",
    "xargs","which","mknod","losetup","chroot","switch_root",
    "passwd","login","su","ntpd","hwclock","bc","dc",
]

def root_control():
    if os.geteuid() != 0:
        print("Please run this script as the root user.")
        sys.exit(1)

def extract():
    print(Fore.GREEN + "Dosya sistemi çıkarılıyor..")
    if not os.path.exists(unzip_loc):
        print(Fore.YELLOW + "[!] unzip bulunamadı, kuruluyor...")
        ret = os.system("sudo apt install unzip -y")
        if ret != 0:
            print(Fore.RED + "[-] unzip kurulamadı, çıkılıyor.")
            sys.exit(1)
    for f in ["recovery.c", "mkfs.c", "tar.c"]:
        src = f"pati-commands/{f}"
        dst = f"/tmp/pati-bak-{f}"
        if os.path.exists(src):
            os.system(f"cp {src} {dst}")
    # ret1 = os.system(f"{unzip_loc} -oqq paticommands.zip -d pati-commands/") <-- devre dışı.
    ret2 = os.system(f"{unzip_loc} -oqq filesystem.zip -d rootfs/")
    if ret2 > 1:
        print(Fore.RED + "[-] ZIP çıkartma başarısız, çıkılıyor.")
        sys.exit(1)
    for f in ["recovery.c", "mkfs.c", "tar.c"]:
        bak = f"/tmp/pati-bak-{f}"
        dst = f"pati-commands/{f}"
        if os.path.exists(bak):
            os.system(f"cp {bak} {dst}")
            os.system(f"rm {bak}")
    print(Fore.GREEN + "[+] Dosya sistemi hazır.")

def compile():
    if not os.path.exists(gcc_loc):
        print(Fore.YELLOW + "[!] aarch64-musl-gcc bulunamadı, path'i kontrol et.")
        sys.exit(1)
    dosyalar = glob.glob("pati-commands/*.c")
    if not dosyalar:
        print(Fore.RED + "[-] pati-commands/ içinde .c dosyası bulunamadı.")
        sys.exit(1)
    dosyalar = [f for f in dosyalar
                if not f.endswith("init.c")
                and not f.endswith("recovery.c")
                and not any(f.endswith(skip) for skip in BUSYBOX_SKIP_COMPILE)]
    flags = "-static"
    basari, hata = 0, 0
    for f in dosyalar:
        out = f.replace(".c", "")
        if "mauvyd.c" in f:
            ret = os.system(f"{gcc_loc} {flags} -I pati-commands/mauvyd-headers {f} pati-commands/mauvyd-headers/pcg.c -o {out}")
        elif "shell.c" in f:
            ret = os.system(f"{gcc_loc} {flags} -I/tmp/readline-aarch64/include {f} /tmp/readline-aarch64/lib/libreadline.a /tmp/readline-aarch64/lib/libhistory.a /tmp/readline-aarch64/lib/libncursesw.a -o {out}")
        elif "tar.c" in f:
            ret = os.system(f"{gcc_loc} {flags} -I/tmp/zlib-aarch64/include {f} /tmp/zlib-aarch64/lib/libz.a -o {out}")
        elif "imeiflasher.c" in f:
            ret = os.system(f"{gcc_loc} {flags} {f} -o {out}")
        elif "psp.c" in f:
            libcrypto_path = os.path.abspath("libcrypto.a")
            openssl_inc = "/tmp/openssl-3.0.15/include"
            ret = os.system(f"{gcc_loc} {flags} -I{openssl_inc} {f} {libcrypto_path} -o {out}")
        else:
            ret = os.system(f"{gcc_loc} {flags} {f} -o {out}")
        if ret == 0:
            print(Fore.GREEN + f"[+] Derlendi: {f} -> {out}")
            basari += 1
        else:
            print(Fore.RED + f"[-] Hata: {f}")
            hata += 1
    print(Fore.GREEN + f"[+] Derleme tamam: {basari} başarılı, {hata} hatalı.")

def create_disk():
    disk = "pati_disk.img"
    if os.path.exists(disk):
        os.remove(disk)
    subprocess.run(["dd", "if=/dev/zero", f"of={disk}", "bs=1M", "count=1000"], check=True)
    subprocess.run(["sfdisk", disk], input=b"label: gpt\n"
        b"start=1M, size=100M, type=0FC63DAF-8483-4772-8E79-3D69D8477DE4\n"
        b"start=102M, size=50M, type=0FC63DAF-8483-4772-8E79-3D69D8477DE4\n"
        b"start=153M, size=47M, type=0FC63DAF-8483-4772-8E79-3D69D8477DE4\n"
        b"start=200M, size=32M, type=0FC63DAF-8483-4772-8E79-3D69D8477DE4\n"
        b"start=232M, size=221M, type=0FC63DAF-8483-4772-8E79-3D69D8477DE4\n"
        b"start=454M, size=32M, type=0FC63DAF-8483-4772-8E79-3D69D8477DE4\n"
        b"start=488M, size=2M, type=0FC63DAF-8483-4772-8E79-3D69D8477DE4\n"
        b"start=492M, size=2M, type=0FC63DAF-8483-4772-8E79-3D69D8477DE4\n"
        b"start=496M, size=32M, type=0FC63DAF-8483-4772-8E79-3D69D8477DE4\n", check=True)
    partitions = [
        ("2048", "100"),      # vda1 = data
        ("208896", "50"),     # vda2 = efs_current
        ("313344", "47"),     # vda3 = efs_backup
        ("409600", "32"),     # vda4 = boot
        ("475136", "221"),    # vda5 = system
        ("929792", "32"),     # vda6 = recovery 
        ("999424", "2"),      # vda7 = misc
        ("1007616", "2"),     # vda8 = vbmeta
        ("1015808", "32"),    # vda9 = cache
    ]
    for off, size_mb in partitions:
        part = tempfile.NamedTemporaryFile(delete=False, suffix=".ext4")
        part.close()
        subprocess.run(["dd", "if=/dev/zero", f"of={part.name}", "bs=1M", f"count={size_mb}"], check=True, capture_output=True)
        subprocess.run(["mkfs.ext4", "-F", part.name], check=True, capture_output=True)
        if size_mb != "100" and size_mb != "221" and size_mb != "32":
            imei1 = tempfile.NamedTemporaryFile(delete=False)
            imei1.write(b"000000000000000\n")
            imei1.close()
            imei2 = tempfile.NamedTemporaryFile(delete=False)
            imei2.write(b"000000000000000\n")
            imei2.close()
            subprocess.run(["debugfs", "-w", "-R", f"write {imei1.name} imei.txt", part.name], check=True, capture_output=True)
            subprocess.run(["debugfs", "-w", "-R", f"write {imei2.name} imei2.txt", part.name], check=True, capture_output=True)
            os.unlink(imei1.name)
            os.unlink(imei2.name)
        subprocess.run(["dd", f"if={part.name}", f"of={disk}", "bs=1M", f"seek={int(off)//2048}", f"count={size_mb}", "conv=notrunc"], check=True, capture_output=True)
        os.unlink(part.name)

def create_system_partition():
    if os.system("which genext2fs >/dev/null 2>&1") != 0:
        print(Fore.YELLOW + "[!] genext2fs bulunamadı, kuruluyor...")
        os.system("sudo apt install -y genext2fs")
    print(Fore.GREEN + "[+] Sistem bölümü oluşturuluyor...")
    system_dir = tempfile.mkdtemp()
    os.system(f"cp -a rootfs/. {system_dir}/")
    os.system(f"rm -rf {system_dir}/init {system_dir}/proc {system_dir}/sys")
    system_img = tempfile.NamedTemporaryFile(delete=False, suffix=".ext4")
    system_img.close()
    subprocess.run(["dd", "if=/dev/zero", f"of={system_img.name}", "bs=1M", "count=221"], check=True, capture_output=True)
    subprocess.run(["genext2fs", "-b", "226304", "-d", system_dir, system_img.name], check=True)
    subprocess.run(["tune2fs", "-j", "-O", "extents,uninit_bg,dir_index", system_img.name], check=True, capture_output=True)
    subprocess.run(["e2fsck", "-pfD", system_img.name], check=True, capture_output=True)
    subprocess.run(["dd", f"if={system_img.name}", "of=pati_disk.img", "bs=1M", "seek=232", "count=221", "conv=notrunc"], check=True)
    os.unlink(system_img.name)
    os.system(f"rm -rf {system_dir}")
    print(Fore.GREEN + "[+] Sistem bölümü diske yazıldı.")

def compile_initramfs_init():
    print(Fore.GREEN + "[+] initramfs init derleniyor...")
    code = """#include <sys/mount.h>
#include <sys/syscall.h>
#include <errno.h>
#include <unistd.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>

int main() {
    mkdir("/proc", 0755);
    mount("proc", "/proc", "proc", 0, NULL);
    
    char bootmode[16] = "normal";

    char *env = getenv("bootmode");
    if (env) {
        strncpy(bootmode, env, 15);
        bootmode[15] = 0;
    } else {
        mkdir("/proc", 0755);
        mount("proc", "/proc", "proc", 0, NULL);
        int fd = open("/proc/cmdline", O_RDONLY);
        if (fd >= 0) {
            char cmdline[512] = {0};
            int n = read(fd, cmdline, sizeof(cmdline) - 1);
            close(fd);
            if (n > 0) {
                cmdline[n] = 0;
                char *p = strstr(cmdline, "bootmode=");
                if (p) {
                    p += 9;
                    int i = 0;
                    while (*p && *p != ' ' && i < 15) bootmode[i++] = *p++;
                    bootmode[i] = 0;
                }
            }
        }
    }
    mkdir("/newroot", 0755);

    int r;
    if (strcmp(bootmode, "recovery") == 0 || strcmp(bootmode, "fastboot") == 0) // <-- yapım aşamasında!
        r = mount("/dev/vda6", "/newroot", "ext4", 0, NULL);
    else
        r = mount("/dev/vda5", "/newroot", "ext4", 0, NULL);
    printf("mount root ret=%d errno=%d\\n", r, errno);

    mkdir("/newroot/dev", 0755);
    mkdir("/newroot/proc", 0755);
    mkdir("/newroot/sys", 0755);
    mkdir("/newroot/oldroot", 0755);
    r = mount("/dev", "/newroot/dev", NULL, MS_MOVE, NULL);
    printf("mount /dev ret=%d errno=%d\\n", r, errno);
    r = mount("/proc", "/newroot/proc", NULL, MS_MOVE, NULL);
    printf("mount /proc ret=%d errno=%d\\n", r, errno);
    r = mount("/sys", "/newroot/sys", NULL, MS_MOVE, NULL);
    printf("mount /sys ret=%d errno=%d\\n", r, errno);
    r = syscall(SYS_pivot_root, "/newroot", "/newroot/oldroot");
    printf("pivot_root ret=%d errno=%d\\n", r, errno);
    r = chdir("/");
    printf("chdir ret=%d errno=%d\\n", r, errno);
    r = umount2("/oldroot", MNT_DETACH);
    printf("umount oldroot ret=%d errno=%d\\n", r, errno);
    struct stat lst;
    r = stat("/sbin", &lst);
    printf("stat /sbin ret=%d mode=0%o\\n", r, lst.st_mode);
    r = stat("/", &lst);
    printf("stat / ret=%d ino=%lu\\n", r, lst.st_ino);
    r = execv("/sbin/init", (char *[]){"/sbin/init", NULL});
    printf("execv /sbin/init failed! ret=%d errno=%d\\n", r, errno);
    printf("access /sbin/init F_OK=%d X_OK=%d\\n", access("/sbin/init", F_OK), access("/sbin/init", X_OK));
    struct stat st;
    r = stat("/sbin/init", &st);
    printf("stat /sbin/init ret=%d mode=0%o\\n", r, st.st_mode);
    for(;;) sleep(1);
}"""
    with open("/tmp/init1.c", "w") as f:
        f.write(code)
    ret = os.system(f"{gcc_loc} -static /tmp/init1.c -o rootfs/init")
    if ret == 0:
        print(Fore.GREEN + "[+] init 1 derlendi -> rootfs/init")
    else:
        print(Fore.RED + "[-] Init 1 derlenemedi.")
        sys.exit(1)

def create_nodes():
    print(Fore.GREEN + "[+] Device nodes oluşturuluyor...")
    os.system("sudo rm -f rootfs/dev/vda rootfs/dev/vda[1-8] rootfs/dev/fb0 rootfs/dev/rtc0 rootfs/dev/urandom")
    os.system("sudo mkdir -p rootfs/dev/imeidata")
    for i in range(10):
        os.system(f"sudo mknod rootfs/dev/vda{i} b 254 {i}" if i > 0 else f"sudo mknod rootfs/dev/vda b 254 0")
    os.system("sudo mknod rootfs/dev/fb0 c 29 0")
    os.system("sudo chmod 666 rootfs/dev/fb0")
    os.system("sudo mknod rootfs/dev/rtc0 c 253 0")
    os.system("sudo mknod rootfs/dev/urandom c 1 9")
    for dev in [("console", "c", 5, 1), ("null", "c", 1, 3), ("tty", "c", 5, 0), ("ttyAMA0", "c", 204, 64), ("ptmx", "c", 5, 2), ("random", "c", 1, 8), ("zero", "c", 1, 5)]:
        os.system(f"sudo mknod rootfs/dev/{dev[0]} {dev[1]} {dev[2]} {dev[3]}")
        os.system(f"sudo chmod 666 rootfs/dev/{dev[0]}")

def move():
    os.makedirs("rootfs/lib/paticommands", exist_ok=True)
    os.makedirs("rootfs/bin", exist_ok=True)
    os.makedirs("rootfs/sbin", exist_ok=True)
    if os.path.exists("pati-commands/mauvyd"):
        os.system("cp pati-commands/mauvyd rootfs/sbin/init")
        os.system("rm -f pati-commands/mauvyd")
    if os.path.exists("pati-commands/shell"):
        os.system("cp pati-commands/shell rootfs/bin/")
        os.system("rm -f pati-commands/shell")
    for f in glob.glob("pati-commands/*"):
        if not f.endswith(".c") and os.path.isfile(f):
            os.system(f"cp {f} rootfs/lib/paticommands/")
            print(Fore.GREEN + f"[+] Taşındı: {f}")
            os.system(f"rm -f {f}")
    os.system("rm -f rootfs/lib/paticommands/init rootfs/lib/paticommands/shell rootfs/lib/paticommands/mauvyd")
    os.system("chmod -R +x rootfs/pcg-startup rootfs/bin rootfs/lib/paticommands rootfs/sbin")



def create_cmd_symlinks():
    print(Fore.GREEN + " [+] Komutlar FS'e kaydediliyor.. ")
    bin_dir = "rootfs/bin"
    os.makedirs(bin_dir, exist_ok=True)
    for cmd in BUSYBOX_CMDS:
        link = os.path.join(bin_dir, cmd)
        if not os.path.exists(link):
            os.symlink("busybox", link)
    symlink_count = len([f for f in os.listdir(bin_dir) if os.path.islink(os.path.join(bin_dir, f))])
    print(Fore.GREEN + f"[+] {symlink_count} adet komut hazır.")

def create_rootfs_etc():
    print(Fore.GREEN + "[+] /etc dosyaları oluşturuluyor...")
    os.makedirs("rootfs/etc", exist_ok=True)
    with open("rootfs/etc/passwd", "w") as f: f.write("root::0:0:root:/root:/bin/sh\n")
    with open("rootfs/etc/group", "w") as f: f.write("root:x:0:\n")
    with open("rootfs/etc/hostname", "w") as f: f.write("pati-mobile\n")
    with open("rootfs/etc/resolv.conf", "w") as f: f.write("nameserver 8.8.8.8\n")
    with open("rootfs/etc/profile", "w") as f:
        f.write("export PATH=/bin:/sbin:/usr/bin:/usr/sbin:/pcg-startup:/lib/paticommands\n")
        f.write("export HOME=/root\n")
        f.write("export TERM=linux\n")
        f.write("PS1='pati@2.6-tr> '\n")
    os.makedirs("rootfs/etc/mauvyd", exist_ok=True)
    pcg_src = "rootfs/dev/pcgconfigs"
    pcg_dst = "rootfs/etc/mauvyd"
    if os.path.isdir(pcg_src):
        for f in os.listdir(pcg_src):
            if f.endswith(".pcg"):
                shutil.copy2(os.path.join(pcg_src, f), os.path.join(pcg_dst, f))
    for f in ["shell.pcg"]:
        fp = os.path.join(pcg_dst, f)
        if os.path.exists(fp):
            with open(fp, "r") as fh: content = fh.read()
            content = content.replace("location=/bin/shell", "location=/bin/sh")
            if "watch=1" not in content: content += "\nwatch=1"
            with open(fp, "w") as fh: fh.write(content)
    print(Fore.GREEN + "[+] /etc hazır.")

def build_initramfs():
    print(Fore.GREEN + "[+] initramfs oluşturuluyor...")
    ret = os.system("cd rootfs && find . | cpio -o -H newc | gzip -9 > ../initramfs.cpio.gz")
    if ret == 0:
        print(Fore.GREEN + "[+] initramfs.cpio.gz hazır.")
    else:
        print(Fore.RED + "[-] initramfs oluşturulamadı.")
        sys.exit(1)
