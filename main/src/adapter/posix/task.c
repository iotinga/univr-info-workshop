#include "adapter/task.h"

#include <assert.h>
#include <pthread.h>
#include <unistd.h>

#include "core/defines.h"
#include "core/error.h"

enum {
    MAX_TASKS = 32
};

struct Task {
    pthread_t thread;
    char name[32];
    Task *next;
};

static Task *TASKS[MAX_TASKS];

Task *Task_Create(TaskEntry *entry, const char *name, uint32_t priority, size_t stackSize, void *arg)
{
    assert(entry != NULL);
    assert(name != NULL);

    Task *result = xcalloc(1, sizeof(Task));
    strcpy(result->name, name);
    if (pthread_create(&result->thread, NULL, (void *)entry, arg) != 0)
    {
        return NULL;
    }

    for (size_t i = 0; i < MAX_TASKS; i++)
    {
        if (TASKS[i] == NULL)
        {
            TASKS[i] = result;
            break;
        }
    }

    return result;
}

void Task_Delete(Task *task)
{
    if (task == NULL)
    {
        task = Task_GetSelf();
    }

    if (task == NULL)
    {
        return;
    }

    for (size_t i = 0; i < MAX_TASKS; i++)
    {
        if (TASKS[i] == task)
        {
            TASKS[i] = NULL;
            break;
        }
    }

    pthread_cancel(task->thread);
    free(task);
}

void Task_SleepMs(uint32_t time)
{
    usleep(((uint64_t)time) * 1000);
}

Task *Task_GetSelf(void)
{
    pthread_t self = pthread_self();

    for (size_t i = 0; i < MAX_TASKS; i++)
    {
        if (TASKS[i] != NULL && TASKS[i]->thread == self)
        {
            return TASKS[i];
        }
    }

    return NULL;
}

const char *Task_GetName(Task *task)
{
    if (task == NULL)
    {
        task = Task_GetSelf();
    }

    if (task == NULL)
    {
        return NULL;
    }

    return task->name;
}

ErrorCode Task_SetPriority(Task *task, uint32_t priority)
{
    return SUCCESS;
}