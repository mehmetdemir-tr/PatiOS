#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <sys/mount.h>
#include <sys/ioctl.h>
#include <termios.h>
#include <fcntl.h>
#include <net/if.h>
#include <arpa/inet.h>
#include <linux/route.h>
#include <signal.h>
#include <sys/stat.h>
#include <errno.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <pthread.h>

#include "mauvyd-headers/mauvyd.h"
#include "mauvyd-headers/pcg.h"


static int term_cols(void) {
    struct winsize w;
    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &w) == 0 && w.ws_col > 0)
        return w.ws_col;
    return 80;
}

static void log_status(const char *msg, int ok) {
    int cols = term_cols();
    int pad = cols - (int)strlen(msg) - 6;
    if (pad < 1) pad = 1;

    printf("%s", msg);
    for (int i = 0; i < pad; i++) putchar(' ');

    if (ok)
        printf("[ \033[0;32mOK\033[0m ]\n");
    else
        printf("[\033[0;31mFAIL\033[0m]\n");
    fflush(stdout);
    usleep(80000);
}

#define CONFIG_DIR "/etc/mauvyd"
#define DEFAULT_PATH "/bin:/sbin:/usr/bin:/usr/sbin:/pcg-startup:/lib/paticommands"
#define HOSTNAME "pati@mobile"
#define OS_NAME "PatiOS - Yeni nesil mobil işletim sistemi"

Service service_table[MAX_SERVICES];
int service_count = 0;
sigset_t block_mask, old_mask;

void setup_network(void);
void sigchld_handler(int sig);
void* start_control_socket(void* arg);
int is_running(const char *name);
void spawn_service(int idx);
void resolve_and_start(int idx);
void start_services(struct dirent **namelist, int n);
void sync_paticommands(void);

static void boot_screen(void) {
    printf("\e[H\e[J");
    printf("\033[H\033[J");

    printf("DateTime 0:19:35 \?\?-\?\?-\?\?\?\?\n");
    printf("ACPI: PCI Root Bridge Initialized (state 0000 00ff100)\n");
    printf("Environment: authorization=\"https://api.güvenligelecek.org\", authHost=\"api.güvenligelecek.org\"\n");
    printf("Starting 1 thread(s)\n\n");
    fflush(stdout);
    usleep(150000);

    log_status("Setting clock (utc):", 1);
    log_status("Starting udev:", 1);
    printf("1 devices found\n");
    log_status("Setting up dispsclslv", 1);
    log_status("Setting up trdtil", 1);
    log_status("Started Apply Kernel Variables", 1);
    log_status("Started Kernel Device Manager", 1);
    log_status("Started Namespace Registration Service", 1);
    printf("Searching for additional items\n");
    log_status("Started initramfs", 1);
    log_status("Started SecurityService", 1);
    log_status("Reached target Local File Systems", 1);
    printf("\n");

    printf("Started File System Check on /dev/disk/uuid/63ec8ac1-9f66-44c8\n");
    printf("Started File System Check on /dev/disk/uuid/a44ed7db-39c9-44a3\n");
    log_status("File System Check Validated", 1);
    printf("\n");

    int r_proc = mount("proc", "/proc", "proc", 0, NULL);
    int e_proc = errno;
    log_status("Mounting procfs", r_proc == 0 || e_proc == EBUSY);

    int r_sys = mount("sysfs", "/sys", "sysfs", 0, NULL);
    int e_sys = errno;
    log_status("Mounting sysfs", r_sys == 0 || e_sys == EBUSY);
    printf("\n");

    mkdir("/data", 0755);
    mkdir("/data/paticommands", 0755);
    int ret = mount("/dev/vda1", "/data", "ext4", 0, NULL);
    if (ret == 0) {
        log_status("Mounting persistent storage", 1);
        sync_paticommands();
    } else if (errno == EBUSY) {
        log_status("Mounting persistent storage", 1);
    } else {
        log_status("Mounting persistent storage", 0);
    }
    putenv("PATH=" DEFAULT_PATH);

    mkdir("/dev/imeidata/efs_current", 0755);
    mkdir("/dev/imeidata/efs_backup", 0755);
    int r_efs1 = mount("/dev/vda2", "/dev/imeidata/efs_current", "ext4", 0, NULL);
    int e_efs1 = errno;
    log_status("Mounting EFS active storage", r_efs1 == 0 || e_efs1 == EBUSY);

    int r_efs2 = mount("/dev/vda3", "/dev/imeidata/efs_backup", "ext4", MS_RDONLY, NULL);
    int e_efs2 = errno;
    log_status("Mounting EFS backup storage (ro)", r_efs2 == 0 || e_efs2 == EBUSY);
    printf("\n");

    putenv("TERM=linux");
    sethostname(HOSTNAME, strlen(HOSTNAME));

    printf("Pati-2.1 by Mehmet Demir. Kod adi: Ananas (Pineapple)\n");
    printf("----------------------------------------\n");
    printf("----- MAUVYD Configuration System -----\n");
    printf("----------------------------------------\n\n");
    fflush(stdout);
    usleep(150000);
}

