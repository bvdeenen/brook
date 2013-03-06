
#include "main.h"
#include "utils.h"

#define SIN_LEN 128
#define F_ACLK 8000
#define F_PWM 80
#define F_SAMPLE 20
#define update_ct  ((uint8_t)(F_PWM/F_SAMPLE))

uint8_t sin_table[SIN_LEN];
uint8_t i;
uint8_t duty_cycle = SIN_LEN-1;
uint8_t update_index = update_ct;
const uint8_t k_period = ((unsigned char)(F_ACLK / F_PWM));


interrupt(TIMER0_A0_VECTOR) taccr0_isr(void)
{
	bittoggle(P1OUT, 0);
	TACCR2 = sin_table[duty_cycle];
	if ( --update_index == 0){
		update_index = update_ct;
		LPM0_EXIT;
	}
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
		LPM0;

		// called once every update_ct interrupts (F_PWM)
		bittoggle(P6OUT,1);
		duty_cycle -=8;
		if (duty_cycle & 128) {
			duty_cycle = SIN_LEN-1;
			bittoggle(P6OUT,0);
		}
	}
	return 0;
}

void show_status()
{
	SEND_CMD(CLR_DISP);
	Delayx100us(50);
	print_int(duty_cycle);
	SEND_CMD(DD_RAM_ADDR2);
	print_string("per:");
	print_int(k_period);
	SEND_CHAR(' ');
	print_int(F_PWM);
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

	BCSCTL1 &= ~DIVA0; // XT1 = 8 MHz.
	BCSCTL1 &= ~DIVA1;

	IE1 &= ~WDTIE;		// disable WDT int.
	IFG1 &= ~WDTIFG;	// clear WDT int. flag

	WDTCTL = WDTPW | WDTTMSEL | WDTCNTCL | WDTSSEL | WDTIS1;	// use WDT as timer, flag each
	// 512 pulses from ACLK

	while (!(IFG1 & WDTIFG)) ;	// count 1024 pulses from XT1 (until XT1's
	// amplitude is OK)

	IFG1 &= ~OFIFG;		// clear osc. fault int. flag
	BCSCTL2 |= SELM0 | SELM1;	// set XT1 as MCLK
}

void InitPorts(void)
{
	P1SEL = 0;		// 
	P1OUT = 0;		//
	P1DIR = BIT0 | BIT7;

	P1SEL |= BIT7; // P1.7 controlled by TACCR2 (DALLAS on easyWeb-II)

	TACCR0  = k_period;
	TACCTL2 = OUTMOD_7;
	TACCR2  = duty_cycle;
	TACCTL0 = CCIE;
	TACTL   = TASSEL_1 + MC_1;	/* Timer A mode control: ACLK,  Up to CCR0 */

	P2SEL = 0;
	P2OUT = 0;
	P2DIR = ~BIT0;		//only P2.0 is input

	P4SEL = 0;
	P4DIR = BIT2|BIT3; // buzzer
	P4OUT = 0;
	bittoggle(P4OUT, BIT2);

	P6SEL = 0;
	P6DIR = BIT0|BIT1;		// all output
}

void InitSin(void)
{
	unsigned char i;
	float v;
	float pi = 3.1415;
	float x = 0;
	for (i = 0; i < SIN_LEN; i++, x += 2 * pi / SIN_LEN) {
		v = sinf(x);
		sin_table[i] = (unsigned char)(k_period / 2 + (k_period / 2) * v);
	}
}

// vim:tw=80:ts=4:noexpandtab
