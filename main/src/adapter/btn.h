#pragma once

#include "adapter/queue.h"

#include "core/error.h"

/**
 * Initialize the button module and hw
 *
 * @param evenQueue event queue to send the button press events
 */
ErrorCode Btn_Init(Queue *eventQueue);