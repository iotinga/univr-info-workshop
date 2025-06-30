#include "adapter/queue.h"

#include <freertos/FreeRTOS.h>
#include <freertos/queue.h>

#include "adapter/logging.h"

LOG_INIT(queue)

struct Queue {
    void *queue;
};

Queue *Queue_New(size_t queueSize, size_t elementSize)
{
    return (Queue *)xQueueCreate(queueSize, elementSize);
}

void Queue_Delete(Queue *queue)
{
    if (queue != NULL)
    {
        vQueueDelete((QueueHandle_t)queue);
    }
}

ErrorCode Queue_Send(Queue *queue, void *message)
{
    assert(queue != NULL);
    assert(message != NULL);

    BaseType_t result = xQueueSend((QueueHandle_t)queue, message, (TickType_t)0);
    if (result != pdTRUE)
    {
        LOG_ERROR("queue send fail %d", result);
        if (result == errQUEUE_FULL)
        {
            LOG_ERROR("queue is full");
        }

        return FAILURE;
    }

    return SUCCESS;
}

ErrorCode Queue_Receive(Queue *queue, void *message, uint64_t timeoutMs)
{
    assert(queue != NULL);
    assert(message != NULL);

    TickType_t ticks = timeoutMs == QUEUE_MAX_TIMEOUT ? portMAX_DELAY : pdMS_TO_TICKS(timeoutMs);
    BaseType_t result = xQueueReceive((QueueHandle_t)queue, message, ticks);

    return (result == pdTRUE) ? SUCCESS : FAILURE;
}