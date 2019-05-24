/*
 * PM - 2017
 * lab0.c
 */

#include <avr/io.h>
#include <util/delay.h>

#define DELAY_SECVENTA		250
#define NUM_SECVENTE		6

#define MORSE_LUNG		150
#define MORSE_SCURT		50
#define MORSE_PAUZA		50

#define NUM_NOTE		8

int frecventa_nota[NUM_NOTE] = {
	262, 294, 330, 349, 392, 440, 494, 523
};

/* Aprinde led-ul PC0 la apasarea butonului PB2. */
void exemplu(void) {
	/* Setez pinul PB2 ca intrare. */
	DDRB &= ~(1 << PB2);

	/* Activez rezistenta de pull-up pentru pinul PB2. */
	PORTB |= (1 << PB2);

	/* Setez pinul PC0 ca iesire. */
	DDRC |= (1 << PC0);

	/* Sting LED-ul. */
	PORTC &= ~(1 << PC0);

	while (1) {
		/* Daca butonul este apasat. */
		if ((PINB & (1 << PB2)) == 0)
			/* Aprind LED-ul. */
			PORTC |= (1 << PC0);
		else
			/* Sting LED-ul. */
			PORTC &= ~(1 << PC0);
	}
}

/*
 * Aprinde LED-urile legate la portul C in functie de secventele alese.
 * Secventa va fi selectata la apasarea butonului PB2 sau PD6.
 */
void task1(void) {
	int secventa = 0;
	int apasat0 = 0;
	int apasat1 = 0;

	int i = 0;

	/* Setez pinii PB2 si PD6 ca pini de intrare. */
	DDRB &= ~(1 << PB2);
	DDRD &= ~(1 << PD6);

	/* Activez rezistenta de pull-up pentru pinii PB2 si PD6. */
	PORTB |= (1 << PB2);
	PORTD |= (1 << PD6);

	/* Setez toti pinii portului C ca pini de iesire. */
	DDRC = 0xFF;

	/* Sting toate LED-urile*/
	PORTC = 0x00;

	while(1) {
		/* Verific daca PB2 este apasat. */
		if ((PINB & (1 << PB2)) == 0) {
			if (!apasat0) {
				if (secventa < NUM_SECVENTE - 1) {
					++secventa;

					/*
					 * Sting LED-urile pentru a pregati
					 * noua secventa.
					 */
					PORTC = 0x00;
					i = 0;
				}

				apasat0 = 1;
			}
		} else {
			apasat0 = 0;
		}

		if ((PIND & (1 << PD6)) == 0) {
			if (!apasat1) {
				if (secventa > 0) {
					--secventa;

					/*
					 * Sting LED-urile pentru a pregati
					 * noua secventa.
					 */
					PORTC = 0x00;
					i = 0;
				}

				apasat1 = 1;
			}
		} else {
			apasat1 = 0;
		}

		/* Iterez prin secvente in functie de cea selectata. */
		switch (secventa) {
			/* Simplu 1*/
			case 0:
				i %= 8;

				PORTC = (1 << i);
				break;

			/* Simplu 2 */
			case 1:
				i %= 8;

				if (i == 0)
					PORTC = 0x00;

				PORTC |= (1 << i);
				break;

			/* Simplu 3 */
			case 2:
				i %= 8;

				PORTC = ((1 << i) | (1<< (7 - i)));
				break;

			/* Avansat 1 */
			case 3:
				i %= 15;

				if (i < 8)
					PORTC = (1 << i);
				else
					PORTC = (1 << (14 - i));
				break;

			/* Avansat 2 */
			case 4:
				i %= 16;

				if (i == 0)
					PORTC = 0x00;

				if (i < 8)
					PORTC |= (1 << i);
				else
					PORTC &= ~(1 << (i - 8));
				break;

			/* Avansat 3 */
			case 5:
				i %= 3;

				PORTC = (PORTC << 1);

				if(i == 0)
					PORTC |= 0x01;
				break;
		}

		i++;
		_delay_ms(DELAY_SECVENTA);
	}
}

