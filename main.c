
#include "main.h"


#define SIN_LEN 128
unsigned char sin_table[SIN_LEN];

unsigned char RXData,i,j,k,temp,cntr,time_out;
#define f_aclk 4000
#define pwm_freq 80
unsigned char duty_cycle = 10;
unsigned char period = ( (unsigned int)( f_aclk/pwm_freq));

interrupt(TIMER0_A0_VECTOR)  taccr0_isr(void)
{
	P1OUT ^= BIT0 ;

	if( duty_cycle-- == 0){
		duty_cycle=SIN_LEN;
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


	while (1)                                      // repeat forever
	{

	}                               
	return 0;
}

void print_string(char* s) {
	for (i=0; i!=32 && s[i] != 0; i++)  
	{
		SEND_CHAR(s[i]);
		if (i==15) SEND_CMD (DD_RAM_ADDR2);
	}  
}

void print_int(unsigned int i) {
	char buffer[10];
	ltoa(i, buffer, 10);
	print_string(buffer);
}

void show_status() {
	SEND_CMD(CLR_DISP);
	Delayx100us(50);
	print_int(duty_cycle);
	SEND_CMD(DD_RAM_ADDR2);
	print_int(period);
	SEND_CHAR(' ');
	print_int( pwm_freq);
	print_string("kHz");
}

// enables the 8MHz crystal on XT1 and use
// it as MCLK

void InitOsc(void)
{
  WDTCTL = WDTPW | WDTHOLD;                      // stop watchdog timer

	  BCSCTL1 |= XTS;                                // XT1 as high-frequency
	  _BIC_SR(OSCOFF);                               // turn on XT1 oscillator
							  
	  do                                             // wait in loop until crystal is stable 
		IFG1 &= ~OFIFG;
	  while (IFG1 & OFIFG);
	 
	  BCSCTL1 |= DIVA0;                              // ACLK = XT1 / 2 = 4MHz.
	  BCSCTL1 &= ~DIVA1;
	  
	  IE1 &= ~WDTIE;                                 // disable WDT int.
	  IFG1 &= ~WDTIFG;                               // clear WDT int. flag
	  
	  WDTCTL = WDTPW | WDTTMSEL | WDTCNTCL | WDTSSEL | WDTIS1; // use WDT as timer, flag each
															   // 512 pulses from ACLK
															   
	  while (!(IFG1 & WDTIFG));                      // count 1024 pulses from XT1 (until XT1's
													 // amplitude is OK)

	  IFG1 &= ~OFIFG;                                // clear osc. fault int. flag
	  BCSCTL2 |= SELM0 | SELM1;                      // set XT1 as MCLK
	  //IE1 |= WDTIE;
	}  

	void InitPorts(void)
	{
	  P1SEL = 0;                                     // 
	  P1OUT = 0;                                     //
	  P1DIR = BIT0 | BIT5 | BIT6;                           //enable only Relay outputs

	  P1DIR |= BIT7;
	  P1SEL |= BIT7;

	  TACCR0 = period;
	  TACCTL2 = OUTMOD_7 ; //+ CCIE;
	  TACCR2 = duty_cycle;
	  TACCTL0 = CCIE;
	  TACTL = TASSEL_1 + MC_1 ;/* Timer A mode control: ACLK,  Up to CCR0 */
	  

	  P2SEL = 0;
	  P2OUT = 0;
	  P2DIR = ~BIT0;                                //only P2.0 is input
	  
	  P4SEL = 0;
	  P4OUT = 0;
	  P4DIR = BIT2 | BIT3;                          //only buzzer pins are outputs
	  P4OUT |= BIT2;

	  P6SEL = 0x80;                                  
	  P6OUT = 0;
	  P6DIR = 0x00;                                  // all output
	  P6DIR |= BIT0;
	}

	void Delay (unsigned int a)
	{
	  for (k=0 ; k != a; ++k);                      //9+a*12 cycles
	}

	void Delayx100us(unsigned char b)
	{
	  for (j=0; j!=b; ++j) Delay (_100us);
	}


	void _E(void)
	{
			bitset(P2OUT,E);		//toggle E for LCD
		Delay(_10us);
		bitclr(P2OUT,E);
	}



	void SEND_CHAR (unsigned char d)
	{
		Delayx100us(5);                 //.5ms	
		temp = d & 0xf0;		//get upper nibble	
		LCD_Data &= 0x0f;
		LCD_Data |= temp;               
		bitset(P2OUT,RS);     	        //set LCD to data mode
		_E();                           //toggle E for LCD
		temp = d & 0x0f;
		temp = temp << 4;               //get down nibble
		LCD_Data &= 0x0f;
		LCD_Data |= temp;
		bitset(P2OUT,RS);   	        //set LCD to data mode
		_E();                           //toggle E for LCD
	}

	void SEND_CMD (unsigned char e)
	{
		Delayx100us(10);                //10ms
		temp = e & 0xf0;		//get upper nibble	
		LCD_Data &= 0x0f;
		LCD_Data |= temp;               //send CMD to LCD
		bitclr(P2OUT,RS);     	        //set LCD to CMD mode
		_E();                           //toggle E for LCD
		temp = e & 0x0f;
		temp = temp << 4;               //get down nibble
		LCD_Data &= 0x0f;
		LCD_Data |= temp;
		bitclr(P2OUT,RS);   	        //set LCD to CMD mode
		_E();                           //toggle E for LCD
	}

	void InitLCD(void)
	{
		bitclr(P2OUT,RS);
		Delayx100us(250);                   //Delay 100ms
		Delayx100us(250);
		Delayx100us(250);
		Delayx100us(250);
		LCD_Data |= BIT4 | BIT5;            //D7-D4 = 0011
		LCD_Data &= ~BIT6 & ~BIT7;
		_E();                               //toggle E for LCD
		Delayx100us(100);                   //10ms
		_E();                               //toggle E for LCD
		Delayx100us(100);                   //10ms
		_E();                               //toggle E for LCD
		Delayx100us(100);                   //10ms
		LCD_Data &= ~BIT4;
		_E();                               //toggle E for LCD
		
		SEND_CMD(DISP_ON);
		SEND_CMD(CLR_DISP);   
	}

void InitSin(void) {
	unsigned char i; float v; float pi=3.1415;
	float x=0;
	for(i=0;i<SIN_LEN;i++, x += 2*pi/SIN_LEN ) {
		v = sinf(x);
		sin_table[i] = (unsigned char)(period/2+(period/2)*v);
	}
}

