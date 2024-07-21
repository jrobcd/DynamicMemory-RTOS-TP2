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

/* ============================================================================================ */

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#include "main.h"
#include "cmsis_os.h"
#include "board.h"
#include "logger.h"
#include "dwt.h"
#include "app.h"
#include "active_object_led.h"


memory_pool_t led_task_pool;
/* ============================================================================================ */

void led_task_init(LedTask_t *task, QueueHandle_t queue, void (*set_state)(led_cmd_t cmd)) 
{
    task->queue = queue;
    task->set_state = set_state;
}

/* ============================================================================================ */

void led_task_run(LedTask_t *task) 
{
    led_cmd_t cmd;
    while (true) 
    {
        if (xQueueReceive(task->queue, &cmd, portMAX_DELAY) == pdPASS) 
        {
            task->set_state(cmd);
            if (cmd == LED_CMD_ON) 
            {
                vTaskDelay(pdMS_TO_TICKS(1000));
                task->set_state(LED_CMD_OFF);
            }
        }
    }
}

/* ============================================================================================ */

void led_red_set_state(led_cmd_t cmd) 
{
    HAL_GPIO_WritePin(LED_RED_PORT, LED_RED_PIN, cmd == LED_CMD_ON ? GPIO_PIN_SET : GPIO_PIN_RESET);
}

/* ============================================================================================ */

void led_green_set_state(led_cmd_t cmd) 
{
    HAL_GPIO_WritePin(LED_GREEN_PORT, LED_GREEN_PIN, cmd == LED_CMD_ON ? GPIO_PIN_SET : GPIO_PIN_RESET);
}

/* ============================================================================================ */

void led_blue_set_state(led_cmd_t cmd) 
{
    HAL_GPIO_WritePin(LED_BLUE_PORT, LED_BLUE_PIN, cmd == LED_CMD_ON ? GPIO_PIN_SET : GPIO_PIN_RESET);
}

/* ============================================================================================ */

LedTask_t *allocate_led_task(void)
{
    return (LedTask_t *)memory_pool_block_get(&led_task_pool);
}

/* ============================================================================================ */

void free_led_task(LedTask_t *task)
{
    memory_pool_block_put(&led_task_pool, task);
}

/* ============================================================================================ */

void create_led_task(QueueHandle_t *event_queue, void (*set_state)(led_cmd_t cmd), const char *task_name)
{
    LedTask_t *task = allocate_led_task();
    if (task == NULL) {
        LOGGER_INFO("Failed to allocate memory for LED task");
        return;
    }

    *event_queue = xQueueCreate(10, sizeof(led_cmd_t));
    if (*event_queue == NULL) {
    	LOGGER_INFO("Failed to create queue for LED task");
        free_led_task(task);
        return;
    }

    led_task_init(task, *event_queue, set_state);

    if (xTaskCreate((TaskFunction_t)led_task_run, task_name, LED_TASK_STACK_SIZE, task, tskIDLE_PRIORITY, NULL) != pdPASS) {
    	LOGGER_INFO("Failed to create LED task");
        vQueueDelete(*event_queue);
        free_led_task(task);
    }
}

/* ============================================================================================ */

void destroy_led_task(LedTask_t *task, QueueHandle_t event_queue)
{
    if (task != NULL) {
        vQueueDelete(event_queue);
        vTaskDelete(NULL);
        free_led_task(task);
    }
}

/* ============================================================================================ */
