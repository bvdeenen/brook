
#include "main.h"
#include "utils.h"

#define SIN_LEN 128
unsigned char sin_table[SIN_LEN];

unsigned char RXData, i, j, k, temp, cntr, time_out;
#define f_aclk 4000
#define pwm_freq 80
unsigned char duty_cycle = 10;
unsigned char period = ((unsigned int)(f_aclk / pwm_freq));

interrupt(TIMER0_A0_VECTOR) taccr0_isr(void)
{
	P1OUT ^= BIT0;

	if (duty_cycle-- == 0) {
		duty_cycle = SIN_LEN;
		P6OUT ^= BIT0;
	}

	TACCR2 = sin_table[duty_cycle];

}

int main(void)
{

	InitOsc();
	InitPorts();
	InitLCD();
	Delayx100us(10);
	InitSin();
	show_status();
	eint();

	while (1)		// repeat forever
	{

	}
	return 0;
}

void print_string(char *s)
{
	for (i = 0; i != 32 && s[i] != 0; i++) {
		SEND_CHAR(s[i]);
		if (i == 15)
			SEND_CMD(DD_RAM_ADDR2);
	}
}

void print_int(unsigned int i)
{
	char buffer[10];
	ltoa(i, buffer, 10);
	print_string(buffer);
}

void show_status()
{
	SEND_CMD(CLR_DISP);
	Delayx100us(50);
	print_int(duty_cycle);
	SEND_CMD(DD_RAM_ADDR2);
	print_int(period);
	SEND_CHAR(' ');
	print_int(pwm_freq);
	print_string("kHz");
}

// enables the 8MHz crystal on XT1 and use
// it as MCLK

void InitOsc(void)
{
	WDTCTL = WDTPW | WDTHOLD;	// stop watchdog timer

	BCSCTL1 |= XTS;		// XT1 as high-frequency
	_BIC_SR(OSCOFF);	// turn on XT1 oscillator

	do			// wait in loop until crystal is stable 
		IFG1 &= ~OFIFG;
	while (IFG1 & OFIFG);

	BCSCTL1 |= DIVA0;	// ACLK = XT1 / 2 = 4MHz.
	BCSCTL1 &= ~DIVA1;

	IE1 &= ~WDTIE;		// disable WDT int.
	IFG1 &= ~WDTIFG;	// clear WDT int. flag

	WDTCTL = WDTPW | WDTTMSEL | WDTCNTCL | WDTSSEL | WDTIS1;	// use WDT as timer, flag each
	// 512 pulses from ACLK

	while (!(IFG1 & WDTIFG)) ;	// count 1024 pulses from XT1 (until XT1's
	// amplitude is OK)

	IFG1 &= ~OFIFG;		// clear osc. fault int. flag
	BCSCTL2 |= SELM0 | SELM1;	// set XT1 as MCLK
	//IE1 |= WDTIE;
}

void InitPorts(void)
{
	P1SEL = 0;		// 
	P1OUT = 0;		//
	P1DIR = BIT0 | BIT5 | BIT6;	//enable only Relay outputs

	P1DIR |= BIT7;
	P1SEL |= BIT7;

	TACCR0 = period;
	TACCTL2 = OUTMOD_7;	//+ CCIE;
	TACCR2 = duty_cycle;
	TACCTL0 = CCIE;
	TACTL = TASSEL_1 + MC_1;	/* Timer A mode control: ACLK,  Up to CCR0 */

	P2SEL = 0;
	P2OUT = 0;
	P2DIR = ~BIT0;		//only P2.0 is input

	P4SEL = 0;
	P4OUT = 0;
	P4DIR = BIT2 | BIT3;	//only buzzer pins are outputs
	P4OUT |= BIT2;

	P6SEL = 0x80;
	P6OUT = 0;
	P6DIR = 0x00;		// all output
	P6DIR |= BIT0;
}

void InitSin(void)
{
	unsigned char i;
	float v;
	float pi = 3.1415;
	float x = 0;
	for (i = 0; i < SIN_LEN; i++, x += 2 * pi / SIN_LEN) {
		v = sinf(x);
		sin_table[i] = (unsigned char)(period / 2 + (period / 2) * v);
	}
}

// vim:tw=80:ts=4:noexpandtab
