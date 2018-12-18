/******************************** includes ***********************************/
#include "platform_config.h"

/*****************************************************************************/

/****************************** functions ************************************/
void Delay(unsigned int nCount)
{
    while (--nCount);
}

void Delay_us(unsigned int time_us)			/* time delay for us in 72MHz */
{
    register unsigned int i;

    for (i = 0; i < time_us; i++)
    {
        asm("NOP");
        asm("NOP");
        asm("NOP");
        asm("NOP");
        asm("NOP");				// 5
        asm("NOP");
        asm("NOP");
        asm("NOP");
        asm("NOP");
        asm("NOP");				// 10
        asm("NOP");
        asm("NOP");
        asm("NOP");
        asm("NOP");
        asm("NOP");				// 15
        asm("NOP");
        asm("NOP");
        asm("NOP");
        asm("NOP");
        asm("NOP");				// 20
        asm("NOP");
        asm("NOP");
        asm("NOP");
        asm("NOP");
        asm("NOP");				// 25
        asm("NOP");
        asm("NOP");
        asm("NOP");
        asm("NOP");
        asm("NOP");				// 30
        asm("NOP");
        asm("NOP");
        asm("NOP");
        asm("NOP");
        asm("NOP");				// 35
        asm("NOP");
        asm("NOP");
        asm("NOP");
        asm("NOP");
        asm("NOP");				// 40
        asm("NOP");
        asm("NOP");
        asm("NOP");
        asm("NOP");
        asm("NOP");				// 45
        asm("NOP");
        asm("NOP");
        asm("NOP");
        asm("NOP");
        asm("NOP");				// 50
        asm("NOP");
        asm("NOP");
        asm("NOP");
        asm("NOP");
        asm("NOP");				// 55
        asm("NOP");
        asm("NOP");
        asm("NOP");
        asm("NOP");
        asm("NOP");				// 60
        asm("NOP");
        asm("NOP");
        asm("NOP");
        asm("NOP");
        asm("NOP");				// 65
        asm("NOP");
    }
}

void Delay_ms(unsigned int time_ms)			/* time delay for ms in 72MHz */
{
    register unsigned int i;

    for (i = 0; i < time_ms; i++)
    {
        Delay_us(1000);
    }
}
