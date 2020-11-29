#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "key.h"
#include "FreeRTOS.h"
#include "task.h"
//#include "FreeRTOSWrapper.h"

#define START_TASK_PRIO     1
#define LED_TASK_PRIO       2
#define TEST1_TASK_PRIO     3
#define TEST2_TASK_PRIO     4

#define START_TASK_SIZE     128
#define LED_TASK_SIZE       50
#define TEST1_TASK_SIZE     128
#define TEST2_TASK_SIZE     128

TaskHandle_t StartTask_Handler;
void start_task(void *pvParam);

TaskHandle_t LEDTask_Handler;
void led_task(void *pvParam);
void test1_task(void *pvParam);
void test2_task(void *pvParam);

void TIM3_Init(void)
{
  RCC->APB1ENR |= RCC_APB1ENR_TIM3EN;
  
  TIM3->PSC = 10800-1;
  TIM3->ARR = 10000-1;
  TIM3->DIER |= TIM_DIER_UIE;
  TIM3->CR1 |= TIM_CR1_CEN;
  
  HAL_NVIC_SetPriority(TIM3_IRQn, 4, 0);
  HAL_NVIC_EnableIRQ(TIM3_IRQn);
}

void TIM5_Init(void)
{
  RCC->APB1ENR |= RCC_APB1ENR_TIM5EN;
  
  TIM5->PSC = 10800-1;
  TIM5->ARR = 10000-1;
  TIM5->DIER |= TIM_DIER_UIE;
  TIM5->CR1 |= TIM_CR1_CEN;
  
  HAL_NVIC_SetPriority(TIM5_IRQn, 5, 0);
  HAL_NVIC_EnableIRQ(TIM5_IRQn);
}

void TIM3_IRQHandler(void)
{
  if ((TIM3->SR & TIM_FLAG_UPDATE) != RESET && (TIM3->DIER & TIM_IT_UPDATE) != RESET)
  {
    TIM3->SR &= ~(TIM_FLAG_UPDATE);
    printf("TIM3 \r\n");
  }
}

void TIM5_IRQHandler(void)
{
  if ((TIM5->SR & TIM_FLAG_UPDATE) != RESET && (TIM5->DIER & TIM_IT_UPDATE) != RESET)
  {
    TIM5->SR &= ~(TIM_FLAG_UPDATE);
    printf("TIM5 \r\n");
  }
}


int main(void)
{
  Cache_Enable();                 //打开L1-Cache
  HAL_Init();                     //初始化HAL库
  Stm32_Clock_Init(432,25,2,9);   //设置时钟,216Mhz 
  delay_init(216);                //延时初始化
  uart_init(115200);              //串口初始化
  LED_Init();                     //初始化LED
  
  TIM3_Init();
  TIM5_Init();

  printf("Main \r\n");

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
        
  xTaskCreate((TaskFunction_t) test1_task,
        (const char*)"test1_task",
        (uint16_t)TEST1_TASK_SIZE,
        (void*)NULL,
        (UBaseType_t)TEST1_TASK_PRIO,
        (TaskHandle_t*)&LEDTask_Handler);
        
//  xTaskCreate((TaskFunction_t) test2_task,
//        (const char*)"test2_task",
//        (uint16_t)TEST2_TASK_SIZE,
//        (void*)NULL,
//        (UBaseType_t)TEST2_TASK_PRIO,
//        (TaskHandle_t*)&LEDTask_Handler);
        
  vTaskDelete(StartTask_Handler);
        
  taskEXIT_CRITICAL();
}

void led_task(void *pvParam)
{
  while(1)
  {
    LED0_Toggle;
    //printf("LED task\r\n");
    vTaskDelay(1000);
  }
}

void test1_task(void *pvParam)
{
  static uint32_t lTotalNum = 0;
  
  while (1)
  {
    lTotalNum++;
    if (lTotalNum == 5)
    {
      printf("Disable interrupt \r\n");
      portDISABLE_INTERRUPTS();
      delay_xms(5000);
      printf("Enable interrupt \r\n");
      portENABLE_INTERRUPTS();
    }
    vTaskDelay(1000);
  }
}

void test2_task(void *pvParam)
{
  while(1)
  {
    printf("test2 task \r\n");
    vTaskDelay(1000);
  }
}
