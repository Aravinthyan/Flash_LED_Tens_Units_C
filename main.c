#include <msp430g2553.h>

// value 81

char count = 0;
char light_state = 0;

void main(void)
{
	WDTCTL = WDTPW | WDTHOLD;	// stop watchdog timer

	BCSCTL1 = CALBC1_1MHZ;  // set range
	DCOCTL = CALDCO_1MHZ;   // set step

	P1REN = BIT3;   // enabling pull up/down resistor for P1.3
	P1DIR = BIT0 + BIT6;   // make P1.0 output
	P1OUT = BIT3;   // pull up resistor is set

	P1IES |= BIT3;   // edge select high->low
	P1IFG &= ~BIT3; // interrupt flag cleared
	P1IE |= BIT3;   // enable interrupt for P1.3

	TA0CCR0 = 62499;    // count to 62499 which is 0.5s
	// set up timer
	// SMCLk
	// /8
	// up mode
	// clear Timer A
	TACTL = TASSEL_2 + ID_3 + MC_1 + TACLR;

	_enable_interrupt();

}

#pragma vector = PORT1_VECTOR
__interrupt void Port_1_interrupt_service_routine(void)
{
    if(P1IFG & BIT3)    // if interrupt is from P1.3
    {
        P1IE &= ~BIT3; // disable any further interrupt from P1.3
        P1IFG &= ~(BIT3);   // clear flag

        count = 0;
        light_state = 0;

        TA0CCTL0 |= CCIE;    // enable interrupt for Timer A0 CCR0
    }
}

#pragma vector = TIMER0_A0_VECTOR
__interrupt void Timer_A0_CCR0_interrupt_service_routine(void)
{
    if(count < 8)  // tens
    {
        if(light_state == 1)    // LED is switched on all this time
        {
            P1OUT &= ~BIT0; // make P1.0 low
            light_state = 0;
            count++;
        }
        else if(light_state == 0)   // LED switched off all this time
        {
            P1OUT |= BIT0;  // make P1.0 high
            light_state = 1;
        }
    }
    else if(count == 8)
    {
        P1OUT &= ~BIT0;
        light_state = 0;
        count++;
    }
    else if(count > 8 && count <= 9)
    {
        if(light_state == 1)    // LED is switched on all this time
        {
            P1OUT &= ~BIT6; // make P1.6 low
            light_state = 0;
            count++;
        }
        else if(light_state == 0)   // LED switched off all this time
        {
            P1OUT |= BIT6;  // make P1.6 high
            light_state = 1;
        }
    }
    else
    {
        count = 0;
        P1OUT &= ~BIT6;
        TA0CCTL0 &= ~CCIE;
        P1IE |= BIT3;
    }
}
