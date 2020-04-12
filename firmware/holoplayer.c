#include <avr/io.h>
//#include <avr/interrupt.h>

#ifndef F_CPU
#define F_CPU 8000000UL
#endif


#define LED_PIN PB3
#define DEBUG_PIN PB0
#define SENSOR_PIN PB4
#define ON_DUTY_CYCLES 4 
#define IR_DEBOUNCE_COUNT 10
#define IR_MAX_COUNT 65534 
#define MAX_COUNT 65534 
#define FRAMES_PER_EVENT 6
//NORMALLY NOT NEEDED
#define FRAMES_PER_EVENT_FIX 2
unsigned char ir_on=0;
unsigned int pulse_front=3000;
unsigned int counter=0;
unsigned int debounce_counter=0;
unsigned int ir_pulse_counter=0;
void init(void)
{
        DDRB |= (1<<LED_PIN)|(1<<DEBUG_PIN); // set LED pin as OUTPUT
        DDRB &=~(1<<SENSOR_PIN) ;
        PORTB |=(1<<SENSOR_PIN) ; //Pull-up
        PORTB &=~(1<<LED_PIN) ;
}

void turn_lights(char on)
{
	if(on)
		PORTB |= (1<<LED_PIN);
	else
		PORTB &= ~(1<<LED_PIN);
}

void on_ir_fall(unsigned int t)
{
	pulse_front=(t / FRAMES_PER_EVENT) + FRAMES_PER_EVENT_FIX;
}

void switch_ir(char s)
{
	if((ir_on != s) )
	{
		debounce_counter++;
		if(debounce_counter>IR_DEBOUNCE_COUNT)
		{
			debounce_counter=0;
			ir_on=s;
			if(!ir_on)
			{
				on_ir_fall(ir_pulse_counter);
				ir_pulse_counter=0;
			}
		}
	}
}

int main(void)
{
	init();
        while (1)
	{
		counter++;
		ir_pulse_counter++;
		//time_since_last_ir_cnt++;
		switch_ir(PINB & _BV(SENSOR_PIN));
		if(counter % (pulse_front-ON_DUTY_CYCLES) == 0)
			turn_lights(1);
		if(counter % pulse_front == 0)
		{
			counter=0;
			turn_lights(0);
		}
		if(ir_on)
			PORTB |= (1<<DEBUG_PIN);
		else
			PORTB &= ~(1<<DEBUG_PIN);

		
		/*
		PORTB |= (1<<DEBUG_PIN);
		delay_ms(250);
		PORTB &= ~(1<<DEBUG_PIN);
		delay_ms(250);
		*/
		if(counter >= MAX_COUNT)
			counter=0;
	};
}
