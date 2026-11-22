SHELL       := /bin/bash
.SHELLFLAGS := -eu -o pipefail -c
.ONESHELL:
.DEFAULT_GOAL := all
.NOTPARALLEL:
FAKEROOT := $(if $(FAKEROOTKEY),,fakeroot)
.DELETE_ON_ERROR:

R := \033[1;31m
G := \033[1;32m
Y := \033[1;33m
C := \033[1;36m
N := \033[0m

TOOLCHAIN_DIR := $(CURDIR)/aarch64--musl--stable-2026.08-1
CROSS   := $(TOOLCHAIN_DIR)/bin/aarch64-buildroot-linux-musl-gcc
CROSS_CXX := $(TOOLCHAIN_DIR)/bin/aarch64-buildroot-linux-musl-g++
STRIP   := $(TOOLCHAIN_DIR)/bin/aarch64-buildroot-linux-musl-strip
UNZIP   := /usr/bin/unzip

USER_HOME := $(shell getent passwd $${SUDO_USER:-$$USER} | cut -d: -f6)
CARGO_ENV := PATH="$(USER_HOME)/.cargo/bin:$$PATH" RUSTUP_HOME="$(USER_HOME)/.rustup" CARGO_HOME="$(USER_HOME)/.cargo"

DISK      := pati_disk.img
SQUASHFS_IMG := rootfs.squashfs
INITRAMFS := initramfs.cpio.gz
ROOTFS    := rootfs
CMDDIR    := pati-commands
BUILDDIR  := build

PURGE_CMDS ?= 1

CFLAGS := -static -Os -fstack-protector-strong -Wall

SAVE_C := recovery.c mkfs.c tar.c

SKIP_C := init.c recovery.c shell.c bakkal_amca.c

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

USER_HOME := $(shell getent passwd $${SUDO_USER:-$$USER} | cut -d: -f6)


.PHONY: all submodules banner compile fish fish-build mbedtls coreutils curl nodes move system init initramfs recovery rootfs.squashfs toolchain

COREUTILS_APPLETS = arch base32 base64 basename basenc cat chgrp chmod chown chroot cksum comm cp csplit cut date dd df dir dircolors dirname du echo env expand expr factor false fmt fold groups head hostid hostname id install join kill link ln logname ls mkdir mkfifo mknod mktemp more mv nice nl nohup nproc numfmt od paste pathchk pr printenv printf ptx pwd readlink realpath rm rmdir seq shred shuf sleep sort split stat sum sync tac tail tee test timeout touch tr true truncate tsort tty uname unexpand uniq unlink vdir wc whoami yes

coreutils: root-check
	$(CARGO_ENV) \
	CFLAGS_aarch64_unknown_linux_musl="-std=gnu17 -Wno-incompatible-pointer-types" \
	CARGO_TARGET_AARCH64_UNKNOWN_LINUX_MUSL_LINKER=$(CROSS) \
	RUSTFLAGS="-C target-feature=+crt-static" \
	cargo build --manifest-path thirdparty/coreutils/Cargo.toml --release --target aarch64-unknown-linux-musl --no-default-features --features "$(COREUTILS_APPLETS)"
	mkdir -p $(ROOTFS)/bin
	cp -f thirdparty/coreutils/target/aarch64-unknown-linux-musl/release/coreutils $(ROOTFS)/bin/coreutils
	chmod +x $(ROOTFS)/bin/coreutils
	for cmd in $(COREUTILS_APPLETS) md5sum sha1sum sha224sum sha256sum sha384sum sha512sum '['; do \
		rm -f "$(ROOTFS)/bin/$$cmd"; \
		ln -sf coreutils "$(ROOTFS)/bin/$$cmd"; \
	done
	@printf "$(G)[+] Initialized coreutils.$(N)\n"



all: toolchain submodules banner $(DISK) compile fish mbedtls coreutils curl nodes move system init initramfs recovery rootfs.squashfs

toolchain:
	@if [ -d "$(TOOLCHAIN_DIR)/bin" ]; then \
		printf "$(G)[+] Toolchain already initialized in ./$(TOOLCHAIN_DIR)$(N)\n"; \
	else \
		printf "$(C)[?] Initializing toolchain..$(N)\n"; \
		wget https://toolchains.bootlin.com/downloads/releases/toolchains/aarch64/tarballs/aarch64--musl--stable-2026.08-1.tar.xz -O aarch64-musl.tar.xz && \
		tar -xf aarch64-musl.tar.xz && \
		rm aarch64-musl.tar.xz;
	fi

submodules:
	@if [ ! -f "thirdparty/fish-shell/CMakeLists.txt" ] || \
	    [ ! -f "thirdparty/coreutils/Cargo.toml" ] || \
	    [ ! -f "thirdparty/curl/CMakeLists.txt" ] || \
	    [ ! -f "thirdparty/mbedtls/CMakeLists.txt" ]; then \
		echo "Downloading submodules.."; \
		sudo -u $${SUDO_USER:-$$(whoami)} git submodule update --init --recursive --depth 1 || ( \
			rm -rf .git/modules/* thirdparty/* && \
			sudo -u $${SUDO_USER:-$$(whoami)} git checkout -- thirdparty && \
			git config -f .gitmodules --get-regexp '^submodule\..*\.path$$' | while read key path; do \
				name=$$(echo "$$key" | sed 's/^submodule\.\(.*\)\.path$$/\1/'); \
				url=$$(git config -f .gitmodules "submodule.$$name.url"); \
				echo "Cloning $$name -> $$path..."; \
				rm -rf "$$path" && \
				sudo -u $${SUDO_USER:-$$(whoami)} git clone --depth 1 --recursive "$$url" "$$path"; \
			done \
		); \
	fi
	@USER_HOME=$$(getent passwd $${SUDO_USER:-$$(whoami)} | cut -d: -f6); \
	sudo -u $${SUDO_USER:-$$(whoami)} "$$USER_HOME/.cargo/bin/rustup" target add aarch64-unknown-linux-musl
	@sudo -u $${SUDO_USER:-$$(whoami)} rustup target add aarch64-unknown-linux-musl

banner:
	@printf "$(R)PatiOS ENV Builder by mehmetdemir-tr$(N)\n"

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


install-coreutils: coreutils
	cp coreutils/target/aarch64-unknown-linux-musl/release/coreutils rootfs/bin/coreutils
	for cmd in ls cp mv rm mkdir cat echo pwd chmod chown; do \
		ln -sf coreutils rootfs/bin/$$cmd; \
	done


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
	esac
	if $(CROSS) $(CFLAGS) $$inc -o "$$out" "$$src" $$extra $$libs; then
		printf "$(G)[+] Derlendi: %s -> %s$(N)\n" "$$src" "$$out"
	else
		rm -f "$$out"
		printf "$(R)[-] Hata: %s$(N)\n" "$$src"
	fi

fish-build: root-check
	@printf "$(G)[*] Compiling fish-shell..$(N)\n"
	rm -rf thirdparty/fish-shell/build-arm64
	sed -i 's/cmake_policy(SET CMP0037 OLD)/# cmake_policy(SET CMP0037 OLD)/' thirdparty/fish-shell/cmake/Tests.cmake 2>/dev/null || true
	sed -i 's/add_custom_target(test DEPENDS fish_run_tests)/# add_custom_target(test DEPENDS fish_run_tests)/' thirdparty/fish-shell/cmake/Tests.cmake 2>/dev/null || true
	$(CARGO_ENV) \
	CARGO_TARGET_AARCH64_UNKNOWN_LINUX_MUSL_LINKER=$(CROSS) \
	RUSTFLAGS="-C target-feature=+crt-static" \
	cmake -S thirdparty/fish-shell -B thirdparty/fish-shell/build-arm64 \
		-DCMAKE_SYSTEM_NAME=Linux \
		-DCMAKE_SYSTEM_PROCESSOR=aarch64 \
		-DCMAKE_C_COMPILER=$(CROSS) \
		-DCMAKE_CXX_COMPILER=$(CROSS_CXX) \
		-DCMAKE_C_FLAGS="-B$(TOOLCHAIN_DIR)/libexec/gcc/aarch64-buildroot-linux-musl/15.3.0" \
		-DCMAKE_CXX_FLAGS="-B$(TOOLCHAIN_DIR)/libexec/gcc/aarch64-buildroot-linux-musl/15.3.0 -fpermissive" \
		-DCMAKE_INSTALL_PREFIX=/usr \
		-DCMAKE_INSTALL_SYSCONFDIR=/etc \
		-DCMAKE_INSTALL_DATADIR=share \
		-DCMAKE_SYSROOT=$(TOOLCHAIN_DIR)/aarch64-buildroot-linux-musl/sysroot \
		-DCMAKE_FIND_ROOT_PATH=$(TOOLCHAIN_DIR)/aarch64-buildroot-linux-musl/sysroot \
		-DCMAKE_FIND_ROOT_PATH_MODE_PROGRAM=NEVER \
		-DCMAKE_FIND_ROOT_PATH_MODE_LIBRARY=ONLY \
		-DCMAKE_FIND_ROOT_PATH_MODE_INCLUDE=ONLY \
		-DCMAKE_POLICY_VERSION_MINIMUM=3.5 \
		-DRust_CARGO_TARGET=aarch64-unknown-linux-musl \
		-DBUILD_TESTING=OFF \
		-DBUILD_TESTS=OFF \
		-DWITH_DOCS=OFF \
		-DWITH_MESSAGE_LOCALIZATION=OFF
	$(CARGO_ENV) \
	CARGO_TARGET_AARCH64_UNKNOWN_LINUX_MUSL_LINKER=$(CROSS) \
	RUSTFLAGS="-C target-feature=+crt-static" \
	cmake --build thirdparty/fish-shell/build-arm64 --parallel $$(nproc)

fish: fish-build
	@printf "$(G)[*] Copying shell..$(N)\n"
	cp -f thirdparty/fish-shell/build-arm64/fish "$(CMDDIR)/shell"
	chmod 755 "$(CMDDIR)/shell"
	$(STRIP) "$(CMDDIR)/shell" 2>/dev/null || true
	mkdir -p $(ROOTFS)/usr/share/fish
	cp -rf thirdparty/fish-shell/share/* $(ROOTFS)/usr/share/fish/
	mkdir -p $(ROOTFS)/lib
	cp -f $(TOOLCHAIN_DIR)/aarch64-buildroot-linux-musl/sysroot/lib/ld-musl-aarch64.so.1 $(ROOTFS)/lib/ 2>/dev/null || true
	cp -f $(TOOLCHAIN_DIR)/aarch64-buildroot-linux-musl/sysroot/lib/libc.musl-aarch64.so.1 $(ROOTFS)/lib/ 2>/dev/null || true
	cp -f $(TOOLCHAIN_DIR)/aarch64-buildroot-linux-musl/sysroot/usr/lib/libstdc++.so.6 $(ROOTFS)/lib/ 2>/dev/null || true
	cp -f $(TOOLCHAIN_DIR)/aarch64-buildroot-linux-musl/sysroot/lib/libgcc_s.so.1 $(ROOTFS)/lib/ 2>/dev/null || true
	cp -f $(TOOLCHAIN_DIR)/aarch64-buildroot-linux-musl/sysroot/usr/lib/libncurses.so.6 $(ROOTFS)/lib/ 2>/dev/null || true
	cp -f $(TOOLCHAIN_DIR)/aarch64-buildroot-linux-musl/sysroot/usr/lib/libtinfo.so.6 $(ROOTFS)/lib/ 2>/dev/null || true
	if [ -d $(TOOLCHAIN_DIR)/aarch64-buildroot-linux-musl/sysroot/usr/share/terminfo ]; then \
		mkdir -p $(ROOTFS)/usr/share/terminfo; \
		cp -rf $(TOOLCHAIN_DIR)/aarch64-buildroot-linux-musl/sysroot/usr/share/terminfo/. $(ROOTFS)/usr/share/terminfo/; \
	fi
	@printf "$(G)[+] Shell hazır: $(CMDDIR)/shell$(N)\n"

mbedtls: root-check
	@printf "$(G)[*] MbedTLS derleniyor...$(N)\n"
	rm -rf thirdparty/mbedtls/build-arm64 thirdparty/mbedtls/install
	cmake -S thirdparty/mbedtls -B thirdparty/mbedtls/build-arm64 \
		-DCMAKE_SYSTEM_NAME=Linux \
		-DCMAKE_SYSTEM_PROCESSOR=aarch64 \
		-DCMAKE_C_COMPILER=$(CROSS) \
		-DCMAKE_C_FLAGS="-Os -static" \
		-DCMAKE_EXE_LINKER_FLAGS="-static" \
		-DCMAKE_SYSROOT=$(TOOLCHAIN_DIR)/aarch64-buildroot-linux-musl/sysroot \
		-DCMAKE_FIND_ROOT_PATH=$(TOOLCHAIN_DIR)/aarch64-buildroot-linux-musl/sysroot \
		-DCMAKE_FIND_ROOT_PATH_MODE_PROGRAM=NEVER \
		-DCMAKE_FIND_ROOT_PATH_MODE_LIBRARY=ONLY \
		-DCMAKE_FIND_ROOT_PATH_MODE_INCLUDE=ONLY \
		-DENABLE_PROGRAMS=OFF \
		-DENABLE_TESTING=OFF \
		-DUSE_STATIC_MBEDTLS_LIBRARY=ON \
		-DUSE_SHARED_MBEDTLS_LIBRARY=OFF \
		-DDISABLE_PACKAGE_CONFIG_AND_INSTALL=ON \
		-DCMAKE_INSTALL_PREFIX=$(CURDIR)/thirdparty/mbedtls/install
	cmake --build thirdparty/mbedtls/build-arm64 --parallel $$(nproc)
	cmake --install thirdparty/mbedtls/build-arm64
	@printf "$(G)[+] MbedTLS hazır.$(N)\n"

curl: root-check mbedtls
	@printf "$(G)[*] cURL derleniyor...$(N)\n"
	PKG_CONFIG_PATH="$(CURDIR)/thirdparty/mbedtls/install/lib/pkgconfig" \
	PKG_CONFIG_LIBDIR="$(CURDIR)/thirdparty/mbedtls/install/lib/pkgconfig" \
	PKG_CONFIG_SYSROOT_DIR="" \
	cmake -S thirdparty/curl -B thirdparty/curl/build-arm64 \
		-DCMAKE_SYSTEM_NAME=Linux \
		-DCMAKE_SYSTEM_PROCESSOR=aarch64 \
		-DCMAKE_C_COMPILER=$(CROSS) \
		-DCMAKE_C_FLAGS="-Os -static" \
		-DCMAKE_EXE_LINKER_FLAGS="-static" \
		-DBUILD_SHARED_LIBS=OFF \
		-DBUILD_CURL_EXE=ON \
		-DBUILD_STATIC_CURL=ON \
		-DCURL_USE_MBEDTLS=ON \
		-DMBEDTLS_ROOT_DIR=$(CURDIR)/thirdparty/mbedtls/install \
		-DCURL_USE_OPENSSL=OFF \
		-DCURL_USE_LIBPSL=OFF \
		-DUSE_LIBIDN2=OFF \
		-DUSE_NGHTTP2=OFF \
		-DCURL_ZLIB=OFF \
		-DCURL_BROTLI=OFF \
		-DCURL_ZSTD=OFF \
		-DCURL_CA_BUNDLE=/etc/ssl/certs/ca-certificates.crt \
		-DCURL_CA_PATH=/etc/ssl/certs \
		-DCMAKE_PREFIX_PATH=$(CURDIR)/thirdparty/mbedtls/install
	cmake --build thirdparty/curl/build-arm64 --parallel $$(nproc)
	mkdir -p $(ROOTFS)/bin
	cp -f thirdparty/curl/build-arm64/src/curl $(ROOTFS)/bin/curl
	$(STRIP) $(ROOTFS)/bin/curl 2>/dev/null || true
	@printf "$(G)[+] Initialized curl.$(N)\n"


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
	@printf "$(G)[*] Moving to $(ROOTFS).. $(N)\n"
	mkdir -p $(ROOTFS)/lib/paticommands $(ROOTFS)/bin $(ROOTFS)/sbin $(ROOTFS)/etc/mauvyd $(ROOTFS)/tmp $(ROOTFS)/etc $(ROOTFS)/root $(ROOTFS)/etc/fish $(ROOTFS)/proc $(ROOTFS)/sys
	chmod 1777 $(ROOTFS)/tmp
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
	printf 'export TERM=xterm-256color\nexport HOME=/root\n' > $(ROOTFS)/etc/profile
	printf 'root:x:0:0:root:/root:/bin/shell\n' > $(ROOTFS)/etc/passwd
	printf 'pati\n' > $(ROOTFS)/etc/hostname
	printf 'NAME="PatiOS"\nVERSION="3.0 (Watermelon)"\nID=patios\nID_LIKE=linux\nVERSION_ID="3.0"\nPRETTY_NAME="PatiOS 3.0 (Watermelon)"\nVARIANT="IoT"\nVARIANT_ID=iot\nHOME_URL="https://github.com/mehmetdemir-tr/PatiOS"\nSUPPORT_URL="https://github.com/mehmetdemir-tr/PatiOS/issues"\nBUG_REPORT_URL="https://github.com/mehmetdemir-tr/PatiOS/issues"\nCPE_NAME="cpe:/o:patios:patios:3.0"\nLOGO=patios\n' > $(ROOTFS)/etc/os-release
	printf 'PatiOS 3.0 (Watermelon)\n' > $(ROOTFS)/etc/issue
	printf '127.0.0.1\tlocalhost pati\n::1\tlocalhost\n' > $(ROOTFS)/etc/hosts
	mkdir -p $(ROOTFS)/etc/ssl/certs
	cp -f /etc/ssl/certs/ca-certificates.crt $(ROOTFS)/etc/ssl/certs/ca-certificates.crt 2>/dev/null || wget -q -O $(ROOTFS)/etc/ssl/certs/ca-certificates.crt https://curl.se/ca/cacert.pem
	printf 'BUILD_ID="$(shell date +%Y%m%d)"\n' >> $(ROOTFS)/etc/os-release
	cp -f pati-config/config.fish $(ROOTFS)/etc/fish/config.fish
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

$(ROOTFS)/init: $(CMDDIR)/bakkal_amca.c
	@printf "$(G)[*] Bakkal Amca (initrd) derleniyor...$(N)\n"
	$(CROSS) -static "$<" -o "$@"
	@printf "$(G)[+] Bakkal Amca hazır -> %s$(N)\n" "$@"

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
	rm -rf $(BUILDDIR)/initramfs-root
	mkdir -p $(BUILDDIR)/initramfs-root
	cp -f $(ROOTFS)/init $(BUILDDIR)/initramfs-root/init
	chmod +x $(BUILDDIR)/initramfs-root/init
	cd $(BUILDDIR)/initramfs-root && find . | cpio -o -H newc | gzip > ../../$(INITRAMFS)
	@printf "$(G)[+] %s hazır.$(N)\n" "$(INITRAMFS)"

rootfs.squashfs: root-check move $(ROOTFS)/init
	@printf "$(G)[*] SquashFS kök dosya sistemi oluşturuluyor...$(N)\n"
	mkdir -p $(ROOTFS)/oldroot
	mksquashfs $(ROOTFS) $(SQUASHFS_IMG) -comp gzip -b 1M -all-root -noappend -no-duplicates
	@printf "$(G)[+] %s hazır.$(N)\n" "$(SQUASHFS_IMG)"


clean: root-check
	@printf "$(G)[*] Ara çıktılar temizleniyor...$(N)\n"
	rm -rf $(BUILDDIR)
	rm -f $(CMDDIR)/shell
	rm -f $(BINS)

distclean: clean
	@printf "$(G)[*] Üretilen tüm çıktılar siliniyor...$(N)\n"
	rm -f "$(DISK)" "$(INITRAMFS)"

help:
	@printf "$(C)PatiOS ENV Builder$(N)\n\n"
	@printf "$(C)Main Commands:$(N)\n"
	@printf "  $(G)%-12s$(N) full compile\n"  "all"
	@printf "  $(G)%-12s$(N) downloads toolchain\n"								 "toolchain"
	@printf "  $(G)%-12s$(N) create pati_disk.img\n"                "disk"
	@printf "  $(G)%-12s$(N) compile pati-commands/*\n"                          "compile"
	@printf "  $(G)%-12s$(N) take fish shell's binary\n"                               "fish"
	@printf "  $(G)%-12s$(N) compile fish-shell\n"                        "fish-build"
	@printf "  $(G)%-12s$(N) compile cURL\n"                                    "curl"
	@printf "  $(G)%-12s$(N) create /dev nodes\n"                         		"nodes"
	@printf "  $(G)%-12s$(N) move binaries to rootfs\n"                           "move"
	@printf "  $(G)%-12s$(N) system partition (vda5)\n"                          "system"
	@printf "  $(G)%-12s$(N) compile init\n"                                 "init"
	@printf "  $(G)%-12s$(N) recovery partition (vda6)\n"                        "recovery"
	@printf "  $(G)%-12s$(N) create initramfs.cpio.gz file\n"                           "initramfs"
	@printf "  $(G)%-12s$(N) Clean the rootfs (aka. Mr. Cleaner)\n"                                "clean"
	@printf "  $(G)%-12s$(N) delete initramfs image and disk\n"      "distclean"
	@printf "\n$(C)Variables:$(N)\n"
	@printf "  $(G)%-12s$(N) = %s   (1=delete after moving, 0=do not delete after moving)\n" "PURGE_CMDS" "$(PURGE_CMDS)"
	@printf "  $(G)%-12s$(N) = %s\n" "DISK" "$(DISK)"
	@printf "  $(G)%-12s$(N) = %s\n" "CROSS" "$(CROSS)"
	@printf "\n$(Y)f.e:  sudo make   |   sudo make disk   |   sudo make PURGE_CMDS=0 all$(N)\n"
