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

#ifndef INC_ACTIVE_OBJECT_UI_H
#define INC_ACTIVE_OBJECT_UI_H

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#include "main.h"
#include "cmsis_os.h"
#include "active_object_led.h"
#include "board.h"
#include "logger.h"
#include "dwt.h"
#include "app.h"

/* This enum defines the possible states of the button */
typedef enum
{
    BUTTON_STATE_NONE,
    BUTTON_STATE_PULSE,
    BUTTON_STATE_SHORT,
    BUTTON_STATE_LONG
} button_state_t;

typedef enum {
    LED_COLOR_RED,
    LED_COLOR_GREEN,
    LED_COLOR_BLUE
} led_color_t;

typedef struct {
    led_cmd_t cmd;
    led_color_t color;
} led_cmd_with_color_t;

/* This struct defines the UI task */
typedef struct
{
    QueueHandle_t button_state_queue; // Queue to receive button states
    LedTask_t *led_task; // LED task
} UiTask_t;

/* UI events queue */
extern QueueHandle_t ui_event_queue;

/* UI task */
extern UiTask_t ui_task;

/**
 * @brief This function creates the UI task
 * @param ui_task This is a pointer to the UI task
 */
void ui_task_create(UiTask_t *ui_task);

/**
 * @brief This function initializes the UI task
 * @param ui_task This is a pointer to the UI task
 * @param button_state_queue This is the queue to receive button states
 * @param red_task This is a pointer to the red LED task
 * @param green_task This is a pointer to the green LED task
 * @param blue_task This is a pointer to the blue LED task
 */
void ui_task_init(UiTask_t *ui_task, QueueHandle_t button_state_queue, LedTask_t *led_task);

/**
 * @brief This function runs the UI task
 * @param ui_task This is a pointer to the UI task
 */
void ui_task_run(void *argument);

#endif // ACTIVE_OBJECT_UI_H
