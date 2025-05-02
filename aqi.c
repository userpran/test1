
#include<reg51.h>
#include<stdio.h>

#define LCD_Port P1						/* P1 port as data port for LCD */
sbit rs=P1^0;							/* P1.0 as Register select pin */
sbit rw=P1^1;							/* P1.1 as R/W pin */
sbit en=P1^2;							/* P1.2 as  Enable pin */

#define ADC_DATA P3						/* P3 port as data port for ADC */

sbit oe		=P2^0;						/*P2^0	as output enable pin */						
sbit eoc	=P2^1;						/*P2^1	as end of conversion pin */
sbit soc	=P2^2;						/*P2^2	as start convertion pin */

sbit a		=P2^4;						/*P2^4	as channel select a pin */
sbit b 		=P2^5;						/*P2^5	as channel select b pin */
sbit c		=P2^6;						/*P2^6	as channel select c pin */

sbit ale	=P2^7;						/*P2^7	as address load enable pin */

//---------------------------FUNCTIONS FOR ADC -------------------------------------
/* Function to select adc channel */
void Channel_Select (unsigned int channel)
{
	switch(channel) 
	{
		case 0: c=0;	b=0;	a=0; break;			//000
		case 1: c=0;	b=0;	a=1; break;			//001
		case 2: c=0;	b=1;	a=0; break;			//010
		case 3: c=0;	b=1;	a=1; break;			//011
		case 4: c=1;	b=0;	a=0; break;			//100
		case 5: c=1;	b=0;	a=1; break;			//101
		case 6: c=1;	b=1;	a=0; break;			//110
		case 7: c=1;	b=1;	a=1; break;			//111
	}
}

/* Function to obtain the converted voltage value */
unsigned int Recive_Value(unsigned int channel)
{
	int adc_value;	
	ADC_DATA=0xFF;		/* Initialising as input port*/
	ale=soc=oe=a=b=c=0;

	eoc=1;

	Channel_Select(channel);

	/* Enabling address load enable	and starting the conversion*/
	ale=1;
	soc=1;
	ale=0;
	soc=0;

	while(eoc==1);		/* Waiting until conversion is over*/
	while(eoc==0);

	oe=1;				/* Enabling output*/
	adc_value=ADC_DATA;	/* Obtaing converted value*/
	oe=0;				/* Disabling output*/

	return adc_value;	/* Returning converted value*/
}


//-----------------------FUNCTIONS FOR LCD--------------------------------------

void delay(unsigned int count)  /* Function to provide delay */
{
  int i ;
  for(i=0;i<count;i++)
  {
	TMOD =0x01;			/* (16-bit timer mode) */
	TH0 = 0xFC; 		/* Load higher 8-bit in TH0 */
	TL0 = 0x68;			/* Load lower 8-bit in TL0 */
	TR0 = 1;			/* Start timer0 */
	while(TF0 == 0);	/* Wait until timer0 flag set */
	TR0 = 0;			/* Stop timer0 */
	TF0 = 0;			/* Clear timer0 flag */
  }
}

void LCD_Command (char cmnd)   	/* LCD16x2 command funtion */
{
	LCD_Port =(LCD_Port & 0x0F) | (cmnd & 0xF0); 	/* sending upper nibble */
	rs=0;	 										
	rw=0;											
	en=1; 
	delay(1);
	en=0;
	delay(10);

	LCD_Port = (LCD_Port & 0x0F) | (cmnd << 4);  /* sending lower nibble */
	en=1; 
	delay(1);
	en=0;
	delay(10);
}

void LCD_Char (char char_data)	/* LCD data write function */
{
	LCD_Port =(LCD_Port & 0x0F) | (char_data & 0xF0); /* sending upper nibble*/    
	rs=1;											
	rw=0;											
	en=1;   				
	delay(1);
	en=0;
	delay(10);

	LCD_Port = (LCD_Port & 0x0F) | (char_data << 4);/* sending lower nibble */
	en=1;											
	delay(1);
	en=0;
	delay(10);
}

void LCD_String (char *str)				/* Send string to LCD function */
{
	int i;
	for(i=0;str[i]!=0;i++)				/* Send each char of string till the null-terminator */
	{
		LCD_Char (str[i]);				/* Call LCD data write */
	}
}

void LCD_x_y (char row, char *str)/* Send string to LCD function */
{
	if (row == 1)
	LCD_Command(0x80);					/* Command for first row */
	
	else if (row == 2)		
	LCD_Command(0xC0);					/* Command for second row */

	LCD_String(str);					/* Call LCD string function */
}

void LCD_Init (void)			/* LCD Initialize function */
{
	delay(20);					
	LCD_Command (0x02);			
	LCD_Command (0x28);			/* Initialization of 16X2 LCD in 4bit mode */
	LCD_Command (0x0C);			/* Display ON Cursor OFF */
	LCD_Command (0x01);			/* clear display */
	LCD_Command (0x06);			/* Auto Increment cursor */
	LCD_Command (0x80);			/* cursor at home position */
}
//----------------------------------------------------------------------------------

void main()
{
	  int Air_Qual_Index ;
		unsigned char sti[17];	/* Array to store formatted string */
		LCD_Init();				/* initialize LCD */
		
		//delay(3000);			/* Wait 3sec for MQ 135 to get hot */

		while(1)
		{		

            Air_Qual_Index = Recive_Value(0);

            sprintf(sti,"		Air Quality 	 b");	/*Writing formatted output to sti*/
            LCD_x_y(1,sti);

			sprintf(sti,"		AQI	:%d PPM ",Air_Qual_Index);	/*Writing formatted output to sti*/
            LCD_x_y(2,sti);
                
            delay(1000);	/* Wait for 1sec */

		}	
}