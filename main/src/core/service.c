#include "core/service.h"

#include "adapter/logging.h"
#include "adapter/queue.h"
#include "adapter/task.h"

#include "core/defines.h"
#include "core/error.h"

LOG_INIT(service)

static void Service_Main(void *arg)
{
    Service *service = arg;

    if (service->setupHook != NULL)
    {
        LOG_DEBUG("%s: calling setup hook", service->name);
        service->setupHook(service);
    }

    if (service->main != NULL)
    {
        service->main(service);
        LOG_ERROR("service: %s terminated unexpectedly", service->name);
    }
    else
    {
        while (service->state == SERVICE_RUNNING)
        {
            ServiceEvent event = {.event = EVENT_TIMEOUT};
            Queue_Receive(service->queue, &event, service->timeoutMs);

            if (service->handler != NULL)
            {
                service->handler(service, event.event, event.payload);
            }
        }
    }

    if (service->teardownHook)
    {
        LOG_DEBUG("%s calling teardown hook", service->name);
        service->teardownHook(service);
    }

    /* signal service termination */
    ON_ERROR_REBOOT(Semaphore_Give(service->terminationSemaphore));

    /* terminate service */
    LOG_INFO("service %s terminated, deleting task", service->name);
    Task_Delete(service->task);
    Queue_Delete(service->queue);
    memset(&service->queue, 0, sizeof(service->queue));

    /* should not reach this, wait for termination */
    while (true)
    {
        Task_SleepMs(1000);
    }
}

void Service_Start(Service *service)
{
    if (service->state != SERVICE_STOPPED)
    {
        LOG_WARNING("service: %s already running!", service->name);
    }
    else
    {
        LOG_INFO("service: starting %s...", service->name);
        service->state = SERVICE_RUNNING;

        if (service->taskStackSize == 0u)
        {
            service->taskStackSize = TASK_DEFAULT_STACK_SIZE;
        }

        if (service->taskPriority == 0u)
        {
            service->taskPriority = TASK_DEFAULT_PRIORITY;
        }

        service->queue = Queue_New(SERVICE_QUEUE_SIZE, sizeof(ServiceEvent));
        ON_NULL_REBOOT(service->queue);
        service->task =
            Task_Create(Service_Main, service->name, service->taskPriority, service->taskStackSize, service);
        ON_NULL_REBOOT(service->task);
    }
}

void Service_Stop(Service *service)
{
    if (service->state == SERVICE_STOPPED)
    {
        LOG_WARNING("%s already stopped!", service->name);
    }
    else
    {
        LOG_INFO("stopping %s...!", service->name);
        service->state = SERVICE_STOPPED;
    }
}

void Service_Notify(Service *service, uint32_t event, uint32_t payload)
{
    if (service->state != SERVICE_STOPPED)
    {

        LOG_VERBOSE("%s notify event %" PRIu32 " %" PRIu32, service->name, event, payload);

        ServiceEvent serviceEvent = {
            .event = event,
            .payload = payload,
        };

        ErrorCode error = Queue_Send(service->queue, &serviceEvent);
        if (error != SUCCESS)
        {
            LOG_ERROR("queue is full");
            LOG_DEBUG("last queue events are:");
            while (Queue_Receive(service->queue, &serviceEvent, 10) == SUCCESS)
            {
                LOG_DEBUG("  - %" PRIu32 " %" PRIu32, serviceEvent.event, serviceEvent.payload);
            }

            ON_ERROR_REBOOT(error);
        }
    }
}