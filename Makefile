SHELL       := /bin/bash
.SHELLFLAGS := -eu -o pipefail -c
.ONESHELL:
.DEFAULT_GOAL := all
.NOTPARALLEL:
.DELETE_ON_ERROR:

R := \033[1;31m
G := \033[1;32m
Y := \033[1;33m
C := \033[1;36m
N := \033[0m

CROSS   := /usr/sbin/aarch64-linux-musl-gcc
UNZIP   := /usr/bin/unzip
ZLIB    := /tmp/zlib-aarch64
OPENSSL := /tmp/openssl-aarch64

DISK      := pati_disk.img
INITRAMFS := initramfs.cpio.gz
ROOTFS    := rootfs
ARCHIVE   := filesystem.zip
CMDDIR    := pati-commands
BUILDDIR  := build

PURGE_CMDS ?= 1

CFLAGS := -static -Os -fstack-protector-strong -s -Wall

SAVE_C := recovery.c mkfs.c tar.c

SKIP_C := init.c recovery.c shell.c initramfs_init.c

SRCS := $(sort $(wildcard $(CMDDIR)/*.c))
CMDS := $(filter-out $(addprefix $(CMDDIR)/,$(SKIP_C)),$(SRCS))
BINS := $(CMDS:.c=)

PARTITIONS := \
  2048:100:data:0 \
  208896:50:efs_current:1 \
  313344:47:efs_backup:1 \
  409600:32:boot:0 \
  475136:221:system:0 \
  929792:32:recovery:0 \
  999424:2:misc:1 \
  1007616:2:vbmeta:1 \
  1015808:32:cache:0

DISK_MB        := 1000
SYSTEM_SEEK    := 232
SYSTEM_MB      := 221
RECOVERY_SEEK  := 454
RECOVERY_MB    := 32

REQUIRED_TOOLS := sfdisk dd mkfs.ext4 debugfs cpio gzip find file tune2fs e2fsck mknod

.PHONY: all banner root-check tools disk extract compile fish nodes move \
        system init recovery initramfs archive clean distclean help

all: banner $(DISK) extract compile fish nodes move system init recovery initramfs archive
	@printf "\n$(G)═══ PatiOS ortamı hazır ═══$(N)\n"
	@printf "  Disk  : $(DISK)\n  Initrd: $(INITRAMFS)\n"
	@printf "  Çalıştır: ./run.sh   (recovery: ./run.sh -r, grafik: ./run.sh -g)\n"

banner:
	@printf "$(R)KedyBox - PatiOS ENV Builder by mehmetdemir-tr$(N)\n"

root-check:
	@if [ "$$EUID" -ne 0 ]; then \
		printf "$(R)[-] Lütfen root olarak çalıştırın: sudo make$(N)\n" >&2; \
		exit 1; \
	fi

tools: root-check
	@if [ ! -x "$(UNZIP)" ]; then \
		printf "$(Y)[!] unzip bulunamadı, kuruluyor...$(N)\n"; \
		apt-get install -y unzip || { printf "$(R)[-] unzip kurulamadı.$(N)\n" >&2; exit 1; }; \
	fi
	@if ! command -v genext2fs >/dev/null 2>&1; then \
		printf "$(Y)[!] genext2fs bulunamadı, kuruluyor...$(N)\n"; \
		apt-get install -y genext2fs || { printf "$(R)[-] genext2fs kurulamadı.$(N)\n" >&2; exit 1; }; \
	fi
	@if [ ! -x "$(CROSS)" ]; then \
		printf "$(Y)[!] aarch64-musl-gcc bulunamadı, path'i kontrol et: $(CROSS)$(N)\n" >&2; \
		exit 1; \
	fi
	@for t in $(REQUIRED_TOOLS); do \
		command -v "$$t" >/dev/null 2>&1 || { printf "$(R)[-] gerekli araç eksik: $$t$(N)\n" >&2; exit 1; }; \
	done
	@if [ -z "$(CMDS)" ]; then \
		printf "$(R)[-] $(CMDDIR)/ içinde derlenecek .c dosyası bulunamadı.$(N)\n" >&2; \
		exit 1; \
	fi

$(DISK):
	@$(MAKE) --no-print-directory disk

disk: root-check tools
	@printf "$(G)[*] $(DISK) oluşturuluyor ($(DISK_MB) MiB)...$(N)\n"
	rm -f "$(DISK)"
	dd if=/dev/zero of="$(DISK)" bs=1M count=$(DISK_MB) status=none
	sfdisk "$(DISK)" <<- 'EOF'
		label: gpt
		start=1M, size=100M, type=0FC63DAF-8483-4772-8E79-3D69D8477DE4
		start=102M, size=50M, type=0FC63DAF-8483-4772-8E79-3D69D8477DE4
		start=153M, size=47M, type=0FC63DAF-8483-4772-8E79-3D69D8477DE4
		start=200M, size=32M, type=0FC63DAF-8483-4772-8E79-3D69D8477DE4
		start=232M, size=221M, type=0FC63DAF-8483-4772-8E79-3D69D8477DE4
		start=454M, size=32M, type=0FC63DAF-8483-4772-8E79-3D69D8477DE4
		start=488M, size=2M, type=0FC63DAF-8483-4772-8E79-3D69D8477DE4
		start=492M, size=2M, type=0FC63DAF-8483-4772-8E79-3D69D8477DE4
		start=496M, size=32M, type=0FC63DAF-8483-4772-8E79-3D69D8477DE4
	EOF
	img=""; i1=""; i2=""
	trap 'rm -f "$$img" "$$i1" "$$i2" 2>/dev/null || true' EXIT
	for part in $(PARTITIONS); do
		off="$${part%%:*}"; rest="$${part#*:}"
		mb="$${rest%%:*}"; rest="$${rest#*:}"
		name="$${rest%%:*}"; imei="$${rest##*:}"
		seek=$$(( off / 2048 ))
		img="$$(mktemp /tmp/pati-part-XXXXXX)"
		dd if=/dev/zero of="$$img" bs=1M count="$$mb" status=none
		mkfs.ext4 -F -q -E lazy_itable_init=0 "$$img"
		if [ "$$imei" = "1" ]; then
			i1="$$(mktemp /tmp/pati-imei1-XXXXXX)"
			i2="$$(mktemp /tmp/pati-imei2-XXXXXX)"
			printf '000000000000000\n' > "$$i1"
			printf '000000000000000\n' > "$$i2"
			debugfs -w -R "write $$i1 imei.txt"  "$$img" >/dev/null 2>&1
			debugfs -w -R "write $$i2 imei2.txt" "$$img" >/dev/null 2>&1
			rm -f "$$i1" "$$i2"
		fi
		dd if="$$img" of="$(DISK)" bs=1M seek="$$seek" count="$$mb" conv=notrunc status=none
		rm -f "$$img"
		printf "$(G)[+]  %-12s seek=%-4s %3sMiB$(N)\n" "$$name" "$$seek" "$$mb"
	done
	@printf "$(G)[+] Disk hazır: $(DISK)$(N)\n"

extract: root-check tools
	@printf "$(G)[*] Dosya sistemi çıkarılıyor...$(N)\n"
	for f in $(SAVE_C); do
		if [ -f "$(CMDDIR)/$$f" ]; then cp -f "$(CMDDIR)/$$f" "/tmp/pati-bak-$$f"; fi
	done
	rc=0
	"$(UNZIP)" -oqq "$(ARCHIVE)" -d "$(ROOTFS)/" || rc=$$?
	if [ "$$rc" -gt 1 ]; then
		printf "$(R)[-] ZIP çıkartma başarısız, çıkılıyor.$(N)\n" >&2
		exit 1
	fi
	for f in $(SAVE_C); do
		if [ -f "/tmp/pati-bak-$$f" ]; then
			cp -f "/tmp/pati-bak-$$f" "$(CMDDIR)/$$f"
			rm -f "/tmp/pati-bak-$$f"
		fi
	done
	@printf "$(G)[+] Dosya sistemi hazır.$(N)\n"

compile: $(BINS)
	@basari=0; hata=0
	for b in $(BINS); do
		if [ -f "$$b" ]; then basari=$$(( basari + 1 )); else hata=$$(( hata + 1 )); fi
	done
	if [ "$$hata" -gt 0 ]; then
		printf "$(Y)[+] Derleme tamam: %s başarılı, %s hatalı — build devam ediyor.$(N)\n" "$$basari" "$$hata"
	else
		printf "$(G)[+] Derleme tamam: %s başarılı, 0 hatalı.$(N)\n" "$$basari"
	fi

$(BINS): $(CMDDIR)/%: $(CMDDIR)/%.c
	@if [ ! -x "$(CROSS)" ]; then \
		printf "$(Y)[!] aarch64-musl-gcc bulunamadı: $(CROSS)$(N)\n" >&2; exit 1; \
	fi
	src="$<"; out="$@"; stem="$${src##*/}"; stem="$${stem%.c}"
	inc=""; libs=""; extra=""
	case "$$stem" in
		mauvyd) inc="-I $(CMDDIR)/mauvyd-headers"; extra="$(CMDDIR)/mauvyd-headers/pcg.c" ;;
		tar)    inc="-I $(ZLIB)/include";    libs="$(ZLIB)/lib/libz.a" ;;
		psp)    inc="-I $(OPENSSL)/include"; libs="$(OPENSSL)/libcrypto.a" ;;
	esac
	if $(CROSS) $(CFLAGS) $$inc -o "$$out" "$$src" $$extra $$libs; then
		printf "$(G)[+] Derlendi: %s -> %s$(N)\n" "$$src" "$$out"
	else
		rm -f "$$out"
		printf "$(R)[-] Hata: %s$(N)\n" "$$src"
	fi

fish: root-check
	@printf "$(G)[*] Shell fish-shell'den alınıyor...$(N)\n"
	src="$$(find fish-shell -type f -name fish -printf '%T@ %p\n' 2>/dev/null | sort -n | tail -n1 | cut -d' ' -f2-)"
	if [ -z "$$src" ]; then
		printf "$(Y)[!] fish-shell/ içinde 'fish' bulunamadı — filesystem.zip içindeki shell kullanılacak.$(N)\n" >&2
		exit 0
	fi
	cp -f "$$src" "$(CMDDIR)/shell"
	chmod 755 "$(CMDDIR)/shell"
	@printf "$(G)[+] Shell alındı: %s -> %s$(N)\n" "$$src" "$(CMDDIR)/shell"
	@printf "$(C)    (%s)$(N)\n" "$$(file --brief "$$src")"

nodes: root-check
	@printf "$(G)[*] Device nodes oluşturuluyor...$(N)\n"
	rm -f $(ROOTFS)/dev/vda $(ROOTFS)/dev/vda? \
	      $(ROOTFS)/dev/fb0 $(ROOTFS)/dev/rtc0 $(ROOTFS)/dev/urandom \
	      $(ROOTFS)/dev/ttyAMA0 $(ROOTFS)/dev/console $(ROOTFS)/dev/null
	mkdir -p $(ROOTFS)/dev/imeidata
	mknod $(ROOTFS)/dev/vda b 254 0
	for i in $$(seq 1 9); do mknod $(ROOTFS)/dev/vda$$i b 254 "$$i"; done
	mknod $(ROOTFS)/dev/fb0 c 29 0
	chmod 666 $(ROOTFS)/dev/fb0
	mknod $(ROOTFS)/dev/rtc0 c 253 0
	mknod $(ROOTFS)/dev/urandom c 1 9
	mknod $(ROOTFS)/dev/ttyAMA0 c 204 64
	mknod $(ROOTFS)/dev/console c 5 1
	mknod $(ROOTFS)/dev/null c 1 3
	@printf "$(G)[+] Device nodes hazır.$(N)\n"

move: root-check
	@printf "$(G)[*] Binalar $(ROOTFS)/ içine taşınıyor...$(N)\n"
	mkdir -p $(ROOTFS)/lib/paticommands $(ROOTFS)/bin $(ROOTFS)/sbin $(ROOTFS)/etc/mauvyd
	if [ -f "$(CMDDIR)/mauvyd" ]; then
		cp -f "$(CMDDIR)/mauvyd" "$(ROOTFS)/sbin/init"
		rm -f "$(CMDDIR)/mauvyd"
	fi
	if [ -f "$(CMDDIR)/shell" ]; then
		cp -f "$(CMDDIR)/shell" "$(ROOTFS)/bin/"
		cp -f "$(CMDDIR)/shell" "$(ROOTFS)/lib/paticommands/"
		rm -f "$(CMDDIR)/shell"
	fi
	for f in $(CMDDIR)/*; do
		case "$$f" in
			*.c) continue ;;
		esac
		if [ -f "$$f" ]; then
			cp -f "$$f" "$(ROOTFS)/lib/paticommands/"
			printf "$(G)[+] Taşındı: %s$(N)\n" "$$f"
			if [ "$(PURGE_CMDS)" = "1" ]; then rm -f "$$f"; fi
		fi
	done
	rm -f $(ROOTFS)/lib/paticommands/init $(ROOTFS)/lib/paticommands/mauvyd
	if [ -d "$(ROOTFS)/dev/pcgconfigs" ]; then
		cp -f $(ROOTFS)/dev/pcgconfigs/*.pcg $(ROOTFS)/etc/mauvyd/ 2>/dev/null || true
	fi
	chmod -R +x $(ROOTFS)/pcg-startup $(ROOTFS)/bin $(ROOTFS)/lib/paticommands $(ROOTFS)/sbin

system: root-check tools $(DISK)
	@printf "$(G)[*] System partition oluşturuluyor...$(N)\n"
	tmpdir="$$(mktemp -d /tmp/pati-system-dir-XXXXXX)"
	img="$$(mktemp /tmp/pati-system-XXXXXX)"
	trap 'rm -rf "$$tmpdir" "$$img"' EXIT
	cp -a $(ROOTFS)/. "$$tmpdir/"
	rm -rf "$$tmpdir/init" "$$tmpdir/proc" "$$tmpdir/sys"
	dd if=/dev/zero of="$$img" bs=1M count=$(SYSTEM_MB) status=none
	mkfs.ext4 -F -b 1024 -d "$$tmpdir" "$$img" $$(($(SYSTEM_MB) * 1024))
	e2fsck -pfD "$$img" || printf "$(Y)[!] e2fsck hata düzeltti, devam ediliyor.$(N)\n"
	dd if="$$img" of="$(DISK)" bs=1M seek=$(SYSTEM_SEEK) count=$(SYSTEM_MB) conv=notrunc status=none
	@printf "$(G)[+] System partition diske yazıldı.$(N)\n"

$(ROOTFS)/init: $(CMDDIR)/initramfs_init.c
	@printf "$(G)[*] initramfs init derleniyor...$(N)\n"
	$(CROSS) -static "$<" -o "$@"
	@printf "$(G)[+] Minimal init derlendi -> %s$(N)\n" "$@"

init: root-check $(ROOTFS)/init

recovery: root-check tools $(DISK)
	@printf "$(G)[*] Recovery partition oluşturuluyor...$(N)\n"
	tmpdir="$$(mktemp -d /tmp/pati-recovery-dir-XXXXXX)"
	initbin="$$(mktemp /tmp/pati-recovery-init-XXXXXX)"
	img="$$(mktemp /tmp/pati-recovery-XXXXXX)"
	trap 'rm -rf "$$tmpdir" "$$initbin" "$$img"' EXIT
	$(CROSS) -static $(CMDDIR)/recovery.c -o "$$initbin"
	mkdir -p "$$tmpdir/sbin" "$$tmpdir/bin" "$$tmpdir/dev"
	cp -f "$$initbin" "$$tmpdir/sbin/init"
	cp -f $(ROOTFS)/bin/shell "$$tmpdir/bin/shell"
	for t in mkfs tar; do
		if [ -f "$(ROOTFS)/lib/paticommands/$$t" ]; then
			cp -f "$(ROOTFS)/lib/paticommands/$$t" "$$tmpdir/bin/$$t"
		fi
	done
	chmod +x "$$tmpdir/sbin/init" "$$tmpdir/bin/shell"
	chmod +x "$$tmpdir/bin/"*
	mknod "$$tmpdir/dev/console" c 5 1
	mknod "$$tmpdir/dev/null"    c 1 3
	mknod "$$tmpdir/dev/ttyAMA0" c 204 64
	mknod "$$tmpdir/dev/tty"     c 5 0
	dd if=/dev/zero of="$$img" bs=1M count=$(RECOVERY_MB) status=none
	genext2fs -b 32768 -d "$$tmpdir" "$$img"
	dd if="$$img" of="$(DISK)" bs=1M seek=$(RECOVERY_SEEK) count=$(RECOVERY_MB) conv=notrunc status=none
	@printf "$(G)[+] Recovery partition diske yazıldı.$(N)\n"

initramfs: root-check $(ROOTFS)/init
	@printf "$(G)[*] initramfs oluşturuluyor...$(N)\n"
	cd $(ROOTFS) && find . | cpio -o -H newc | gzip -9 > ../$(INITRAMFS)
	@printf "$(G)[+] %s hazır.$(N)\n" "$(INITRAMFS)"

archive: root-check
	@printf "$(G)[*] %s temiz rootfs'ten yeniden paketleniyor...$(N)\n" "$(ARCHIVE)"
	rm -f "$(ARCHIVE)"
	cd $(ROOTFS) && zip -qry9 ../$(ARCHIVE) . -x "./dev/*"
	@printf "$(G)[+] %s güncellendi.$(N)\n" "$(ARCHIVE)"

clean: root-check
	@printf "$(G)[*] Ara çıktılar temizleniyor...$(N)\n"
	rm -rf $(BUILDDIR)
	rm -f $(CMDDIR)/shell
	rm -f $(BINS)

distclean: clean
	@printf "$(G)[*] Üretilen tüm çıktılar siliniyor...$(N)\n"
	rm -f "$(DISK)" "$(INITRAMFS)"
	rm -rf $(ROOTFS)

help:
	@printf "$(C)KedyBox — PatiOS ENV Builder$(N)\n\n"
	@printf "$(C)Hedefler:$(N)\n"
	@printf "  $(G)%-12s$(N) tam derleme (disk varsa yeniden oluşturulmaz)\n"  "all"
	@printf "  $(G)%-12s$(N) pati_disk.img'yi sıfırdan oluştur\n"                "disk"
	@printf "  $(G)%-12s$(N) filesystem.zip -> rootfs/\n"                        "extract"
	@printf "  $(G)%-12s$(N) pati-commands/*.c derle\n"                          "compile"
	@printf "  $(G)%-12s$(N) fish binary'sini al\n"                               "fish"
	@printf "  $(G)%-12s$(N) /dev node'larını oluştur\n"                         "nodes"
	@printf "  $(G)%-12s$(N) binaları rootfs'e taşı\n"                           "move"
	@printf "  $(G)%-12s$(N) system partition (vda5)\n"                          "system"
	@printf "  $(G)%-12s$(N) rootfs/init derle\n"                                 "init"
	@printf "  $(G)%-12s$(N) recovery partition (vda6)\n"                        "recovery"
	@printf "  $(G)%-12s$(N) initramfs.cpio.gz üret\n"                           "initramfs"
	@printf "  $(G)%-12s$(N) rootfs'ten filesystem.zip yenile\n"                 "archive"
	@printf "  $(G)%-12s$(N) ara çıktıları sil\n"                                "clean"
	@printf "  $(G)%-12s$(N) disk + initramfs + rootfs dahil her şeyi sil\n"      "distclean"
	@printf "\n$(C)Değişkenler:$(N)\n"
	@printf "  $(G)%-12s$(N) = %s   (1=taşındıktan sonra sil, 0=koru)\n" "PURGE_CMDS" "$(PURGE_CMDS)"
	@printf "  $(G)%-12s$(N) = %s\n" "DISK" "$(DISK)"
	@printf "  $(G)%-12s$(N) = %s\n" "CROSS" "$(CROSS)"
	@printf "\n$(Y)Örnek:  sudo make   |   sudo make disk   |   sudo make PURGE_CMDS=0 all$(N)\n"
