#include "ppm.h"
#include "main.h"

uint16_t PPM_Sample_Cnt = 0;//通道
uint16_t PPM_Chn_Max = 8;//最大通道数
uint32_t PPM_Time = 0;//获取通道时间
uint16_t PPM_OK = 0;//下一次解析状态,初始为0, 个人感觉第一次下降沿应该会大于2050

//读一段数据, 计时20ms ; 20ms到后储存并读下一段数据, 数组序号加一
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    if(GPIO_Pin == GPIO_PIN_0)
    {
        // 移除所有Delay函数
        HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13);

        // 进入临界区（可选）
        __disable_irq();
        PPM_Time = TIM2->CNT;
        TIM2->CNT = 0;

        if(PPM_OK){
            if(PPM_Sample_Cnt < PPM_Chn_Max){
                PPM_Databuf[PPM_Sample_Cnt] = PPM_Time;
                PPM_Sample_Cnt++;
            }
            if(PPM_Sample_Cnt >= PPM_Chn_Max){
                PPM_OK = 0;
                ppm_data_ready = 1;
            }
        }

        if(PPM_Time >= 2050){
            PPM_OK = 1;
            PPM_Sample_Cnt = 0;
        }
        __enable_irq(); // 退出临界区
    }
}


