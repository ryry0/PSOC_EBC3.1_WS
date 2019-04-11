/**
* @file main.c
* @brief example main file of the CWRU Stimulation Library (CwruStimLib)
* @author Lu Li - lxl361@case.edu
*
* @version 1.0a
* @date 08/11/2017
*/

#include <project.h>
#include <stdio.h>
#include <math.h> //May need to add 'm' flag in GCC
#include "common.h"

// CwruStimLib Related
#include"CwruStimLib/CwruStimLib.h"

/*******************************************************************************
* Function Name: main()
********************************************************************************
* Summary:
*  Main function for the project.
*
* Theory:
*  The function starts UART and interrupt components.
*
*******************************************************************************/
int main()
{
    uint8 button = 0u;
    uint8 buttonPre = 0u;

    // CwruStimLib Related
    int8_t sucessStatus = 1u;
    cwru_stim_struct_t cwru_stim_brd1;
    cwru_stim_struct_t cwru_stim_brd2;

    uint8_t cmd = 0;
    uint8_t mode = 0;
    uint8_t setting = 0;
    float param = 0;

    LED_Write(LED_OFF);     /* Clear LED */

    CyGlobalIntEnable;      /* Enable global interrupts. */

    // CwruStimLib Related
    sucessStatus *= cwru_stim_init(&cwru_stim_brd1,
                                STIM_BRD_PERC,
                                STIM_UART_PORT_0,
                                STIM_SETTING_DEFAULT);

    sucessStatus *= cwru_stim_init(&cwru_stim_brd2,
                                STIM_BRD_PERC,
                                STIM_UART_PORT_1,
                                STIM_SETTING_DEFAULT);

    // Call start func to send sync message to brds
    //sucessStatus *= cwru_stim_start(cwru_stim_struct_t *cwru_stim, const uint8_t sync_signal)
    sucessStatus *= cwru_stim_start(&cwru_stim_brd1, UECU_SYNC_MSG);
    sucessStatus *= cwru_stim_start(&cwru_stim_brd2, UECU_SYNC_MSG);

    for(;;)
    {
        if(sucessStatus >= 0u)
        {
            /* Indicate an error on the LED */
            LED_Write(LED_ON);
        }

        // CwruStimLib Related

        // Example 1: update IPI or PW or AMP based on preset pattern
        //          and gait percentage value from GED
        cmd = STIM_UPDATE_GAIT_CYCLE;
        mode = VCK5_BRD1|PW; // (Board type with id) with (IPI or PW or AMP)
        setting = PATTERN_STAND;
        param = 5000; // 0-10000 for 0%-100% gait cycle
        // update board 1, based on VCK5 Board 1 Stand pattern, change PW value
        sucessStatus *= cwru_stim_update(&cwru_stim_brd1, cmd, mode, setting, param);

        cmd = STIM_UPDATE_GAIT_CYCLE;
        mode = VCK5_BRD2|PW; // (Board type with id) with (IPI or PW or AMP)
        setting = PATTERN_STAND;
        param = 5000; // 0-10000 for 0%-100% gait cycle
        // update board 2, based on VCK5 Board 2 Stand pattern, change PW value
        sucessStatus *= cwru_stim_update(&cwru_stim_brd2, cmd, mode, setting, param);

        // Example 1: update IPI or PW or AMP based on preset pattern
        //          and gait percentage value from GED
        cmd = STIM_UPDATE_GAIN;
        mode = PW; // PW or AMP
        setting = 1; // Channel ID 0-STIM_CHANNEL_USED
        param = 0.5; // 0-1 for float for 0%-100% of gain factor
        // update board 1, change channel 1 PW gain factor to 50%.
        sucessStatus *= cwru_stim_update(&cwru_stim_brd1, cmd, mode, setting, param);

        // Flash LED 20ms
        LED_Write(~LED_Read());
        CyDelay(20);

        /***********************************************************************
        * Handle SW2 press.
        ***********************************************************************/
        button = SW2_Read();
        if((button == 0u) && (buttonPre != 0u))
        {
            /* Clear LED */
            LED_Write(LED_OFF);

            /* Clear error status */
            sucessStatus = 1u;
        }
        buttonPre = button;
    } // end for(;;)
}

/* [] END OF FILE */
