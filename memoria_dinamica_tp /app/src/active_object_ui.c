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

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#include "active_object_ui.h"
#include "main.h"
#include "cmsis_os.h"
#include "board.h"
#include "logger.h"
#include "dwt.h"
#include "app.h"


QueueHandle_t ui_event_queue;

QueueHandle_t led_r_event_queue;
QueueHandle_t led_g_event_queue;
QueueHandle_t led_b_event_queue;
/* ============================================================================================ */

void ui_task_init(UiTask_t *ui_task, QueueHandle_t button_state_queue, LedTask_t *red_task, LedTask_t *green_task, LedTask_t *blue_task) 
{
    ui_task->button_state_queue = button_state_queue;
    ui_task->red_task = red_task;
    ui_task->green_task = green_task;
    ui_task->blue_task = blue_task;
}

/* ============================================================================================ */

void ui_task_run(void *argument)
{
	UiTask_t* ui_task = (UiTask_t*)argument;
    button_state_t button_state;
    while (true) 
    {
        /* Wait for a button state change */
        if (xQueueReceive(ui_task->button_state_queue, &button_state, portMAX_DELAY) == pdPASS) 
        {
            // switch (button_state) 
            // {
            //     /* Handle the button state */
            //     case BUTTON_STATE_PULSE:
            //         LOGGER_INFO("Button pulse detected");
            //         xQueueSend(ui_task->red_task->queue, &(led_cmd_t){LED_CMD_ON}, portMAX_DELAY);
            //         break;
            //     case BUTTON_STATE_SHORT:
            //         LOGGER_INFO("Button short press detected");
            //         xQueueSend(ui_task->green_task->queue, &(led_cmd_t){LED_CMD_ON}, portMAX_DELAY);
            //         break;
            //     case BUTTON_STATE_LONG:
            //         LOGGER_INFO("Button long press detected");
            //         xQueueSend(ui_task->blue_task->queue, &(led_cmd_t){LED_CMD_ON}, portMAX_DELAY);
            //         break;
            //     default:
            //         break;
            // }
            switch (button_state) 
            {
                case BUTTON_STATE_PULSE:
                    LOGGER_INFO("Button pulse detected");
                    create_led_task(&led_r_event_queue, led_red_set_state, "Red LED Task");
                    xQueueSend(led_r_event_queue, &(led_cmd_t){LED_CMD_ON}, portMAX_DELAY);
                    destroy_led_task(ui_task->red_task, led_r_event_queue);
                    break;
                case BUTTON_STATE_SHORT:
                    LOGGER_INFO("Button short press detected");
                    create_led_task(&led_g_event_queue, led_green_set_state, "Green LED Task");
                    xQueueSend(led_g_event_queue, &(led_cmd_t){LED_CMD_ON}, portMAX_DELAY);
                    destroy_led_task(ui_task->green_task, led_g_event_queue);
                    break;
                case BUTTON_STATE_LONG:
                    LOGGER_INFO("Button long press detected");
                    create_led_task(&led_b_event_queue, led_blue_set_state, "Blue LED Task");
                    xQueueSend(led_b_event_queue, &(led_cmd_t){LED_CMD_ON}, portMAX_DELAY);
                    destroy_led_task(ui_task->blue_task, led_b_event_queue);
                    break;
                default:
                    break;
            }
        }
    }
}

/* ============================================================================================ */

void ui_task_create(UiTask_t *ui_task) 
{
    /* Create the UI event queue */
    ui_event_queue = xQueueCreate(10, sizeof(button_state_t));

    /* Initialize the UI task */
    ui_task_init(ui_task, ui_event_queue, &red_task, &green_task, &blue_task);

    /* Create the UI task */
    BaseType_t status;

    status = xTaskCreate(ui_task_run, "UI Task", configMINIMAL_STACK_SIZE, ui_task, tskIDLE_PRIORITY, NULL);
    configASSERT(pdPASS == status);
}

/* ============================================================================================ */
