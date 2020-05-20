#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "key.h"
#include "FreeRTOS.h"
#include "task.h"

#define START_TASK_PRIO				1
#define START_TASK_SIZE				128
TaskHandle_t StartTask_Handler;
void start_task(void *pvParam);

#define LED_TASK_PRIO				2
#define LED_TASK_SIZE				50
TaskHandle_t LEDTask_Handler;
void led_task(void *pvParam);

int main(void)
{
    Cache_Enable();                 //打开L1-Cache
    HAL_Init();				        //初始化HAL库
    Stm32_Clock_Init(432,25,2,9);   //设置时钟,216Mhz 
    delay_init(216);                //延时初始化
	uart_init(115200);		        //串口初始化
    LED_Init();                     //初始化LED
	
	printf("Main\n\r");
	
	xTaskCreate((TaskFunction_t) start_task,
				(const char*)"start_task",
				(uint16_t)START_TASK_SIZE,
				(void*)NULL,
				(UBaseType_t)START_TASK_PRIO,
				(TaskHandle_t*)&StartTask_Handler);
				
	vTaskStartScheduler();
}

void start_task(void *pvParam)
{
	taskENTER_CRITICAL();
	
	xTaskCreate((TaskFunction_t) led_task,
				(const char*)"led_task",
				(uint16_t)LED_TASK_SIZE,
				(void*)NULL,
				(UBaseType_t)LED_TASK_PRIO,
				(TaskHandle_t*)&LEDTask_Handler);
				
	taskEXIT_CRITICAL();
				
	while(1)
	{
		printf("Start task\n\r");
		vTaskDelay(500);
	}
}

void led_task(void *pvParam)
{
	while(1)
	{
		LED0_Toggle;
		printf("LED task\n\r");
		vTaskDelay(500);
	}
}