void setup_network() {
    int fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (fd < 0) { perror("socket"); return; }

    struct ifreq ifr;
    memset(&ifr, 0, sizeof(ifr));
    strncpy(ifr.ifr_name, "eth0", IFNAMSIZ);
    ifr.ifr_flags = IFF_UP | IFF_RUNNING;
    ioctl(fd, SIOCSIFFLAGS, &ifr);

    struct sockaddr_in *addr = (struct sockaddr_in *)&ifr.ifr_addr;
    addr->sin_family = AF_INET;
    inet_pton(AF_INET, "10.0.2.15", &addr->sin_addr);
    ioctl(fd, SIOCSIFADDR, &ifr);

    inet_pton(AF_INET, "255.255.255.0", &addr->sin_addr);
    ioctl(fd, SIOCSIFNETMASK, &ifr);

    struct rtentry route;
    memset(&route, 0, sizeof(route));
    struct sockaddr_in *gw = (struct sockaddr_in *)&route.rt_gateway;
    gw->sin_family = AF_INET;
    inet_pton(AF_INET, "10.0.2.2", &gw->sin_addr);
    struct sockaddr_in *dst = (struct sockaddr_in *)&route.rt_dst;
    dst->sin_family = AF_INET;
    dst->sin_addr.s_addr = INADDR_ANY;
    struct sockaddr_in *mask = (struct sockaddr_in *)&route.rt_genmask;
    mask->sin_family = AF_INET;
    mask->sin_addr.s_addr = INADDR_ANY;
    route.rt_flags = RTF_UP | RTF_GATEWAY;
    route.rt_dev = "eth0";
    ioctl(fd, SIOCADDRT, &route);
    close(fd);

    FILE *resolv = fopen("/etc/resolv.conf", "w");
    if (resolv) {
        fprintf(resolv, "nameserver 8.8.8.8\n");
        fclose(resolv);
    }
    printf("[+]: Network aktif (10.0.2.15).\n");
}

void sync_paticommands() {
    DIR *d = opendir("/data/paticommands");
    if (!d) { printf("[BILGI]: /data/paticommands bulunamadi.\n"); return; }
    struct dirent *ent;
    while ((ent = readdir(d)) != NULL) {
        if (ent->d_name[0] == '.') continue;
        char kaynak[512], hedef[512];
        snprintf(kaynak, sizeof(kaynak), "/data/paticommands/%s", ent->d_name);
        snprintf(hedef, sizeof(hedef), "/lib/paticommands/%s", ent->d_name);
        struct stat st;
        if (stat(kaynak, &st) == 0 && S_ISREG(st.st_mode)) {
            int in = open(kaynak, O_RDONLY);
            int out = open(hedef, O_WRONLY | O_CREAT | O_TRUNC, 0755);
            if (in >= 0 && out >= 0) {
                char buf[4096]; ssize_t n;
                while ((n = read(in, buf, sizeof(buf))) > 0) write(out, buf, n);
                printf("[TAMAM]: %s kopyalandi.\n", ent->d_name);
            }
            if (in >= 0) close(in);
            if (out >= 0) close(out);
        }
    }
    closedir(d);
}

void sigchld_handler(int sig) {
    (void)sig;
    int status;
    pid_t dead_pid;
    while ((dead_pid = waitpid(-1, &status, WNOHANG)) > 0) {
        for (int i = 0; i < service_count; i++) {
            if (service_table[i].pid == dead_pid && service_table[i].restart) {
                sleep(1);
                pid_t new_pid = fork();
                if (new_pid == 0) {
                    char *argv[32] = {0};
                    argv[0] = service_table[i].location;
                    char tmp[512];
                    strncpy(tmp, service_table[i].args_str, 511);
                    char *token = strtok(tmp, " ");
                    int j = 1;
                    while (token && j < 31) { argv[j++] = token; token = strtok(NULL, " "); }
                    execv(service_table[i].location, argv);
                    exit(1);
                }
                service_table[i].pid = new_pid;
            }
        }
    }
}

