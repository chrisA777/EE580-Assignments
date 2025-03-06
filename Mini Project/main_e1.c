//---------------------------------------------------------
// main example 1
//---------------------------------------------------------
#include "hellocfg.h"			//BIOS include file
#include "framework.h"


//---------------------------------------------------------
//---------------------------------------------------------
void main(void)
{
	initAll();
	return;  		// return to BIOS scheduler
}


//---------------------------------------------------------
//---------------------------------------------------------
void audioHWI(void)
{
	int16_t s16;

	s16 = read_audio_sample();
	if (MCASP->RSLOT)
	{
		// THIS IS THE LEFT CHANNEL!!!
		//s16 = process(s16);
	}
	else {
		// THIS IS THE RIGHT CHANNEL!!!
		//s16 = process(s16);
	}
	write_audio_sample(s16);
}