/* Genereaza sunetul pentru tipul de ton folosit: lung (150ms) sau scurt (50ms). */
void speaker_morse(int tip_morse) {
	int i;

	/* Alternam starea pinului PD4 la fiecare milisecunda. */
	for (i = 0; i < tip_morse; ++i) {
		PIND = (1 << PD4);

		_delay_ms(1);
	}
}

/*
 * Genereaza tonuri pentru codul Morse folosind speaker-ul. Tonul generat este
 * selectat folosind butoanele PB2 si PD6.
 */
void task2(void) {
	/* Setez pinii PB2 si PD6 ca intrare. */
	DDRB &= ~(1 << PB2);
	DDRD &= ~(1 << PD6);

	/* Activez rezistenta de pull-up pentru pinii PB2 si PD6. */
	PORTB |= (1 << PB2);
	PORTD |= (1 << PD6);

	/* Setez pinul PD4 ca iesire. */
	DDRD |= (1 << PD4);

	/* Opresc speaker-ul. */
	PORTD &= ~(1 << PD4);

	while (1) {
		if ((PINB & (1 << PB2)) == 0)
			speaker_morse(MORSE_SCURT);
		else if ((PIND & (1 << PD6)) == 0)
			speaker_morse(MORSE_LUNG);

		PORTD &= ~(1 << PD4);
		_delay_ms(MORSE_PAUZA);
	}
}

/* Genereaza o nota folosind speaker-ul. */
void speaker_reda(int nota) {
	int perioada_nota = 1000000l / frecventa_nota[nota];
	int i;

	/*
	 * Nu putem folosi variable ca argument pentru functia
	 * _delay_us(), asa ca folosim acest "workaround".
	 */
	PORTD |= (1 << PD4);
	for (i = 0; i < perioada_nota / 2; ++i)
		_delay_us(1);

	PORTD &= ~(1 << PD4);
	for (i = 0; i < perioada_nota / 2; ++i)
		_delay_us(1);
}

/* Aprinde LED-ul corespunzator notei alese. */
void led_aprinde(int nota) {
	PORTC = (1 << nota);
}

/* Stinge LED-urile. */
void led_stinge(void) {
	PORTC = 0x00;
}

/*
 * Genereaza note folosind speaker-ul. Nota generata este aleasa folosind
 * butoanele PB2 si PD6.
 */
void task3(void) {
	int apasat0 = 0;
	int apasat1 = 0;

	int nota_curenta = -1;

	/* Setez pinii PB2 si PD6 ca pini de intrare. */
	DDRB &= ~(1 << PB2);
	DDRD &= ~(1 << PD6);

	/* Activez rezistenta de pull-up pentru pinii PB2 si PD6. */
	PORTB |= (1 << PB2);
	PORTD |= (1 << PD6);

	/* Setez toti pinii portului C ca pini de iesire. */
	DDRC = 0xFF;

	/* Sting toate LED-urile*/
	PORTC = 0x00;

	/* Setez pinul PD4 ca iesire. */
	DDRD |= (1 << PD4);

	/* Opresc speaker-ul. */
	PORTD &= ~(1 << PD4);

	while (1) {
		if ((PINB & (1 << PB2)) == 0) {
			if (!apasat0) {
				if (nota_curenta < NUM_NOTE - 1)
					++nota_curenta;

				apasat0 = 1;
				_delay_ms(10);
			}
		} else {
			apasat0 = 0;
		}

		if ((PIND & (1 << PD6)) == 0) {
			if (!apasat1) {
				if (nota_curenta > -1)
					--nota_curenta;

				apasat1 = 1;
				_delay_ms(10);
			}
		} else {
			apasat1 = 0;
		}

		if (nota_curenta != -1) {
			/* Aprindem LED-ul corespunzator. */
			led_aprinde(nota_curenta);

			/* Redam nota. */
			speaker_reda(nota_curenta);
		} else {
			/* Stingem LED-urile. */
			led_stinge();
		}
	}
}

int main(void) {
	exemplu();
	/*task1();*/
	/*task2();*/
	/*task3();*/

	while (1);

	return 0;
}