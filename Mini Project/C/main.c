//#include "hellocfg.h"
#include "stdio.h"
#include <stdlib.h>
#include "types.h"
#include "evmc6748.h"
#include "evmc6748_timer.h"
#include "evmc6748_i2c.h"
#include "evmc6748_led.h"
#include "evmc6748_dip.h"
#include "framework.h"
/*
 *  ======== main ========
 */

uint8_t state = 0; //0: off, 1: recording, 2: filtering


void main(void)
{
    initAll();
    /* fall into DSP/BIOS idle loop */
    return;
}

void dipPRD(void)
{
    // Can we get DIP status with HWI??
    uint8_t dip_status1;
    uint8_t dip_status2;

    DIP_get(DIP_1, &dip_status1);
    DIP_get(DIP_2, &dip_status2);

    if (dip_status1)
    {
        if (dip_status2)
        {
            state = 2;
        }
        else
        {
            state = 1;
        }
    }
    else
    {
        state = 0;
    }
}

void ledPRD(void)
{
    if (state == 1)
    {
        LED_toggle(LED_1);
        LED_toggle(LED_2);
    }
}

void audioHWI(void)
{
    int16_t s16;
    s16 = read_audio_sample();

    switch(state)
    {
        case 0:
            write_audio_sample(0);
            break;
        case 1:
            if (MCASP->RSLOT)
            {
                // Left audio channel
                write_audio_sample(s16);
            }
            else
            {
                // right audio channel
                // Need to output something?
                write_audio_sample(s16);
            }
            break;
        case 2:
            write_audio_sample(0);
            break;
    }
}
