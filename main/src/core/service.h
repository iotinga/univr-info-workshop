#pragma once

#include <stdbool.h>
#include <stddef.h>

#include "adapter/queue.h"
#include "adapter/semaphore.h"
#include "adapter/task.h"

#include "core/error.h"

/* Service forward declaration */
struct Service;

typedef void ServiceMethod(struct Service *self);
typedef void ServiceEventHandler(struct Service *self, uint32_t event, uint32_t payload);

typedef enum ServiceState {
    SERVICE_STOPPED,
    SERVICE_WAITING,
    SERVICE_RUNNING,
} ServiceState;

typedef struct Service {
    /** Name of the service */
    const char *name;

    /** Service task priority */
    uint32_t taskPriority;

    /** Service stack size */
    size_t taskStackSize;

    /** Handle of the service queue */
    Queue *queue;

    /** Handle of the service task */
    Task *task;

    /** time to wait for service message reception */
    uint64_t timeoutMs;

    /** state of this task */
    ServiceState state;

    /** hook that is called right after starting the thread and before invoking main/processing events */
    ServiceMethod *setupHook;

    /** hook that is called when the main exits */
    ServiceMethod *teardownHook;

    /** main function of the task. Provided in alternative to handler */
    ServiceMethod *main;

    /** event handling function, to use this set main to NULL */
    ServiceEventHandler *handler;

    /** Semaphore that is unlocked on service termination */
    Semaphore *terminationSemaphore;
} Service;

/** Event sent to the service queue */
typedef struct {
    /** Event type */
    uint32_t event;

    /** Event payload, if present */
    uint32_t payload;
} ServiceEvent;

#define EVENT_TIMEOUT 0xafaf
#define EVENT_STOP    0xafad
#define isTIMEOUT(e)  (e.type == EVENT_TIMEOUT)

#define SERVICE_QUEUE_SIZE 20u

/*
 * USAGE:
 * // in .h
 * SERVICE_DECLARE(SERVICE_NAME);
 *
 * // in .c
 * SERVICE_DEFINE(SERVICE_NAME) {
 *   .name = "TEST",
 *   .main = Main,
 *   // ...
 * };
 */
#define SERVICE_DECLARE(__name) extern Service *__name
#define SERVICE_DEFINE(__name)                                                                                         \
    extern Service g_##__name;                                                                                         \
    Service *__name = &g_##__name;                                                                                     \
    Service g_##__name =

/**
 * Starts the specified service (if not already started)
 */
void Service_Start(Service *service);

/**
 * Stops (kills) the specified service.
 */
void Service_Stop(Service *service);

/**
 * Sends an event to a specified service.
 */
void Service_Notify(Service *service, uint32_t event, uint32_t payload);
