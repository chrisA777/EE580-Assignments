//---------------------------------------------------------
// main
//---------------------------------------------------------
#include "hellocfg.h"			//BIOS include file
#include "framework.h"

int16_t volatile mask = 0xffff;

uint8_t state = 0; // 0: off, 1: recording, 2: filtering

//---------------------------------------------------------
//---------------------------------------------------------
void main(void)
{
	initAll();
	return;  		// return to BIOS scheduler
}


//---------------------------------------------------------
//---------------------------------------------------------
void dipPRD(void)
{
	uint8_t dip_status1;
	uint8_t dip_status2;

	DIP_get(DIP_1, &dip_status1);
	DIP_get(DIP_2, &dip_status2);

	if(dip_status1)
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


//---------------------------------------------------------
//---------------------------------------------------------
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
                // THIS IS THE LEFT CHANNEL!!!
                write_audio_sample(s16);
            }
            break;
        case 2:
            write_audio_sample(0);
            break;
	}



}

