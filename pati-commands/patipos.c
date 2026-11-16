/*
 * patipos - PatiOS dokunmatik kasa (POS) uygulamasi
 *
 * Dogrudan /dev/fb0 uzerine cift tamponlu cizim yapar, /dev/input/event*
 * icinde tarama ile buldugu dokunmatik/mouse aygitindan girdi alir.
 *
 * V0.2: Oklu secim sistemi
 *  - 6 urunluk izgara; her urune dokunmak sepete 1 adet ekler (xN rozetli)
 *  - Hucre sol seridi (-) adet azaltir
 *  - Sepet paneli: satir satir icerik + TOPLAM/ADET
 *  - FIS BAS: tum urunleri adetleriyle fise yazdirir (ESC/POS)
 *  - Cozunurlukten bagimsiz oransal yerlesim
 */

#include <dirent.h>
#include <fcntl.h>
#include <linux/fb.h>
#include <linux/input.h>
#include <linux/kd.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <unistd.h>

#define RENK_ARKAPLAN  0xFF12161F
#define RENK_KART      0xFF1E2533
#define RENK_KART_SEC  0xFF2A4A66
#define RENK_EKSI      0xFF232B3A
#define RENK_VURGU     0xFF2ECC71
#define RENK_TEHLIKE   0xFFE74C3C
#define RENK_YAZI      0xFFFFFFFF
#define RENK_SONUK     0xFF8FA3B8

#define URUN_SAYISI 6

typedef struct {
    const char *ad;
    int fiyat;
    int adet;
    /* Cizimde guncellenen hucre dikdortgeni (vurus testi icin) */
    int x, y, w, h;
    int son_vurus; /* Dokunma geri bildirimi icin kare sayaci */
} Urun;

static Urun urunler[URUN_SAYISI] = {
    {"CAY",    15, 0},
    {"KAHVE",  45, 0},
    {"TOST",   80, 0},
    {"SU",     10, 0},
    {"SIMIT",  25, 0},
    {"AYRAN",  30, 0},
};

/* Alt bar butonlari; her karede ekran_ciz gunceller */
static struct { int x, y, w, h; } fis_btn, iptal_btn;

uint32_t *fbp = NULL;
uint32_t *backbuffer = NULL;
struct fb_var_screeninfo vinfo;
struct fb_fix_screeninfo finfo;
size_t screensize = 0;

