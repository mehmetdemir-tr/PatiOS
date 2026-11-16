# PatiOS

aarch64 (Snapdragon) cihazlar ve QEMU icin, tamamen statik musl uzerine kurulu
minimal Linux dagitimi. Kendi cekirdegi (`Pati-2.1-Generic`), kendi C komutlari
ve dogrudan framebuffer uzerine cizilen dokunmatik arayuzleri vardir.

## Depo icerigi

| Yol | Aciklama |
|---|---|
| `Makefile` | Disk imaji, system/recovery partition, initramfs ve derleme akisi |
| `pati-commands/` | PatiOS'a ozgu komutlar (C kaynak kodu, musl statik derlenir) |
| `run.sh` | QEMU baslatici: normal / `-g` grafik (SDL) / `-v` VNC / `-r` recovery |
| `filesystem.zip` | Taban rootfs; `make extract` ile `rootfs/` altina acilir |

`filesystem.zip` icindeki `bin/coreutils` bir uutils multicall binary'sidir;
`ls`, `cat`, `csplit` gibi adlar ona symlink'tir (busybox mantigi).

## Gereksinimler

`aarch64-linux-musl-gcc`, `unzip`, `zip`, `sfdisk`, `mkfs.ext4` + `debugfs`
(e2fsprogs), `genext2fs`, `cpio`, `gzip`, `file`, `qemu-system-aarch64` ve
root yetkisi (Fedora/Debian paket isimleriyle kurulabilir).

Cekirdek Image dosyasi depoya dahil degildir: `run.sh` calisma dizininde
`Pati21-Kernel-Snapdragon` adinda bir kernel bekler.

## Derleme

```bash
sudo make              # tam akis: disk -> extract -> compile -> system -> ...
sudo make PURGE_CMDS=0 all   # derlenen binaryleri pati-commands/ altinda koru
sudo make compile      # yalnizca pati-commands/*.c derle
sudo make archive      # rootfs/ -> filesystem.zip (symlink'ler korunur)
```

## Calistirma

```bash
./run.sh        # metin konsolu (ttyAMA0)
./run.sh -g     # SDL grafik arayuz
./run.sh -v     # VNC (localhost:5901)
./run.sh -r     # recovery boot
```

## pati-commands

`init.c`, `recovery.c`, `shell.c`, `initramfs_init.c` Makefile akisinin ozel
hedefleri tarafindan, diger `*.c` dosyalari ise `compile` hedefi tarafindan
derlenir. Grafik uygulamalar (`patipos` kasa sistemi gibi) dogrudan
`/dev/fb0` + `/dev/input` uzerinden calisir.