void* start_control_socket(void* arg) {
    (void)arg;
    int fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (fd < 0) return NULL;

    struct sockaddr_un addr;
    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, "/run/mauvyd.sock", sizeof(addr.sun_path) - 1);
    addr.sun_path[sizeof(addr.sun_path) - 1] = '\0';

    unlink("/run/mauvyd.sock");
    mkdir("/run", 0755);

    if (bind(fd, (struct sockaddr*)&addr, sizeof(addr)) < 0) return NULL;
    listen(fd, 5);

    while (1) {
        int client = accept(fd, NULL, NULL);
        if (client < 0) continue;

        char buf[256] = {0};
        ssize_t n = read(client, buf, sizeof(buf) - 1);
        if (n <= 0) { close(client); continue; }
        buf[n] = '\0';
        if (buf[n-1] == '\n') buf[n-1] = '\0';

        if (strcmp(buf, "status") == 0) {
            for (int i = 0; i < service_count; i++) {
                char line[512];
                int alive = (service_table[i].pid > 0 && kill(service_table[i].pid, 0) == 0);
                snprintf(line, sizeof(line), "%s (%s): %s (pid %d) restart=%s\n",
                    service_table[i].name,
                    service_table[i].location,
                    alive ? "running" : "dead",
                    service_table[i].pid,
                    service_table[i].restart ? "on" : "off");
                write(client, line, strlen(line));
            }

        } else if (strncmp(buf, "start ", 6) == 0) {
            char *name = buf + 6;
            if (*name == '\0') {
                write(client, "Error: service name required.\n", 30);
                close(client); continue;
            }
            int idx = -1;
            for (int i = 0; i < service_count; i++)
                if (strcmp(service_table[i].name, name) == 0) { idx = i; break; }
            if (idx < 0) {
                char resp[512]; snprintf(resp, sizeof(resp), "Service '%s' not found.\n", name);
                write(client, resp, strlen(resp));
            } else if (service_table[idx].pid > 0 && kill(service_table[idx].pid, 0) == 0) {
                char resp[512]; snprintf(resp, sizeof(resp), "%s is already running (pid %d).\n", name, service_table[idx].pid);
                write(client, resp, strlen(resp));
            } else {
                spawn_service(idx);
                char resp[512]; snprintf(resp, sizeof(resp), "Starting %s (pid %d)...\n", name, service_table[idx].pid);
                write(client, resp, strlen(resp));
            }

        } else if (strncmp(buf, "stop ", 5) == 0) {
            char *name = buf + 5;
            if (*name == '\0') {
                write(client, "Error: service name required.\n", 30);
                close(client); continue;
            }
            int found = 0;
            for (int i = 0; i < service_count; i++) {
                if (strcmp(service_table[i].name, name) == 0) {
                    if (service_table[i].pid > 0) {
                        kill(service_table[i].pid, SIGTERM);
                        service_table[i].pid = 0;
                    }
                    char resp[512]; snprintf(resp, sizeof(resp), "Stopping %s..\n", name);
                    write(client, resp, strlen(resp));
                    found = 1; break;
                }
            }
            if (!found) {
                char resp[512]; snprintf(resp, sizeof(resp), "Service '%s' not found.\n", name);
                write(client, resp, strlen(resp));
            }

        } else if (strncmp(buf, "restart ", 9) == 0) {
            char *name = buf + 9;
            if (*name == '\0') {
                write(client, "Error: service name required.\n", 30);
                close(client); continue;
            }
            int idx = -1;
            for (int i = 0; i < service_count; i++)
                if (strcmp(service_table[i].name, name) == 0) { idx = i; break; }
            if (idx < 0) {
                char resp[512]; snprintf(resp, sizeof(resp), "Service '%s' not found.\n", name);
                write(client, resp, strlen(resp));
            } else {
                if (service_table[idx].pid > 0) {
                    kill(service_table[idx].pid, SIGTERM);
                    usleep(100000);
                }
                spawn_service(idx);
                char resp[512]; snprintf(resp, sizeof(resp), "Restarting %s (pid %d)...\n", name, service_table[idx].pid);
                write(client, resp, strlen(resp));
            }

        } else if (strncmp(buf, "log ", 4) == 0) {
            char *name = buf + 4;
            if (*name == '\0') {
                write(client, "Error: service name required.\n", 30);
                close(client); continue;
            }
            char logpath[256];
            snprintf(logpath, sizeof(logpath), "/tmp/%s.log", name);
            FILE *lf = fopen(logpath, "r");
            if (!lf) {
                char resp[512]; snprintf(resp, sizeof(resp), "No log file for '%s'.\n", name);
                write(client, resp, strlen(resp));
            } else {
                char lbuf[4096];
                size_t r;
                while ((r = fread(lbuf, 1, sizeof(lbuf), lf)) > 0)
                    write(client, lbuf, r);
                fclose(lf);
            }

        } else if (strncmp(buf, "enable ", 7) == 0) {
            char *name = buf + 7;
            if (*name == '\0') {
                write(client, "Error: service name required.\n", 30);
                close(client); continue;
            }
            int found = 0;
            for (int i = 0; i < service_count; i++) {
                if (strcmp(service_table[i].name, name) == 0) {
                    service_table[i].restart = 1;
                    char cfgpath[512];
                    snprintf(cfgpath, sizeof(cfgpath), "%s/%s.pcg", CONFIG_DIR, name);
                    pcg_write(cfgpath, "restart", "1");
                    char resp[512]; snprintf(resp, sizeof(resp), "Enabled auto-restart for %s.\n", name);
                    write(client, resp, strlen(resp));
                    found = 1; break;
                }
            }
            if (!found) {
                char resp[512]; snprintf(resp, sizeof(resp), "Service '%s' not found.\n", name);
                write(client, resp, strlen(resp));
            }

        } else if (strncmp(buf, "disable ", 8) == 0) {
            char *name = buf + 8;
            if (*name == '\0') {
                write(client, "Error: service name required.\n", 30);
                close(client); continue;
            }
            int found = 0;
            for (int i = 0; i < service_count; i++) {
                if (strcmp(service_table[i].name, name) == 0) {
                    service_table[i].restart = 0;
                    char cfgpath[512];
                    snprintf(cfgpath, sizeof(cfgpath), "%s/%s.pcg", CONFIG_DIR, name);
                    pcg_write(cfgpath, "restart", "0");
                    char resp[512]; snprintf(resp, sizeof(resp), "Disabled auto-restart for %s.\n", name);
                    write(client, resp, strlen(resp));
                    found = 1; break;
                }
            }
            if (!found) {
                char resp[512]; snprintf(resp, sizeof(resp), "Service '%s' not found.\n", name);
                write(client, resp, strlen(resp));
            }

        } else {
            write(client, "Available: status, start <s>, stop <s>, restart <s>, log <s>, enable <s>, disable <s>\n", 83);
        }

        close(client);
    }
    return NULL;
}

