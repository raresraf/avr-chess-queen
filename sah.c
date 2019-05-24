/*
 * PM - 2017
 * lab0.c
 */

#include <avr/io.h>
#include <util/delay.h>
#include "usart.h"


#define F_CPU 16000000

/* Aprinde led-ul PC0 la apasarea butonului PB2.

void exemplu(void) {
	DDRB &= ~(255 << PB0);

	PORTB |= (255 << PB0);

	DDRA |= (255 << PA0);

	PORTA &= ~(255 << PA0);

	while (1) {
		btn_0_msk = 1;
		while(btn_0_msk != 0){
			if ((PINB & btn_0_msk) == 0)
				btn_0 = btn_0 | btn_0_msk;
			btn_0_msk = btn_0_msk << 1;
		}

		PORTA = btn_0;
	}
} */

uint8_t decode_digit(uint8_t var){
	switch(var){
		case 1: return 1;
		case 2: return 2;
		case 4: return 3;
		case 8: return 4;
		case 16: return 5;
		case 32: return 6;
		case 64: return 7;
		case 128: return 8;
	}
	return 0;
}
char decode_alpha(uint8_t var){
	switch(var){
		case 1: return 'a';
		case 2: return 'b';
		case 4: return 'c';
		case 8: return 'd';
		case 16: return 'e';
		case 32: return 'f';
		case 64: return 'g';
		case 128: return 'h';
	}	
	return 0;
}

uint8_t code_digit(char var){
	switch(var){
		case '1': return 1;
		case '2': return 2;
		case '3': return 4;
		case '4': return 8;
		case '5': return 16;
		case '6': return 32;
		case '7': return 64;
		case '8': return 128;
	}
	return 0;
}
uint8_t code_alpha(char var){
	switch(var){
		case 'a': return 1;
		case 'b': return 2;
		case 'c': return 4;
		case 'd': return 8;
		case 'e': return 16;
		case 'f': return 32;
		case 'g': return 64;
		case 'h': return 128;
	}	
	return 0;
}

void chess_play(void) {
	//butoane
	DDRA &= ~(255 << PA0);
	PORTA |= (255 << PA0);

	DDRC |= (3 << PC0);
	PORTC &= ~(3 << PC0);
	DDRC |= (3 << PC6);
	PORTC &= ~(3 << PC6);

	DDRD |= (3 << PD4);
	PORTD &= ~(3 << PD4);

	DDRD |= (3 << PD6);
	PORTD &= ~(3 << PD6);	

	DDRD |= (1 << PD7);
	PORTD |= (1 << PD7);

	DDRB |= (255 << PB0);
	PORTB &= ~(255 << PB0);

	uint8_t parity = 0;
	uint8_t btn_alpha = 0;
	uint8_t btn_digit = 0;
	uint8_t contor = 0;
	uint8_t phase = 0;
	char send_buffer[1024];

	uint8_t btn_alpha_phase2 = 0;
	uint8_t btn_digit_phase2 = 0;
	char c1;
	char c2;
	char c3;
	char c4;


	uint8_t await;
	while (1) {
		if(parity == 0){
			contor = 0;
			while(contor < 8){
				if ((PINA & (1 << contor)) == 0 && phase < 4) {
					if(phase % 2 == 0){
						btn_alpha = 0;
						btn_digit = 0;
						btn_alpha |= (1 << contor);
						phase++;
					}
					else {
						btn_digit |= (1 << contor);
						phase++;
					}
					_delay_ms(1000);
				}
				contor++;
			}
			PORTB = btn_alpha;
			PORTC = ((btn_digit & 3) << PC0) | (((btn_digit & 12) >> 2) << PC6);
			PORTD = (((btn_digit & 240) >> 4) << PD4);
			
			if(phase == 2){
				btn_alpha_phase2 = btn_alpha;
				btn_digit_phase2 = btn_digit;
			}
			if(phase == 4){
				sprintf(send_buffer, " %c%d%c%d ", decode_alpha(btn_alpha_phase2), decode_digit(btn_digit_phase2), decode_alpha(btn_alpha), decode_digit(btn_digit));
				USART0_print(send_buffer);

				_delay_ms(1000);
				btn_digit = 255;
				btn_alpha = 255;
				PORTB = btn_alpha;
				PORTC = ((btn_digit & 3) << PC0) | (((btn_digit & 12) >> 2) << PC6);
				PORTD = (((btn_digit & 240) >> 4) << PD4);
				c1 = USART0_receive();
				UDR0 = c1;
				//USART0_transmit(c1);
				_delay_ms(50);
				c2 = USART0_receive();
	            UDR0 = c2;
				_delay_ms(50);
				c3 = USART0_receive();
	            UDR0 = c3;
				_delay_ms(50);
				c4 = USART0_receive();
	            UDR0 = c4;
				_delay_ms(50);
				
				_delay_ms(1000);
				//sprintf(send_buffer, "%c%c%c%c", c1,c2,c3,c4);
				//USART0_print(send_buffer);

				phase = 0;
				parity = 1;
			}
		}
		else {
			if(phase == 0 || phase == 1){
				btn_alpha = code_alpha(c1);
				btn_digit = code_digit(c2);
				PORTB = btn_alpha;
				PORTC = ((btn_digit & 3) << PC0) | (((btn_digit & 12) >> 2) << PC6);
				PORTD = (((btn_digit & 240) >> 4) << PD4);
			}
			else{
				btn_alpha = code_alpha(c3);
				btn_digit = code_digit(c4);
				PORTB = btn_alpha;
				PORTC = ((btn_digit & 3) << PC0) | (((btn_digit & 12) >> 2) << PC6);
				PORTD = (((btn_digit & 240) >> 4) << PD4);				
			}
			switch (phase){
				case 0: await = btn_alpha; break;
				case 1: await = btn_digit; break;
				case 2: await = btn_alpha; break;
				case 3: await = btn_digit; break;
				default: await = 0;
			}
			contor = 0;
			while(contor < 8){

				//sprintf(send_buffer, " Await:%d %d ", await, (1 << contor));
				//USART0_print(send_buffer);
				if ((PINA & (1 << contor)) == 0 && phase < 4 && (1 << contor) == await)  {
					
					phase++;
					_delay_ms(1000);
				}
				contor++;
			}

			if(phase == 4){
				phase = 0;
				parity = 0;	
				btn_alpha = 0;
				btn_digit = 0;

			}
		}

	}
}

int main(void) {
	USART0_init();
	chess_play();
	while (1);

	return 0;
}