/* 8x8 ASCII font (buyuk harfler, rakamlar, noktalama) */
const uint8_t font8x8[128][8] = {
    [' '] = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
    ['-'] = {0x00,0x00,0x00,0x7E,0x00,0x00,0x00,0x00},
    [':'] = {0x00,0x18,0x18,0x00,0x00,0x18,0x18,0x00},
    ['0'] = {0x3C,0x66,0x6E,0x76,0x66,0x66,0x3C,0x00},
    ['1'] = {0x18,0x38,0x18,0x18,0x18,0x18,0x7E,0x00},
    ['2'] = {0x3C,0x66,0x06,0x0C,0x18,0x30,0x7E,0x00},
    ['3'] = {0x3C,0x66,0x06,0x1C,0x06,0x66,0x3C,0x00},
    ['4'] = {0x0C,0x1C,0x2C,0x4C,0x7E,0x0C,0x0C,0x00},
    ['5'] = {0x7E,0x60,0x7C,0x06,0x06,0x66,0x3C,0x00},
    ['6'] = {0x3C,0x60,0x7C,0x66,0x66,0x66,0x3C,0x00},
    ['7'] = {0x7E,0x06,0x0C,0x18,0x30,0x30,0x30,0x00},
    ['8'] = {0x3C,0x66,0x66,0x3C,0x66,0x66,0x3C,0x00},
    ['9'] = {0x3C,0x66,0x66,0x3E,0x06,0x0C,0x38,0x00},
    ['A'] = {0x18,0x3C,0x66,0x66,0x7E,0x66,0x66,0x00},
    ['B'] = {0x7C,0x66,0x66,0x7C,0x66,0x66,0x7C,0x00},
    ['C'] = {0x3C,0x66,0x60,0x60,0x60,0x66,0x3C,0x00},
    ['D'] = {0x1F,0x36,0x66,0x66,0x66,0x36,0x1F,0x00},
    ['E'] = {0x7E,0x60,0x60,0x78,0x60,0x60,0x7E,0x00},
    ['F'] = {0x7E,0x60,0x60,0x78,0x60,0x60,0x60,0x00},
    ['H'] = {0x66,0x66,0x66,0x7E,0x66,0x66,0x66,0x00},
    ['I'] = {0x3C,0x18,0x18,0x18,0x18,0x18,0x3C,0x00},
    ['K'] = {0x66,0x6C,0x78,0x70,0x78,0x6C,0x66,0x00},
    ['L'] = {0x60,0x60,0x60,0x60,0x60,0x60,0x7E,0x00},
    ['M'] = {0x63,0x77,0x7F,0x6B,0x63,0x63,0x63,0x00},
    ['N'] = {0xC6,0xE6,0xF6,0xDE,0xCE,0xC6,0xC6,0x00},
    ['O'] = {0x3C,0x66,0x66,0x66,0x66,0x66,0x3C,0x00},
    ['P'] = {0x7C,0x66,0x66,0x7C,0x60,0x60,0x60,0x00},
    ['R'] = {0x3F,0x66,0x66,0x3E,0x36,0x66,0x67,0x00},
    ['S'] = {0x3C,0x66,0x60,0x3C,0x06,0x66,0x3C,0x00},
    ['T'] = {0x7E,0x18,0x18,0x18,0x18,0x18,0x18,0x00},
    ['U'] = {0x66,0x66,0x66,0x66,0x66,0x66,0x3C,0x00},
    ['V'] = {0x66,0x66,0x66,0x66,0x66,0x3C,0x18,0x00},
    ['X'] = {0xCC,0xCC,0x78,0x30,0x78,0xCC,0xCC,0x00},
    ['Y'] = {0x66,0x66,0x66,0x3C,0x18,0x18,0x18,0x00}
};

/* ---------------- Cizim ---------------- */

void dikdortgen_ciz(int x, int y, int w, int h, uint32_t renk) {
    uint32_t stride = finfo.line_length / 4;
    for (int j = y; j < y + h; j++) {
        if (j < 0 || j >= (int)vinfo.yres) continue;
        for (int i = x; i < x + w; i++) {
            if (i < 0 || i >= (int)vinfo.xres) continue;
            backbuffer[(j * stride) + i] = renk;
        }
    }
}

void cerceve_ciz(int x, int y, int w, int h, int kalinlik, uint32_t renk) {
    dikdortgen_ciz(x, y, w, kalinlik, renk);
    dikdortgen_ciz(x, y + h - kalinlik, w, kalinlik, renk);
    dikdortgen_ciz(x, y, kalinlik, h, renk);
    dikdortgen_ciz(x + w - kalinlik, y, kalinlik, h, renk);
}

void karakter_ciz(char c, int x, int y, int olcek, uint32_t renk) {
    if (c < 0 || c >= 128) return;
    if (c >= 'a' && c <= 'z') c -= 32; /* Font yalnizca buyuk harf tanimli */
    for (int row = 0; row < 8; row++) {
        uint8_t line = font8x8[(int)c][row];
        for (int col = 0; col < 8; col++) {
            if (line & (0x80 >> col)) {
                dikdortgen_ciz(x + col * olcek, y + row * olcek, olcek, olcek, renk);
            }
        }
    }
}

void metin_ciz(const char *str, int x, int y, int olcek, uint32_t renk) {
    while (*str) {
        karakter_ciz(*str, x, y, olcek, renk);
        x += 9 * olcek;
        str++;
    }
}

