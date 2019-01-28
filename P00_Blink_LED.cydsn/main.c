/* ========================================
 *
 * Copyright YOUR COMPANY, THE YEAR
 * All Rights Reserved
 * UNPUBLISHED, LICENSED SOFTWARE.
 *
 * CONFIDENTIAL AND PROPRIETARY INFORMATION
 * WHICH IS THE PROPERTY OF your company.
 *
 * ========================================
*/
#include "project.h"

#define BUZZER_TIMER_CLK 8000000
uint16 buzzer_freq = 20;
uint16 buzzer_clock_div = 2;
uint16 buzzer_timer_peorid = 1;
uint16 buzzer_timer_cmp1 = 0;

uint16 buzzer_chirp_dir = 1;

int main(void)
{
    CyGlobalIntEnable; /* Enable global interrupts. */
    PWM_1_Start();
    PWM_0_Start();
    PWM_2_Start();
    

    /* Place your initialization/startup code here (e.g. MyInst_Start()) */

    for(;;)
    {
        /* Place your application code here. */
        //LED_R_Write(~KEY1_Read());
        //LED_G_Write(~KEY2_Read());
        
        if (buzzer_chirp_dir == 0) buzzer_freq--;
        if (buzzer_chirp_dir == 1) buzzer_freq++;
        
        if (buzzer_freq < 20) buzzer_chirp_dir = 1;
        else if (buzzer_freq > 20000) buzzer_chirp_dir = 0;
        else {
            // buzzer_freq = 20;
            buzzer_clock_div = BUZZER_TIMER_CLK / buzzer_freq;
            buzzer_timer_peorid = buzzer_clock_div - 1;
            buzzer_timer_cmp1 = buzzer_clock_div/2-1;
            PWM_2_WritePeriod(buzzer_timer_peorid);
            PWM_2_WriteCompare(buzzer_timer_cmp1);
            CyDelay(1);
        }
    }
}

/* [] END OF FILE */
