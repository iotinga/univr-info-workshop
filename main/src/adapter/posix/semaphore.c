#include "adapter/semaphore.h"

#include <assert.h>
#include <errno.h>
#include <pthread.h>
#include <sys/time.h>
#include <unistd.h>

#include "adapter/logging.h"

#include "core/defines.h"
#include "core/error.h"

LOG_INIT(semaphore);

struct Semaphore {
    pthread_mutex_t mutex;
    pthread_cond_t cond;
    volatile bool locked;
};

static const uint64_t ONE_SEC_IN_NS = 1000 * 1000 * 1000;

Semaphore *Semaphore_New(void)
{
    Semaphore *sem = xcalloc(1, sizeof(Semaphore));
    sem->locked = true;

    if (pthread_mutex_init(&sem->mutex, NULL) != 0)
    {
        return NULL;
    }

    if (pthread_cond_init(&sem->cond, NULL) != 0)
    {
        return NULL;
    }

    return sem;
}

static uint64_t Semaphore_ElapsedTimeMs(struct timeval *start)
{
    struct timeval tv = {0};
    assert(0 == gettimeofday(&tv, NULL));

    uint64_t startMs = start->tv_sec * 1000 + start->tv_usec / 1000 / 1000;
    uint64_t endMs = tv.tv_sec * 1000 + tv.tv_usec / 1000 / 1000;

    return endMs - startMs;
}

ErrorCode Semaphore_Take(Semaphore *semaphore, uint64_t timeoutMs)
{
    assert(semaphore != NULL);

    assert(0 == pthread_mutex_lock(&semaphore->mutex));

    struct timeval tv = {0};
    assert(0 == gettimeofday(&tv, NULL));

    ErrorCode error = SUCCESS;
    while (semaphore->locked && Semaphore_ElapsedTimeMs(&tv) < timeoutMs)
    {
        struct timespec tp = {
            .tv_sec = tv.tv_sec + timeoutMs / 1000,
            .tv_nsec = (tv.tv_usec + (timeoutMs % 1000) * 1000) * 1000,
        };
        while (tp.tv_nsec >= ONE_SEC_IN_NS)
        {
            tp.tv_sec += 1;
            tp.tv_nsec -= ONE_SEC_IN_NS;
        }

        int res = pthread_cond_timedwait(&semaphore->cond, &semaphore->mutex, &tp);
        if (res != 0)
        {
            if (res == ETIMEDOUT)
            {
                LOG_DEBUG("timeout elapsed %" PRIu64, Semaphore_ElapsedTimeMs(&tv));
                error = ERROR_TIMEOUT;
            }
            else
            {
                LOG_ERROR("pthread_cond_timedwait %d", res);
                error = ERROR_OS;
            }
            goto error;
        }
    }

    /* NOTE: POSIX does not provide warranties for the fact that here the semaphore is unlocked.
     * We need to check the condition again. */
    if (!semaphore->locked)
    {
        semaphore->locked = true;
    }
    else
    {
        error = ERROR_TIMEOUT;
    }

error:
    assert(0 == pthread_mutex_unlock(&semaphore->mutex));

    return error;
}

ErrorCode Semaphore_Give(Semaphore *semaphore)
{
    assert(semaphore != NULL);

    assert(0 == pthread_mutex_lock(&semaphore->mutex));

    ErrorCode error = SUCCESS;
    if (semaphore->locked)
    {
        semaphore->locked = false;
        assert(pthread_cond_signal(&semaphore->cond) == 0);
    }

    assert(0 == pthread_mutex_unlock(&semaphore->mutex));

    return error;
}

void Semaphore_Free(Semaphore *semaphore)
{
    if (semaphore != NULL)
    {
        pthread_cond_destroy(&semaphore->cond);
        pthread_mutex_destroy(&semaphore->mutex);
        free(semaphore);
    }
}