/* Metnin alana sigacak en buyuk olcegini dondurur */
int metin_olcek(const char *s, int alan_w, int alan_h) {
    int ow = alan_w / (int)(9 * strlen(s) + 1);
    int oh = alan_h / 9;
    int olcek = ow < oh ? ow : oh;
    if (olcek < 1) olcek = 1;
    if (olcek > 4) olcek = 4;
    return olcek;
}

/* Metni alana en buyuk sigan olcekle ortalar */
void metin_ortala(const char *s, int x, int y, int w, int h, uint32_t renk) {
    int olcek = metin_olcek(s, w, h);
    int tw = (int)strlen(s) * 9 * olcek;
    int th = 8 * olcek;
    metin_ciz(s, x + (w - tw) / 2, y + (h - th) / 2, olcek, renk);
}

void imlec_ciz(int mx, int my) {
    dikdortgen_ciz(mx - 5, my - 5, 11, 11, RENK_YAZI);
    dikdortgen_ciz(mx - 3, my - 3, 7, 7, RENK_TEHLIKE);
}

/* ---------------- Sepet ---------------- */

static int sepet_toplam(void) {
    int t = 0;
    for (int i = 0; i < URUN_SAYISI; i++)
        if (urunler[i].adet > 0) t += urunler[i].adet * urunler[i].fiyat;
    return t;
}

static int sepet_adet(void) {
    int n = 0;
    for (int i = 0; i < URUN_SAYISI; i++)
        if (urunler[i].adet > 0) n += urunler[i].adet;
    return n;
}

static void sepet_temizle(void) {
    for (int i = 0; i < URUN_SAYISI; i++) urunler[i].adet = 0;
}

/* ---------------- Fis ---------------- */

void fis_yazdir(const Urun *u, int n) {
    char metin[1024];
    size_t off = 0;
    int toplam = 0, adet_toplam = 0;

    off += (size_t)snprintf(metin + off, sizeof(metin) - off,
        "================================\n"
        "       PATIOS AKILLI KASA       \n"
        "================================\n");
    for (int i = 0; i < n; i++) {
        if (u[i].adet <= 0) continue;
        toplam += u[i].adet * u[i].fiyat;
        adet_toplam += u[i].adet;
        if (off > sizeof(metin) - 64) break;
        off += (size_t)snprintf(metin + off, sizeof(metin) - off,
                                "%2dx %-7s %5d TL\n",
                                u[i].adet, u[i].ad, u[i].adet * u[i].fiyat);
    }
    off += (size_t)snprintf(metin + off, sizeof(metin) - off,
        "--------------------------------\n"
        "TOPLAM             %6d TL\n"
        "ADET               %6d\n"
        "DURUM           ONAYLANDI\n"
        "================================\n\n\n",
        toplam, adet_toplam);

    int prn = open("/dev/usb/lp0", O_WRONLY);
    if (prn >= 0) {
        write(prn, "\x1B\x40", 2);        /* ESC @ : yaziciyi baslat */
        write(prn, metin, strlen(metin));
        write(prn, "\x1D\x56\x00", 3);    /* GS V : kagit kes */
        close(prn);
    } else {
        /* Yazici yoksa fişi dosyaya yedekle */
        FILE *f = fopen("/data/son_fis.txt", "w");
        if (f) {
            fprintf(f, "%s", metin);
            fclose(f);
        }
    }
}

/* ---------------- Girdi ---------------- */

/* /dev/input altinda dokunmatik veya mouse aygiti arar.
 * Oncelik: duz ABS_X/ABS_Y dokunmatik > ABS_MT_POSITION coklu dokunma > mouse.
 * Bulamazsa -1 doner. */
