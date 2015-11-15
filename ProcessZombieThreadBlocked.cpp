#define LOG_TAG "ProcessZombieThreadBlocked"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <pthread.h>

int thread_init = 0;
pthread_mutex_t thread_init_mutex;
pthread_cond_t thread_init_cond;

void *thread_routine(void *)
{
    void *ptr;
    int fd, ret;

    pthread_mutex_lock(&thread_init_mutex);
    thread_init++;
    pthread_cond_signal(&thread_init_cond);
    pthread_mutex_unlock(&thread_init_mutex);

    printf("%d:%d, %s:%d\n", getpid(), gettid(), __func__, __LINE__);

    fd = open("/sys/kernel/debug/hung/mutex", O_WRONLY);
    if (fd < 0) {
        printf("failed to open /sys/kernel/debug/hung/mutex, error = %s\n", strerror(errno));
        return NULL;
    }

    ret = write(fd, "1", 1);
    if (ret < 0) {
        printf("failed to wrtiet to /sys/kernel/debug/hung/mutex, error = %s\n", strerror(errno));
        return NULL;
    }

    printf("%d:%d, %s:%d\n", getpid(), gettid(), __func__, __LINE__);

    return NULL;
}

int main()
{
    pthread_t thread;
    int ret, status;
    pid_t pid;

    printf("%d:%d, process-zombie-thread-blocked starts\n", getpid(), gettid());
    pthread_mutex_init(&thread_init_mutex, NULL);
    pthread_cond_init(&thread_init_cond, NULL);

    ret = pthread_create(&thread, NULL, thread_routine, NULL);
    if (ret < 0) {
        printf("failed to pthread_create, error = %s\n", strerror(errno));
        return 1;
    }

    pthread_mutex_lock(&thread_init_mutex);
    while (thread_init < 1)
        pthread_cond_wait(&thread_init_cond, &thread_init_mutex);
    pthread_mutex_unlock(&thread_init_mutex);

    usleep(5000000);

    printf("%d:%d, process-zombie-thread-blocked ends\n", getpid(), gettid());

    return 0;
}
