## Pati Mobile OS (ARM64) based on Linux Kernel
* Linux Çekirdeği (Kernel) tabanlı olarak geliştirilen bir mobil işletim sistemi projesidir.
* Sürüm: 2.6-Karpuz

## Katkıda Bulunma
Projeye katkı sağlamak isterseniz, lütfen **Issues** sekmesi üzerinden yeni bir konu açın.

**Geliştirme Notu:** Bu proje, öğrenme odaklı bir protokolle geliştirilmektedir. Yapay zeka yardımı alınırken doğrudan kod kopyalamak yerine, terim araştırması ve mantık sorgulama yöntemi tercih edilmektedir.

## Lisans
Bu proje **GPL v3** lisansı ile yayınlanmıştır.
*   Yapılan tüm katkılar bu lisans altında olmalıdır.
*   Proje kapalı kaynak kodlu hale getirilemez.

## Kullanılan Teknolojiler:
- QEMU (Sanallaştırma)
- Linux Çekirdeği
- C programlama dili
- BusyBox (isviçre çakısı)

## Özellikler:
- Kendi init (başlangıç) servisi mevcuttur.
- Kendine has komutları vardır.
- Kendi sürümünüzü oluşturabilirsiniz.

## Platformlar:
- Raspberry Pi 3/4/5
- WSL (Windows Subsystem For Linux)
- Linux Dağıtımları (Örneğin Debian, Pardus)
- Termux (Android)

## Ekran Görüntüleri (Legacy):
![Pati Shell](https://raw.githubusercontent.com/mehmetdemir-tr/Pati/main/screenshots/genel.jpeg)

*Pati Shell — Ana kabuk arayüzü*

![Yardım](https://raw.githubusercontent.com/mehmetdemir-tr/Pati/main/screenshots/yardim.jpeg)

*`yardim` komutu — Tüm yerleşik komutlar*

![Patifetch](https://raw.githubusercontent.com/mehmetdemir-tr/Pati/main/screenshots/patifetch.jpeg)

*`patifetch` — Sistem bilgi aracı*

![Patifetch](https://raw.githubusercontent.com/mehmetdemir-tr/Pati/main/screenshots/mamakabi.jpeg)

*`mamakabi` — RAM bilgisi*



| Komut            | Açıklama                              |
|------------------|---------------------------------------|
| **Sistem**       |                                       |
| patifetch        | Sistem bilgisi gösterir               |
| uname            | Çekirdek ve sistem bilgisi            |
| uptime           | Çalışma süresi                        |
| hostname         | Makine adı                            |
| date             | Tarih ve saat                         |
| whoami           | Mevcut kullanıcı                      |
| id               | Kullanıcı/grup bilgisi                |
| env              | Ortam değişkenleri                    |
| **Dosya İşlemleri** |                                    |
| ls               | Dizin listesi                         |
| cd               | Dizin değiştir                        |
| pwd              | Mevcut dizin                          |
| cat              | Dosya oku                             |
| cp               | Dosya kopyala                         |
| mv               | Dosya taşı                            |
| rm               | Dosya sil                             |
| mkdir            | Dizin oluştur                         |
| rmdir            | Dizin sil                             |
| touch            | Dosya oluştur                         |
| ln               | Bağlantı oluştur                      |
| find             | Dosya bul                             |
| stat             | Dosya bilgisi                         |
| file             | Dosya türü                            |
| chmod            | İzin değiştir                         |
| chown            | Sahip değiştir                        |
| dd               | Blok düzeyinde kopyala                |
| df               | Disk kullanımı                        |
| blkid            | Blok aygıt bilgisi                    |
| **Metin İşleme** |                                       |
| grep             | İçerik ara                            |
| sed              | Metin düzenle                         |
| awk              | Metin işle                            |
| head             | İlk satırlar                          |
| tail             | Son satırlar                          |
| more             | Sayfalı oku                           |
| less             | Gelişmiş oku                          |
| wc               | Satır/sözcük/byte say                 |
| sort             | Sırala                                |
| uniq             | Tekrarları filtrele                   |
| diff             | Dosya karşılaştır                     |
| tr               | Karakter dönüştür                     |
| cut              | Sütun kırp                            |
| tee              | Dosyaya ve ekrana yaz                 |
| vi               | Metin düzenleyici                     |
| **Sıkıştırma**   |                                       |
| tar              | Arşivle/çöz                           |
| gzip             | gzip sıkıştır                         |
| gunzip           | gzip çöz                              |
| xz               | xz sıkıştır                           |
| unzip            | ZIP çöz                               |
| cpio             | Arşiv                                 |
| **Ağ**           |                                       |
| miyav            | ICMP ping                             |
| patinet          | Ağ yönetim aracı (dhcp/statik)        |
| ping             | Ağ testi                              |
| ifconfig         | Ağ arayüzü yapılandırması             |
| route            | Yönlendirme tablosu                   |
| udhcpc           | DHCP istemcisi                        |
| wget             | Dosya indir                           |
| nc               | Netcat                                |
| telnet           | Telnet bağlantısı                     |
| nslookup         | DNS sorgusu                           |
| traceroute       | Yörünge izleme                        |
| **Process**      |                                       |
| karabaş          | Çalışan processleri listeler (-a tümü)|
| ps               | Process durumu                        |
| kill             | Process öldür                         |
| killall          | Toplu process öldür                   |
| pidof            | PID bul                               |
| **PatiOS**       |                                       |
| temizle          | Ekranı temizler                       |
| mamakabı         | RAM bilgisi                           |
| imeiflasher      | IMEI Flashlayıcı                      |
| psp              | Pati Sistem Yamalayıcı                |
| umci             | Uluslararası Mobil Cihaz ID           |
| mauvyctl         | Servis yönetim aracı                  |
| 19mayıs          | Atatürk'ü Anma mesajı                 |
| Türkiye          | Türk bayrağı                          |
| zilit            | Uzaktan cihaz kilidi                  |
| mkfs             | EXT4 dosya sistemi oluşturucu         |
| yardım           | Bu mesajı gösterir                    |
| çıkış            | Sistemi kapatır                       |
| **Sistem Ynt.**  |                                       |
| mount            | Dosya sistemi bağla                   |
| umount           | Dosya sistemi ayır                    |
| insmod           | Modül yükle                           |
| rmmod            | Modül kaldır                          |
| lsmod            | Modül listesi                         |
| modprobe         | Modül yükle/kaldır                    |
| fsck             | Dosya sistemi kontrolü                |
| fdisk            | Bölümleme                             |
| reboot           | Yeniden başlat                        |
| halt             | Sistemi durdur                        |
| sync             | Disk senkronize                       |
| passwd           | Şifre değiştir                        |
| su               | Kullanıcı değiştir                    |
| crond            | Zamanlanmış görevler                  |
| logger           | Syslog'a kayıt                        |
| stty             | Terminal ayarları                     |
| nohup            | Arka plan çalıştır                    |
| xargs            | Argüman taşı                          |
| which            | Komut yolu                            |
| mknod            | Düğüm oluştur                         |
| bc               | Hesap makinesi                        |
