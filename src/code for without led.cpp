/*
 * rProject.c
 *
 * Created: 21-04-2023 13:34:54
 * Author : Admin
 */
#define F_CPU 1000000UL
#include <avr/io.h>
#include <avr/delay.h>
#include <stdlib.h>
#include <string.h>

void LCD_CMD (unsigned char command) {
	PORTD = 0X20; //0b00100000
	PORTC = command;
	_delay_ms(100);
	PORTD = 0X00; //0b00000000
}

void LCD_DATA1 (unsigned char DATA1) {
	PORTD = 0XA0; //0b10100000;
	PORTC = DATA1;
	_delay_ms(100);
	PORTD = 0X80; //0b10000000;
}

void LCD_Init() {
	LCD_CMD(0X38);
	LCD_CMD(0X0E);
	LCD_CMD(0X06);
	LCD_CMD(0X01);
	LCD_CMD(0X80);
}

void ADC_Init()
{
	DDRA=0x0;		/*  Make ADC port as input  */
	ADCSRA = 0x87;		/*  Enable ADC, fr/128  */
}

int ADC_Read()
{
	ADMUX = 0x40;		/* Vref: Avcc, ADC channel: 0  */
	ADCSRA |= (1<<ADSC);	/* start conversion  */
	while ((ADCSRA &(1<<ADIF))==0);	/* monitor end of conversion interrupt flag */
	ADCSRA |=(1<<ADIF);	/* set the ADIF bit of ADCSRA register */
	return(ADCW);		/* return the ADCW */
}

void LCD_MSG(unsigned char row, unsigned char col, char *msg) {
	if (row == 1) {
		row = 0X80;
	} else {
		row = 0XC0;
	}
	LCD_CMD(row|col);
	while(*msg) {
		LCD_DATA1(*msg);
		msg++;
	}
}

void LED (float moist) {
	if(moist > 60) {
		PORTB = 0b10000000;
	} else if (moist <=60 && moist > 40) {
		PORTB = 0b11000000;
	} else {
		PORTB = 0b01000000;
	}
	
}

int main(void)
{
	DDRB = 0XFF;
    DDRD = 0XFF;
	DDRC = 0XFF;
	LCD_Init();
	ADC_Init();		/* initialize the ADC */
	char array[10];
	int adc_value;
	float moisture;
	while(1) {
		adc_value = ADC_Read();	/* Copy the ADC value */
		moisture = 100-(adc_value*100.00)/1023.00;
		LED(moisture);
		LCD_MSG(1, 1, "Moisture: ");
		dtostrf(moisture,3,2,array);
		strcat(array,"%   ");
		LCD_MSG(0, 1, array);
		memset(array,0,10);
		_delay_ms(100);
	}
}