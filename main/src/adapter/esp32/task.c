#include "adapter/task.h"

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#include "core/error.h"

struct Task {
    void *task;
};

Task *Task_Create(TaskEntry *entry, const char *name, uint32_t priority, size_t stackSize, void *arg)
{
    assert(entry != NULL);
    assert(name != NULL);

    TaskHandle_t result = NULL;
    BaseType_t status = xTaskCreate((TaskFunction_t)entry, name, stackSize, arg, priority, &result);
    if (status != pdPASS)
    {
        return NULL;
    }

    return (Task *)result;
}

void Task_Delete(Task *task)
{
    vTaskDelete((TaskHandle_t)task);
}

void Task_SleepMs(uint32_t time)
{
    vTaskDelay(pdMS_TO_TICKS(time));
}

Task *Task_GetSelf(void)
{
    return (Task *)xTaskGetCurrentTaskHandle();
}

const char *Task_GetName(Task *task)
{
    return pcTaskGetName((TaskHandle_t)task);
}

ErrorCode Task_SetPriority(Task *task, uint32_t priority)
{
    vTaskPrioritySet((TaskHandle_t)task, priority);

    return SUCCESS;
}