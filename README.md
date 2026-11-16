# PatiOS - Linux Tabanlı ARM64 üzerine inşa edilen bir dağıtım.
[![Language: C](https://img.shields.io/badge/Language-C-A8B9CC.svg)](https://ibm.com)
[![AI Assisted](https://img.shields.io/badge/AI_Assisted-orange)](#)

**PatiOS**, sistem yöneticisi, kabuk ve servis araçları gibi temel sistem bileşenleri %100 C ile yazılmış, Linux kernel'i üzerinde çalışan bir Linux From Scratch (LFS) dağıtımıdır.

* **Geliştirme Motoru:** Kendi özel Linux dağıtımınızı derlemek ve genişletmek için yardımcı ekosistem aracı olan **[KedyBox](https://github.com/mehmetdemir-tr/KedyBox)** reposunu kullanabilirsiniz.
* **Kod Adı:** `watermelon-karpuz`

---

## Teknik Mimari ve Öne Çıkan Özellikler

PatiOS, Debian/Ubuntu gibi hazır dağıtım tabanlarını kullanmaz. Tamamen kendi rootfs ve user-space araçlarını kullanır:

* **Standalone User-Space:** Sistem; Python gibi ağır yorumlayıcılara ihtiyaç duymadan, C ile derlenmiş minimal user-space ikili dosyalarıyla çalışır.
* **Musl-libc Optimizasyonu:** `aarch64-linux-musl-gcc` zinciri hedeflenerek derlenmiştir. Bu sayede standart `glibc` kütüphanelerine kıyasla daha hafif binary boyutu sağlar.

---

## Proje İçeriği ve Dosya Yapısı

* `shell.c` : İşletim sisteminin ana kabuğu.
* `mauvyd.c` : Çekirdek başlangıcından sonra devreye giren temel sistem yönetim dosyası. (systemd tarzı)
* `pati-services/` : `karabaş` gibi PatiOS-Core'a has olan servisler ve programlar.

---

## Dağıtım Kurulumu ve Hedef Platformlar

### Desteklenen / Hedeflenen platformlar
* Raspberry Pi 3 / 4 / 5 (ARM64)
* QEMU (Sanallaştırma ortamları)
* WSL (Windows Subsystem for Linux) ve Yerel Linux Dağıtımları

### Kurulum aşamaları (Raspberry Pi 3/4/5) 
1. SD kartınızı **MBR** düzeniyle bölümlendirin ve minimum **512MB FAT32** alanı oluşturun.
2. [Raspberry Pi Firmware](https://github.com/raspberrypi/firmware) reposundan `boot` klasör içeriğini bu bölüme taşıyın.
3. KedyBox ile derlediğiniz `initramfs` dosyasının adını `initramfs.gz` yaparak FAT32 bölümüne yükleyin.
4. `cmdline.txt` dosyasını oluşturup şu parametreleri ekleyin:  
   `console=serial0,115200 console=tty1 rdinit=/init`
5. `config.txt` dosyasını oluşturup sistemi şu örnek parametrelerle kaydedebilirsiniz:
   ```text
   display_auto_detect=1
   initramfs initramfs.gz followkernel
   disable_fw_kms_setup=1
   arm_64bit=1
   arm_boost=1
   ```

---

## Sistem Ekran Görüntüleri

![Shell](https://raw.githubusercontent.com/mehmetdemir-tr/Pati/main/screenshots/genel.jpeg)  
*Shell.*

![Yardım](https://raw.githubusercontent.com/mehmetdemir-tr/Pati/main/screenshots/yardim.jpeg)  
*`yardım` komutu.*

![Patifetch](https://raw.githubusercontent.com/mehmetdemir-tr/Pati/main/screenshots/patifetch.jpeg)  
*`patifetch` komutu.*

---

## Lisans ve Katkıda Bulunma

Bu proje **MIT** lisansı ile yayınlanmaktadır. 
* Projenin tüm telif hakları açık kalmak kaydıyla, ticari ve kurumsal projelerde kapatılarak veya entegre edilerek kullanılması tamamen serbesttir.
* Projeye katkı sağlamak veya hata bildirmek için **Issues** sekmesinden istek açabilirsiniz.

**Geliştirme Notu:** Bu proje, araştırma ve derin mühendislik odaklı bir protokolle geliştirilmektedir. Yapay zeka yardımı alınırken doğrudan kod kopyalamak yerine, işletim sistemi teorisi, terim araştırması ve alt seviye mantık sorgulama yöntemi tercih edilmektedir.


## Timeline (Projenin zaman akışı):
1. 27 Mart 2026 - Repo oluşturuldu (Pati adıyla)
2. 11 Nisan 2026 - İlk resmi sürüm
3. 9 Mayıs 2026 - 2.1 sürümüne ve Ananas kod adına geçiş
4. 19 Mayıs 2026 - Özel 19 Mayıs Atatürk'ü anma gençlik ve spor bayramı sürümü yayınlandı
5. 2 Temmuz 2026 - Projenin son sürümü yayınlandı (2.6)
6. 16 Kasım 2026 - PatiOS-3.0 Yayınlandı

---
*Geliştirici: [mehmetdemir-tr](https://github.com/mehmetdemir-tr)*
