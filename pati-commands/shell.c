#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <locale.h>
#include <dirent.h>
#include <sys/stat.h>
#include <readline/readline.h>
#include <readline/history.h>

#define ARG_MAX 64

static int is_builtin(const char *cmd) {
    return strcmp(cmd, "exit") == 0 || strcmp(cmd, "cd") == 0;
}

static void exec_builtin(char **args) {
    if (strcmp(args[0], "pati") == 0) {
        printf("meraba. :)\n");
    }
    if (strcmp(args[0], "cd") == 0) {
        const char *path = args[1] ? args[1] : getenv("HOME");
        if (chdir(path) != 0) perror("cd");
    }
}

static int parse_args(char *line, char **argv, int max) {
    int argc = 0;
    char *p = line;
    while (*p && argc < max - 1) {
        while (*p == ' ') p++;
        if (*p == '\0') break;
        if (*p == '"' || *p == '\'') {
            char quote = *p++;
            argv[argc] = p;
            while (*p && *p != quote) p++;
            if (*p) *p++ = '\0';
        } else {
            argv[argc] = p;
            while (*p && *p != ' ') p++;
            if (*p) *p++ = '\0';
        }
        argc++;
    }
    argv[argc] = NULL;
    return argc;
}

static char *command_generator(const char *text, int state) {
    static char **path_dirs, **p;
    static size_t text_len;

    if (!state) {
        text_len = strlen(text);
        char *path = getenv("PATH");
        if (!path || !path[0]) return NULL;
        char *dup = strdup(path);
        if (!dup) return NULL;
        path_dirs = NULL;
        int count = 0;
        char *dir = strtok(dup, ":");
        while (dir) {
            path_dirs = realloc(path_dirs, (count + 1) * sizeof(char *));
            path_dirs[count++] = strdup(dir);
            dir = strtok(NULL, ":");
        }
        free(dup);
        path_dirs = realloc(path_dirs, (count + 1) * sizeof(char *));
        path_dirs[count] = NULL;
        p = path_dirs;
    }

    while (p && *p) {
        DIR *d = opendir(*p);
        p++;
        if (!d) continue;
        struct dirent *e;
        while ((e = readdir(d)) != NULL) {
            if (strncmp(e->d_name, text, text_len) != 0) continue;
            if (e->d_name[0] == '.' && text[0] != '.') continue;
            char full[1024];
            snprintf(full, sizeof(full), "%s/%s", *(p - 1), e->d_name);
            struct stat st;
            if (stat(full, &st) == 0 && (st.st_mode & S_IXUSR)) {
                closedir(d);
                return strdup(e->d_name);
            }
        }
        closedir(d);
    }
    if (!state) {
        p = NULL;
        if (path_dirs) {
            char **q = path_dirs;
            while (*q) free(*q++);
            free(path_dirs);
            path_dirs = NULL;
        }
    }
    return NULL;
}

static char **completion(const char *text, int start, int end) {
    rl_attempted_completion_over = 1;
    if (start == 0)
        return rl_completion_matches(text, command_generator);
    return NULL;
}

int main(void) {
    setlocale(LC_ALL, "");
    setbuf(stdout, NULL);
    setbuf(stderr, NULL);
    using_history();
    rl_attempted_completion_function = completion;

    char *line;
    char *argv[ARG_MAX];

    while (1) {
        char cwd[256], prompt[512];
        if (!getcwd(cwd, sizeof(cwd))) strcpy(cwd, "?");
        char *home = getenv("HOME");
        if (home && strncmp(cwd, home, strlen(home)) == 0) {
            size_t hlen = strlen(home);
            if (cwd[hlen] == '/' || cwd[hlen] == '\0')
                snprintf(prompt, sizeof(prompt),
                    "\n\1\033[32m\2pati-2.6\1\033[0m\2@\1\033[34m\2~%s\1\033[0m\2> ", cwd + hlen);
            else
                snprintf(prompt, sizeof(prompt),
                    "\n\1\033[32m\2pati-2.6\1\033[0m\2@\1\033[34m\2%s\1\033[0m\2> ", cwd);
        } else {
            snprintf(prompt, sizeof(prompt),
                "\n\1\033[32m\2pati-2.6\1\033[0m\2@\1\033[34m\2%s\1\033[0m\2> ", cwd);
        }

        line = readline(prompt);
        if (!line) { printf("\n"); break; }
        if (*line == '\0') { free(line); continue; }

        add_history(line);

        int argc = parse_args(line, argv, ARG_MAX);
        if (argc == 0) { free(line); continue; }

        if (is_builtin(argv[0])) {
            exec_builtin(argv);
            free(line);
            continue;
        }

        pid_t pid = fork();
        if (pid == 0) {
            execvp(argv[0], argv);
            fprintf(stderr, "%s komutu bulunamadı. 'yardım' komutuna bakmanı öneririm. (ve bir papatya çayı içmeni :D)\n", argv[0]);
            exit(127);
        } else if (pid > 0) {
            int status;
            wait(&status);
        } else {
            perror("fork");
        }
        free(line);
    }
    return 0;
}
