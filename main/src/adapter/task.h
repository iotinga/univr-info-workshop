#pragma once

#include <stddef.h>
#include <stdint.h>

#include "core/error.h"

/**
 * \file osal_task.h
 * OSAL for the task/threads
 */

/**
 * Task entrypoint
 *
 * \param arg the argument that is passed when creating the task
 */
typedef void TaskEntry(void *arg);

/** Default task priority */
static const uint32_t TASK_DEFAULT_PRIORITY = 1;

/** Maximum task priority */
static const uint32_t TASK_MAX_PRIORITY = 24;

/** Default stack size in bytes */
static const size_t TASK_DEFAULT_STACK_SIZE = 4096;

/** Opaque task handle */
typedef struct Task Task;

/**
 * Creates and starts a new task
 *
 * \param entry task entrypoint
 * \param name task name
 * \param priority task priority
 * \param stackSize dimension of the stack in bytes
 * \param arg argument that is passed to the task entrypoint
 * \returns the task handle, or NULL in case of error
 */
Task *Task_Create(TaskEntry *entry, const char *name, uint32_t priority, size_t stackSize, void *arg);

/**
 * Stops and deletes a task
 *
 * \param task the task handle. If NULL deletes the self task
 */
void Task_Delete(Task *task);

/**
 * Gets the handle of the task calling this function
 *
 * \returns the task handle for the task calling the function
 */
Task *Task_GetSelf(void);

/**
 * Suspend the calling task for at least the specified amount of milliseconds
 *
 * \param time amount of milliseconds to sleep
 */
void Task_SleepMs(uint32_t time);

/**
 * Gets the name of the specified task handle
 *
 * \param task task handle. If NULL returns the name of the current task.
 * \returns the task name, NULL in case of error
 */
const char *Task_GetName(Task *task);