#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

void *scheduler(void *arg);
void schedulerdeitu();

extern pthread_mutex_t mutexS;
extern pthread_cond_t condS;