void start_services(struct dirent **namelist, int n) {
    for (int i = 0; i < n; i++) {
        struct dirent *entry = namelist[i];
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            free(namelist[i]); continue;
        }
        char service_name[64] = {0};
        strncpy(service_name, entry->d_name, sizeof(service_name) - 1);
        char *dot = strrchr(service_name, '.');
        if (dot) *dot = '\0';
        char fulldst[512];
        snprintf(fulldst, sizeof(fulldst), "%s/%s", CONFIG_DIR, entry->d_name);
        char location[256] = {0}, wait_str[16] = {0}, watch_str[16] = {0};
        char args_str[512] = {0}, depends_str[256] = {0}, restart_str[16] = {0};
        pcg_read(fulldst, "location", location, sizeof(location));
        pcg_read(fulldst, "wait", wait_str, sizeof(wait_str));
        pcg_read(fulldst, "watch", watch_str, sizeof(watch_str));
        pcg_read(fulldst, "args", args_str, sizeof(args_str));
        pcg_read(fulldst, "depends", depends_str, sizeof(depends_str));
        pcg_read(fulldst, "restart", restart_str, sizeof(restart_str));
        if (location[0] != '/') { free(namelist[i]); continue; }
        if (service_count >= MAX_SERVICES) { free(namelist[i]); continue; }
        service_table[service_count].pid = 0;
        strncpy(service_table[service_count].name, service_name, 63);
        strncpy(service_table[service_count].location, location, 255);
        strncpy(service_table[service_count].args_str, args_str, 511);
        strncpy(service_table[service_count].depends_str, depends_str, 255);
        service_table[service_count].restart = (strcmp(restart_str, "1") == 0);
        service_table[service_count].visit_state = STATE_UNVISITED;
        service_count++;
        if (strcmp(watch_str, "1") == 0)
            printf("[INFO] %s Karabaş tarafından izlenecek.\n", location);
        free(namelist[i]);
    }
    sigemptyset(&block_mask);
    sigaddset(&block_mask, SIGCHLD);
    for (int i = 0; i < service_count; i++)
        if (service_table[i].visit_state == STATE_UNVISITED)
            resolve_and_start(i);
}