int girdi_aygit_bul(void) {
    DIR *d = opendir("/dev/input");
    struct dirent *e;
    int dokun_fd = -1;  /* ABS_X/ABS_Y bildiren dokunmatik/tablet */
    int mt_fd = -1;     /* ABS_MT_POSITION_X/Y (tip B coklu dokunma) */
    int mouse_fd = -1;

    if (!d) return -1;

    while ((e = readdir(d)) != NULL) {
        int fd;
        char yol[272];
        unsigned char tipler[(EV_MAX + 7) / 8] = {0};
        unsigned char abs_kodlar[(ABS_MAX + 7) / 8] = {0};
        int ev_abs, ev_rel, ev_key;

        if (strncmp(e->d_name, "event", 5) != 0) continue;
        snprintf(yol, sizeof(yol), "/dev/input/%s", e->d_name);
        fd = open(yol, O_RDONLY | O_NONBLOCK);
        if (fd < 0) continue;

        ioctl(fd, EVIOCGBIT(0, sizeof(tipler)), tipler);
        ev_abs = tipler[EV_ABS / 8] & (1 << (EV_ABS % 8));
        ev_rel = tipler[EV_REL / 8] & (1 << (EV_REL % 8));
        ev_key = tipler[EV_KEY / 8] & (1 << (EV_KEY % 8));

        if (ev_abs) {
            ioctl(fd, EVIOCGBIT(EV_ABS, sizeof(abs_kodlar)), abs_kodlar);
            int has_xy =
                (abs_kodlar[ABS_X / 8] & (1 << (ABS_X % 8))) &&
                (abs_kodlar[ABS_Y / 8] & (1 << (ABS_Y % 8)));
            int has_mt =
                (abs_kodlar[ABS_MT_POSITION_X / 8] & (1 << (ABS_MT_POSITION_X % 8))) &&
                (abs_kodlar[ABS_MT_POSITION_Y / 8] & (1 << (ABS_MT_POSITION_Y % 8)));

            if (has_xy && dokun_fd < 0) { dokun_fd = fd; continue; }
            if (has_mt && mt_fd < 0)    { mt_fd = fd; continue; }
        }

        /* Mouse: REL_X/REL_Y + tus bildiren aygit -> yedek olarak acik tut */
        if (ev_rel && ev_key && mouse_fd < 0) {
            unsigned char rel_kodlar[(REL_MAX + 7) / 8] = {0};
            ioctl(fd, EVIOCGBIT(EV_REL, sizeof(rel_kodlar)), rel_kodlar);
            if ((rel_kodlar[REL_X / 8] & (1 << (REL_X % 8))) &&
                (rel_kodlar[REL_Y / 8] & (1 << (REL_Y % 8)))) {
                mouse_fd = fd;
                continue;
            }
        }

        close(fd);
    }

    closedir(d);

    /* Secilen oncelik disindaki acik tanimlayicilari kapat */
    if (dokun_fd >= 0) {
        if (mt_fd >= 0) close(mt_fd);
        if (mouse_fd >= 0) close(mouse_fd);
        return dokun_fd;
    }
    if (mt_fd >= 0) {
        if (mouse_fd >= 0) close(mouse_fd);
        return mt_fd;
    }
    return mouse_fd;
}

/* ---------------- Ekran ---------------- */

