#ifndef MAUVYD_H
#define MAUVYD_H

#include <sys/types.h>

#define MAX_SERVICES 64
typedef enum {
    STATE_UNVISITED,
    STATE_VISITING,
    STATE_VISITED
} VisitState;

typedef struct {
    pid_t pid;
    char name[64];
    char location[256];
    char args_str[512];
    char depends_str[256];
    int restart;
    VisitState visit_state;
} Service;

extern Service service_table[MAX_SERVICES];
extern int service_count;

#endif
