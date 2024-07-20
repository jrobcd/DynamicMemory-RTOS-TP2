/*
 * Copyright (c) 2023 Sebastian Bedin <sebabedin@gmail.com>.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its
 *    contributors may be used to endorse or promote products derived from
 *    this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 * @author : Sebastian Bedin <sebabedin@gmail.com>
 */

#ifndef INC_ACTIVE_OBJECT_LED_H
#define INC_ACTIVE_OBJECT_LED_H

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#include "main.h"
#include "cmsis_os.h"
#include "board.h"
#include "logger.h"
#include "dwt.h"
#include "app.h"

/* This enum defines the possible commands for the LED */
typedef enum
{
    LED_CMD_OFF,
    LED_CMD_ON
} led_cmd_t;

/* This struct defines the LED task */
typedef struct
{
    QueueHandle_t queue; // Queue to receive commands
    void (*set_state)(led_cmd_t cmd); // Function pointer to set the LED state
} LedTask_t;

/* LED events queues */
//extern QueueHandle_t led_r_event_queue;
//extern QueueHandle_t led_g_event_queue;
//extern QueueHandle_t led_b_event_queue;
extern QueueHandle_t led_event_queue;

/* LED tasks */
//extern LedTask_t red_task;
//extern LedTask_t green_task;
//extern LedTask_t blue_task;
extern LedTask_t led_task;

/**
 * @brief This function creates the LED tasks
 * @param red_task This is a pointer to the red LED task
 * @param green_task This is a pointer to the green LED task
 * @param blue_task This is a pointer to the blue LED task
 */
void led_tasks_create(LedTask_t *led_task);

/**
 * @brief This function initializes the LED task
 * @param task This is a pointer to the LED task
 * @param queue This is the queue to receive commands
 * @param set_state This is a function pointer to set the LED state
 */
void led_task_init(LedTask_t *task, QueueHandle_t queue, void (*set_state)(led_cmd_t cmd));

/**
 * @brief This function runs the LED task
 * @param task This is a pointer to the LED task
 */
void led_task_run(LedTask_t *task);

#endif // ACTIVE_OBJECT_LED_H
