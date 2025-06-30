#include "adapter/semaphore.h"

#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>

struct Semaphore {
    void *semaphore;
};

Semaphore *Semaphore_New(void)
{
    return (Semaphore *)xSemaphoreCreateBinary();
}

ErrorCode Semaphore_Take(Semaphore *semaphore, uint64_t timeoutMs)
{
    assert(semaphore != NULL);

    return xSemaphoreTake((SemaphoreHandle_t)semaphore, pdMS_TO_TICKS(timeoutMs)) == pdTRUE ? SUCCESS : ERROR_TIMEOUT;
}

ErrorCode Semaphore_Give(Semaphore *semaphore)
{
    assert(semaphore != NULL);

    return xSemaphoreGive((SemaphoreHandle_t)semaphore) == pdTRUE ? SUCCESS : FAILURE;
}

void Semaphore_Free(Semaphore *semaphore)
{
    vSemaphoreDelete((SemaphoreHandle_t)semaphore);
}