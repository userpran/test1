#include<reg51.h>
#include<stdio.h>
sbit dd=P1^0;	
void delay()
{
	TMOD =0x01;			/* Set Timer0 in 16-bit timer mode */
	TH0 = 0x00; 		/* Load higher 8-bit in TH0 */
	TL0 = 0x2E;			/* Load lower 8-bit in TL0 */
	TR0 = 1;			/* Start Timer0 */
	while(TF0 == 0);	/* Wait until Timer0 overflow flag (TF0) is set */
	TR0 = 0;			/* Stop Timer0 */
	TF0 = 0;			/* Clear Timer0 overflow flag */
  
}

void main()
{
	
	while (1)
		{
			dd=1;
			delay();
			dd=0;
			delay();
		}

}