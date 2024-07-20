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

#include "app.h"
#include "active_object_led.h"

//QueueHandle_t led_r_event_queue;
//QueueHandle_t led_g_event_queue;
//QueueHandle_t led_b_event_queue;
QueueHandle_t led_event_queue;

void led_red_set_state(led_cmd_t cmd);
void led_green_set_state(led_cmd_t cmd);
void led_blue_set_state(led_cmd_t cmd);

/* ============================================================================================ */

void led_task_init(LedTask_t *task, QueueHandle_t queue, void (*set_state)(led_cmd_t cmd))
{
    task->queue = queue;
    task->set_state = set_state;
}

/* ============================================================================================ */

void led_task_run(LedTask_t *task) {
    led_cmd_with_color_t cmd_with_color;

    while (true) {
        if (xQueueReceive(task->queue, &cmd_with_color, portMAX_DELAY) == pdPASS) {
            switch (cmd_with_color.color) {
                case LED_COLOR_RED:
                    led_red_set_state(cmd_with_color.cmd);
                    break;
                case LED_COLOR_GREEN:
                    led_green_set_state(cmd_with_color.cmd);
                    break;
                case LED_COLOR_BLUE:
                    led_blue_set_state(cmd_with_color.cmd);
                    break;
                default:
                    break;
            }
            vTaskDelete(NULL);
        }
    }
}


/* ============================================================================================ */

/* Modify this function depending on the HW */
void led_red_set_state(led_cmd_t cmd) {
    HAL_GPIO_WritePin(LED_RED_PORT, LED_RED_PIN, cmd == LED_CMD_ON ? GPIO_PIN_SET : GPIO_PIN_RESET);
}

void led_green_set_state(led_cmd_t cmd) {
    HAL_GPIO_WritePin(LED_GREEN_PORT, LED_GREEN_PIN, cmd == LED_CMD_ON ? GPIO_PIN_SET : GPIO_PIN_RESET);
}

void led_blue_set_state(led_cmd_t cmd) {
    HAL_GPIO_WritePin(LED_BLUE_PORT, LED_BLUE_PIN, cmd == LED_CMD_ON ? GPIO_PIN_SET : GPIO_PIN_RESET);
}


/* ============================================================================================ */

void led_tasks_create(LedTask_t *led_task) {
    led_event_queue = xQueueCreate(10, sizeof(led_cmd_with_color_t));
    led_task_init(led_task, led_event_queue, NULL); // set_state function is no longer needed here

    xTaskCreate((TaskFunction_t)led_task_run, "LED Task", configMINIMAL_STACK_SIZE, led_task, tskIDLE_PRIORITY, NULL);
}


/* ============================================================================================ */