void ekran_ciz(int mx, int my, int frame) {
    int W = (int)vinfo.xres, H = (int)vinfo.yres;
    int pad = W / 60;  if (pad < 4) pad = 4;
    int ust = H / 9;   if (ust < 36) ust = 36;
    int alt = H / 8;   if (alt < 44) alt = 44;
    int sol_w = (W * 62) / 100;
    char yazi[32];

    dikdortgen_ciz(0, 0, W, H, RENK_ARKAPLAN);

    /* Baslik: sol baslik, sag canli toplam */
    dikdortgen_ciz(0, 0, W, ust, RENK_KART);
    metin_ortala("PATIOS KASA V0.2", 0, 0, W / 2, ust, RENK_VURGU);
    snprintf(yazi, sizeof(yazi), "TOPLAM: %d TL", sepet_toplam());
    metin_ortala(yazi, W / 2, 0, W / 2, ust, RENK_YAZI);

    /* Urun izgarasi: 3 sutun x 2 satir */
    int gx = pad, gy = ust + pad;
    int gw = sol_w - 2 * pad;
    int gh = H - alt - gy - pad;
    int hucre_w = (gw - 2 * pad) / 3;
    int hucre_h = (gh - pad) / 2;

    for (int i = 0; i < URUN_SAYISI; i++) {
        int cx = gx + (i % 3) * (hucre_w + pad);
        int cy = gy + (i / 3) * (hucre_h + pad);
        Urun *u = &urunler[i];
        u->x = cx; u->y = cy; u->w = hucre_w; u->h = hucre_h;

        dikdortgen_ciz(cx, cy, hucre_w, hucre_h, u->adet > 0 ? RENK_KART_SEC : RENK_KART);
        if (frame - u->son_vurus < 4)
            cerceve_ciz(cx, cy, hucre_w, hucre_h, 2, RENK_VURGU);

        /* Sol serit: adet azaltma */
        int eksi_w = (hucre_w * 28) / 100;
        dikdortgen_ciz(cx, cy, eksi_w, hucre_h, RENK_EKSI);
        metin_ortala("-", cx, cy, eksi_w, hucre_h, u->adet > 0 ? RENK_YAZI : RENK_SONUK);

        /* Ad ve fiyat */
        metin_ortala(u->ad, cx + eksi_w + 4, cy + hucre_h / 8,
                     hucre_w - eksi_w - 8, hucre_h * 3 / 8, RENK_YAZI);
        snprintf(yazi, sizeof(yazi), "%d TL", u->fiyat);
        metin_ortala(yazi, cx + eksi_w + 4, cy + hucre_h / 2,
                     hucre_w - eksi_w - 8, hucre_h * 3 / 8, RENK_VURGU);

        /* Sag ust adet rozeti */
        if (u->adet > 0) {
            snprintf(yazi, sizeof(yazi), "x%d", u->adet);
            int ro = metin_olcek(yazi, hucre_w / 3, hucre_h / 4);
            metin_ciz(yazi, cx + hucre_w - (int)strlen(yazi) * 9 * ro - 6, cy + 6, ro, RENK_VURGU);
        }
    }

    /* Sepet paneli */
    int px = sol_w + pad, py = gy;
    int pw = W - px - pad, ph = gh;
    dikdortgen_ciz(px, py, pw, ph, RENK_KART);
    metin_ortala("SEPET", px, py + 4, pw, ph / 8, RENK_YAZI);

    int so = metin_olcek("00 SIMIT 99999", pw - 20, 24);
    if (so < 1) so = 1;
    int son_y = py + ph - 8 - 2 * 11 * so;   /* Alt bilgiler icin baslangic */
    int y = py + ph / 8 + 8;
    int dolu = 0;
    for (int i = 0; i < URUN_SAYISI; i++) {
        if (urunler[i].adet <= 0) continue;
        if (y + 11 * so > son_y) break;      /* Panel dolduysa listeleme */
        snprintf(yazi, sizeof(yazi), "%dx %s", urunler[i].adet, urunler[i].ad);
        metin_ciz(yazi, px + 10, y, so, RENK_YAZI);
        snprintf(yazi, sizeof(yazi), "%d", urunler[i].adet * urunler[i].fiyat);
        metin_ciz(yazi, px + pw - (int)strlen(yazi) * 9 * so - 10, y, so, RENK_SONUK);
        y += 11 * so;
        dolu++;
    }
    if (!dolu)
        metin_ortala("SEPET BOS", px, py + ph / 3, pw, ph / 4, RENK_SONUK);

    snprintf(yazi, sizeof(yazi), "TOPLAM: %d TL", sepet_toplam());
    metin_ciz(yazi, px + 10, son_y, so, RENK_VURGU);
    snprintf(yazi, sizeof(yazi), "ADET: %d", sepet_adet());
    metin_ciz(yazi, px + 10, son_y + 11 * so, so, RENK_YAZI);

    /* Alt bar: FIS BAS + IPTAL */
    int by = H - alt + pad;
    int bh = alt - 2 * pad;
    fis_btn.x = pad; fis_btn.y = by;
    fis_btn.w = (W - 3 * pad) * 55 / 100; fis_btn.h = bh;
    iptal_btn.x = fis_btn.x + fis_btn.w + pad;
    iptal_btn.w = W - pad - iptal_btn.x;

    int sepet_dolu = sepet_adet() > 0;
    dikdortgen_ciz(fis_btn.x, fis_btn.y, fis_btn.w, fis_btn.h,
                   sepet_dolu ? RENK_VURGU : RENK_SONUK);
    metin_ortala("FIS BAS", fis_btn.x, fis_btn.y, fis_btn.w, fis_btn.h, RENK_ARKAPLAN);
    dikdortgen_ciz(iptal_btn.x, iptal_btn.y, iptal_btn.w, iptal_btn.h,
                   sepet_dolu ? RENK_TEHLIKE : RENK_SONUK);
    metin_ortala("IPTAL", iptal_btn.x, iptal_btn.y, iptal_btn.w, iptal_btn.h, RENK_YAZI);

    imlec_ciz(mx, my);
}