void spawn_service(int idx) {
    sigprocmask(SIG_BLOCK, &block_mask, &old_mask);
    pid_t pid = fork();
    if (pid == 0) {
        sigprocmask(SIG_SETMASK, &old_mask, NULL);
        char fulldst[512];
        snprintf(fulldst, sizeof(fulldst), "%s/%s.pcg", CONFIG_DIR, service_table[idx].name);
        char watch_str[16] = {0};
        pcg_read(fulldst, "watch", watch_str, sizeof(watch_str));
        if (strcmp(watch_str, "1") == 0) {
            setsid();
            int ttyfd = open("/dev/ttyAMA0", O_RDWR | O_NOCTTY);
            if (ttyfd < 0) ttyfd = open("/dev/console", O_RDWR | O_NOCTTY);
            if (ttyfd >= 0) {
                ioctl(ttyfd, TIOCSCTTY, 0);
                dup2(ttyfd, STDIN_FILENO);
                dup2(ttyfd, STDOUT_FILENO);
                dup2(ttyfd, STDERR_FILENO);
                if (ttyfd > 2) close(ttyfd);
            }
        } else {
            char logpath[256];
            snprintf(logpath, sizeof(logpath), "/tmp/%s.log", service_table[idx].name);
            int logfd = open(logpath, O_WRONLY | O_CREAT | O_APPEND, 0644);
            if (logfd >= 0) {
                dup2(logfd, STDOUT_FILENO);
                dup2(logfd, STDERR_FILENO);
                close(logfd);
            }
        }
        char *argv[32] = {0};
        argv[0] = service_table[idx].location;
        if (service_table[idx].args_str[0] != '\0') {
            char *token = strtok(service_table[idx].args_str, " ");
            int j = 1;
            while (token && j < 31) { argv[j++] = token; token = strtok(NULL, " "); }
        }
        execv(service_table[idx].location, argv);
        perror("execv failed");
        exit(1);
    } else if (pid > 0) {
        service_table[idx].pid = pid;
        sigprocmask(SIG_SETMASK, &old_mask, NULL);
        char fulldst[512];
        snprintf(fulldst, sizeof(fulldst), "%s/%s.pcg", CONFIG_DIR, service_table[idx].name);
        char wait_str[16] = {0};
        pcg_read(fulldst, "wait", wait_str, sizeof(wait_str));
        if (strcmp(wait_str, "1") == 0) {
            int status;
            waitpid(pid, &status, 0);
        }
    }
}

int is_running(const char *name) {
    for (int i = 0; i < service_count; i++) {
        if (strcmp(service_table[i].name, name) == 0) {
            if (service_table[i].pid <= 0) return 0;
            return (kill(service_table[i].pid, 0) == 0);
        }
    }
    return 0;
}

void resolve_and_start(int idx) {
    if (service_table[idx].visit_state == STATE_VISITING) {
        printf("[!!!] Circular dependency at: %s\n", service_table[idx].name);
        return;
    }
    if (service_table[idx].visit_state == STATE_VISITED) return;
    service_table[idx].visit_state = STATE_VISITING;
    if (service_table[idx].depends_str[0] != '\0') {
        int dep_found = -1;
        for (int i = 0; i < service_count; i++)
            if (strcmp(service_table[i].name, service_table[idx].depends_str) == 0) { dep_found = i; break; }
        if (dep_found != -1) {
            resolve_and_start(dep_found);
            int retries = 0;
            while (!is_running(service_table[idx].depends_str) && retries < 20) {
                usleep(50000); retries++;
            }
        } else {
            printf("[!!!] Dependency '%s' for '%s' not found.\n",
                   service_table[idx].depends_str, service_table[idx].name);
        }
    }
    printf("[+]: Starting: %s\n", service_table[idx].name);
    spawn_service(idx);
    service_table[idx].visit_state = STATE_VISITED;
}

int main() {
    struct dirent **namelist;
    int n = scandir(CONFIG_DIR, &namelist, NULL, alphasort);

    boot_screen();

    setup_network();

    struct sigaction sa;
    sa.sa_handler = sigchld_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART | SA_NOCLDSTOP;
    sigaction(SIGCHLD, &sa, NULL);

    pthread_t socket_thread;
    if (pthread_create(&socket_thread, NULL, start_control_socket, NULL) == 0)
        pthread_detach(socket_thread);

    if (n >= 0) {
        start_services(namelist, n);
        free(namelist);
    }

    while (1) {
        pause();
    }
    return 0;
}
