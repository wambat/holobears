#include <avr/io.h>
#include <avr/interrupt.h>

#ifndef F_CPU
#define F_CPU 8000000UL
#endif


#define LED_PIN PB3
#define DEBUG_PIN PB0
#define SENSOR_PIN PB4
#define ON_DUTY_RATIO 8 
#define IR_DEBOUNCE_COUNT 1000
#define IR_MAX_COUNT 65534 
#define BASE_CYCLE 180 //180 is 24 HZ
#define CYCLE_DIVIDER 10

unsigned char cycle=BASE_CYCLE;
unsigned char ir_on=0;
//unsigned long long time_since_last_ir=0;
unsigned long long time_since_last_ir_cnt=0;
/*
void delay_ms(uint8_t ms) {
  uint16_t delay_count = F_CPU / 17500;
  volatile uint16_t i;

  while (ms != 0) {
    for (i=0; i != delay_count; i++);
    ms--;
  }
}
*/
ISR(TIM0_COMPA_vect)
{
        if(PORTB & _BV(LED_PIN))
	   OCR0A=cycle-ON_DUTY_RATIO*cycle/100;
        else
	   OCR0A=ON_DUTY_RATIO*cycle/100;
        PORTB ^= _BV(LED_PIN); // toggle LED pin
}

void init_timer(void)
{
        TCCR0A |= _BV(WGM01); // set timer counter mode to CTC
        //TCCR0B |= _BV(CS02)|_BV(CS00);
        TCCR0B |= _BV(CS02); // set prescaler to 1024 (CLK=1200000Hz/1024/256=4.57Hz, 0.22s)
        OCR0A = cycle; // set Timer's counter max value
        TIMSK0 |= _BV(OCIE0A); // enable Timer CTC interrupt
        sei(); // enable global interrupts
}
void init_io(void)
{
        DDRB |= (1<<LED_PIN)|(1<<DEBUG_PIN); // set LED pin as OUTPUT
        DDRB &=~(1<<SENSOR_PIN) ;
        PORTB |=(1<<SENSOR_PIN) ; //Pull-up
        PORTB &=~(1<<LED_PIN) ;
}
void init(void)
{
	init_io();
	init_timer();
}

void on_ir_front(long long time_since_last_ir)
{
	cycle= (unsigned char) (time_since_last_ir / CYCLE_DIVIDER);
}

void switch_ir(char s)
{
	if((ir_on != s) )
		if(time_since_last_ir_cnt>IR_DEBOUNCE_COUNT)
		{
			ir_on=!ir_on;
			if(ir_on)
				on_ir_front(time_since_last_ir_cnt);
			time_since_last_ir_cnt=0;
		}
}

int main(void)
{
	init();
        while (1)
	{
		time_since_last_ir_cnt++;
		switch_ir(PINB & _BV(SENSOR_PIN));
		/*
		if(ir_on)
			PORTB |= (1<<DEBUG_PIN);
		else
			PORTB &= ~(1<<DEBUG_PIN);
		*/

		
		/*
		PORTB |= (1<<DEBUG_PIN);
		delay_ms(250);
		PORTB &= ~(1<<DEBUG_PIN);
		delay_ms(250);
		*/
	};
}