/* ---------------- Vurus ---------------- */

void tusa_basildi(int mx, int my, int frame) {
    /* Urun hucreleri */
    for (int i = 0; i < URUN_SAYISI; i++) {
        Urun *u = &urunler[i];
        int eksi_w = (u->w * 28) / 100;
        if (mx >= u->x && mx < u->x + u->w && my >= u->y && my < u->y + u->h) {
            u->son_vurus = frame;
            if (mx < u->x + eksi_w) {
                if (u->adet > 0) u->adet--;   /* Eksi seridi */
            } else {
                u->adet++;                    /* Urun ekle */
            }
            return;
        }
    }

    /* FIS BAS */
    if (mx >= fis_btn.x && mx < fis_btn.x + fis_btn.w &&
        my >= fis_btn.y && my < fis_btn.y + fis_btn.h) {
        if (sepet_adet() > 0) {
            fis_yazdir(urunler, URUN_SAYISI);
            sepet_temizle();
        }
        return;
    }

    /* IPTAL */
    if (mx >= iptal_btn.x && mx < iptal_btn.x + iptal_btn.w &&
        my >= iptal_btn.y && my < iptal_btn.y + iptal_btn.h) {
        sepet_temizle();
    }
}

/* ---------------- Ana ---------------- */

int main(void) {
    /* 1. Cekirdek metin konsolunu grafik moduna al (Glitch engelleme) */
    int tty_fd = open("/dev/tty0", O_RDWR);
    if (tty_fd >= 0) {
        ioctl(tty_fd, KDSETMODE, KD_GRAPHICS);
    }

    /* 2. Framebuffer ac */
    int fb = open("/dev/fb0", O_RDWR);
    if (fb < 0) {
        if (tty_fd >= 0) {
            ioctl(tty_fd, KDSETMODE, KD_TEXT);
            close(tty_fd);
        }
        return 1;
    }
    ioctl(fb, FBIOGET_VSCREENINFO, &vinfo);
    ioctl(fb, FBIOGET_FSCREENINFO, &finfo);
    screensize = vinfo.yres_virtual * finfo.line_length;
    if (screensize == 0) screensize = vinfo.yres * finfo.line_length;
    fbp = mmap(0, screensize, PROT_READ | PROT_WRITE, MAP_SHARED, fb, 0);
    if (fbp == MAP_FAILED) {
        close(fb);
        if (tty_fd >= 0) {
            ioctl(tty_fd, KDSETMODE, KD_TEXT);
            close(tty_fd);
        }
        return 1;
    }
    backbuffer = malloc(screensize);
    if (!backbuffer) {
        munmap(fbp, screensize);
        close(fb);
        if (tty_fd >= 0) {
            ioctl(tty_fd, KDSETMODE, KD_TEXT);
            close(tty_fd);
        }
        return 1;
    }

    /* 3. Girdi aygitini bul (once dokunmatik, sonra mouse), yoksa event0 */
    int ev_fd = girdi_aygit_bul();
    if (ev_fd < 0) ev_fd = open("/dev/input/event0", O_RDONLY | O_NONBLOCK);

    /* Dokunmatik eksenlerin gercek min/max araligini oku (duz ABS ve MT) */
    int eks_min_x = 0, eks_maks_x = 32767, eks_min_y = 0, eks_maks_y = 32767;
    int mt_min_x = 0, mt_maks_x = 32767, mt_min_y = 0, mt_maks_y = 32767;
    if (ev_fd >= 0) {
        struct input_absinfo ai;
        if (ioctl(ev_fd, EVIOCGABS(ABS_X), &ai) == 0) {
            eks_min_x = ai.minimum;
            eks_maks_x = ai.maximum;
        }
        if (ioctl(ev_fd, EVIOCGABS(ABS_Y), &ai) == 0) {
            eks_min_y = ai.minimum;
            eks_maks_y = ai.maximum;
        }
        if (ioctl(ev_fd, EVIOCGABS(ABS_MT_POSITION_X), &ai) == 0) {
            mt_min_x = ai.minimum;
            mt_maks_x = ai.maximum;
        }
        if (ioctl(ev_fd, EVIOCGABS(ABS_MT_POSITION_Y), &ai) == 0) {
            mt_min_y = ai.minimum;
            mt_maks_y = ai.maximum;
        }
    }

    int mx = (int)vinfo.xres / 2, my = (int)vinfo.yres / 2;
    int frame = 0;
    struct input_event ev;

    while (1) {
        if (ev_fd >= 0) {
            while (read(ev_fd, &ev, sizeof(ev)) > 0) {
                if (ev.type == EV_REL) {
                    if (ev.code == REL_X) mx += ev.value;
                    if (ev.code == REL_Y) my += ev.value;
                } else if (ev.type == EV_ABS) {
                    if (ev.code == ABS_X && eks_maks_x > eks_min_x)
                        mx = (int)(((long)(ev.value - eks_min_x) * (long)vinfo.xres) / (eks_maks_x - eks_min_x));
                    if (ev.code == ABS_Y && eks_maks_y > eks_min_y)
                        my = (int)(((long)(ev.value - eks_min_y) * (long)vinfo.yres) / (eks_maks_y - eks_min_y));
                    /* Tip B coklu dokunma panelleri */
                    if (ev.code == ABS_MT_POSITION_X && mt_maks_x > mt_min_x)
                        mx = (int)(((long)(ev.value - mt_min_x) * (long)vinfo.xres) / (mt_maks_x - mt_min_x));
                    if (ev.code == ABS_MT_POSITION_Y && mt_maks_y > mt_min_y)
                        my = (int)(((long)(ev.value - mt_min_y) * (long)vinfo.yres) / (mt_maks_y - mt_min_y));
                } else if (ev.type == EV_KEY && ev.value == 1 &&
                           (ev.code == BTN_LEFT || ev.code == BTN_TOUCH)) {
                    tusa_basildi(mx, my, frame);
                }
            }
        }

        if (mx < 0) mx = 0;
        if (my < 0) my = 0;
        if (mx >= (int)vinfo.xres) mx = (int)vinfo.xres - 1;
        if (my >= (int)vinfo.yres) my = (int)vinfo.yres - 1;

        ekran_ciz(mx, my, frame);
        memcpy(fbp, backbuffer, screensize);
        usleep(16000);
        frame++;
    }

    if (tty_fd >= 0) {
        ioctl(tty_fd, KDSETMODE, KD_TEXT);
        close(tty_fd);
    }
    free(backbuffer);
    munmap(fbp, screensize);
    close(fb);
    return 0;
}
