#include "adapter/queue.h"

#include <assert.h>
#include <errno.h>
#include <pthread.h>
#include <sys/time.h>

#include "adapter/logging.h"

#include "core/defines.h"

LOG_INIT(queue);

struct Queue {
    size_t size;
    size_t elementSize;
    size_t head;
    size_t tail;
    size_t nElements;
    void **messages;
    pthread_mutex_t mutex;
    pthread_cond_t cond;
};

static const uint64_t ONE_SEC_IN_NS = 1000 * 1000 * 1000;

Queue *Queue_New(size_t queueSize, size_t elementSize)
{
    Queue *queue = xcalloc(1, sizeof(Queue));
    queue->size = queueSize;
    queue->elementSize = elementSize;
    queue->messages = xcalloc(queueSize, sizeof(void *));

    if (pthread_mutex_init(&queue->mutex, NULL) != 0)
    {
        return NULL;
    }

    if (pthread_cond_init(&queue->cond, NULL) != 0)
    {
        return NULL;
    }

    return queue;
}

void Queue_Delete(Queue *queue)
{
    if (queue != NULL)
    {
        pthread_mutex_destroy(&queue->mutex);
        pthread_cond_destroy(&queue->cond);
        free(queue);
    }
}

ErrorCode Queue_Send(Queue *queue, void *message)
{
    assert(queue != NULL);
    assert(message != NULL);

    if (pthread_mutex_lock(&queue->mutex) != 0)
    {
        return FAILURE;
    }

    ErrorCode error = SUCCESS;
    if (queue->nElements == queue->size)
    {
        error = ERROR_RESOURCE_NOT_AVAILABLE;
        goto error;
    }

    queue->messages[queue->head] = xmalloc(queue->elementSize);
    memcpy(queue->messages[queue->head], message, queue->elementSize);

    queue->head = (queue->head + 1) % queue->size;
    queue->nElements++;

    if (queue->nElements == 1)
    {
        pthread_cond_signal(&queue->cond);
    }

error:
    pthread_mutex_unlock(&queue->mutex);

    return error;
}

ErrorCode Queue_Receive(Queue *queue, void *message, uint64_t timeoutMs)
{
    ErrorCode error = SUCCESS;

    assert(queue != NULL);
    assert(message != NULL);

    if (pthread_mutex_lock(&queue->mutex) != 0)
    {
        LOG_ERROR("cannot get mutex");

        return FAILURE;
    }

    if (queue->nElements == 0)
    {
        struct timeval tv = {0};
        assert(0 == gettimeofday(&tv, NULL));

        struct timespec tp = {
            .tv_sec = tv.tv_sec + timeoutMs / 1000,
            .tv_nsec = (tv.tv_usec + timeoutMs % 1000 * 1000) * 1000,
        };
        while (tp.tv_nsec > ONE_SEC_IN_NS)
        {
            tp.tv_sec += 1;
            tp.tv_nsec -= ONE_SEC_IN_NS;
        }

        int res = pthread_cond_timedwait(&queue->cond, &queue->mutex, &tp);
        if (res != 0)
        {
            if (res == ETIMEDOUT)
            {
                error = ERROR_TIMEOUT;
            }
            else
            {
                LOG_ERROR("pthread_cond_timedwait = %d", res);
                error = ERROR_OS;
            }
            goto error;
        }
    }

    if (queue->nElements == 0)
    {
        error = ERROR_TIMEOUT;
    }
    else
    {
        memcpy(message, queue->messages[queue->tail], queue->elementSize);
        queue->nElements--;
        queue->tail = (queue->tail + 1) % queue->size;
    }

error:
    assert(0 == pthread_mutex_unlock(&queue->mutex));

    return error;
}