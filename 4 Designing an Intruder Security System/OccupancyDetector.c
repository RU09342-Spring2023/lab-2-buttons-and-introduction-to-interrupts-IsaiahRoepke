/*
 * OccupancyDetector.c
 *
 *  Created on: Feb 7, 2023
 *      Author: Isaiah Roepke
 */

#include <msp430.h>
#define ARMED_STATE 0  //define the different states the alarm will take.
#define WARNING_STATE 1
#define ALERT_STATE 2

// Put some initialization here

char state = ARMED_STATE; //The state of the alarm starts in the ARMED state.

int main(){

    WDTCTL = WDTPW | WDTHOLD; //Stops watchdog timer so processor doesn't reset.

P4OUT |= BIT1; //enables pullup resistor on P4.1
P4DIR &= ~BIT1; //enables P4.1 as an input
P4REN |= BIT1; //enables P4.1 resistor


P2OUT |= BIT3; //enables pullup resistor on P2.3
P2DIR &= ~BIT3; //enables P2.3 as an input
P2REN |= BIT3; //enables P2.3 resistor


P6OUT &= ~BIT6; //initializes the green LED as off.
P6DIR |= BIT6; //initialized the green LED as an output
P1OUT &= ~BIT0; //initializes the red LED as off
P1DIR |= BIT0; //Initialized the red LED as an output

PM5CTL0 &= ~LOCKLPM5; //enable low power mode



char cnt = 0; //the character cnt is used to control the timing between states



while(1)
{


  switch (state) { //checks which state the MC is in
    case ARMED_STATE:
    {
      cnt = 0; //cnt is set to 0 everytime ARMED_STATE is reached.


      if(!(P2IN & BIT3)){ //if the input P2.3 is 0, the MC will transition to the WARNING_STATE
          state = WARNING_STATE;
          break;
      }


              P1OUT &= ~BIT0; //Red LED is kept off
                P6OUT ^= BIT6;// Blink the Green LED
      __delay_cycles(1500000); //The Green LED is blinked once every 3s, 1.5s on, 1.5s off.
      break;
      // If something happens, you can move into the WARNING_STATE
      // state = WARNING_STATE
    }
    case WARNING_STATE:
    {
      cnt = cnt + 1; //cnt is iterated by 1 everycycle of WARNING_STATE
      if (cnt >= 20){ //When cnt reaches 20, the MC transitions to the ALERT_STATE.This will take 10s. Greater than or equal to is used as a precaution
          state = ALERT_STATE;
          break;
      }

      if (P2IN & BIT3){ //If the input P2.3 rises to a one before cnt == 20, then the MC will transition back to ARMED_STATE
          state = ARMED_STATE;
          break;
      }
          P6OUT &= ~BIT6; //Green LED is kept off.
                P1OUT ^= BIT0; //Red LED is blinked once a second, on 0.5s, off 0.5s

      __delay_cycles(500000);// Do something in the WARNING_STATE

      break;

    }
    case ALERT_STATE:
    {
        cnt = cnt + 1; //cnt is iterated by 1, starting at 20
        if ((cnt < 30) && (P2IN & BIT3)) //if cnt is still below 30 and the input rises to a 1, the MC transitions back to ARMED_STATE.
            state = ARMED_STATE;
        if (!(P4IN & BIT1)){ //After 15 seconds total has passed, cnt will be 30 or above. The reset the alarm, press the P4.1 button.
            state = ARMED_STATE;
            break;
        }


              }
        P6OUT &= ~BIT6; //Green LED is kept off.
              P1OUT |= BIT0; //Red LED is kept solid.
           __delay_cycles(500000);
           break;

    }
  }
}
/* If I were to improve this program, instead of using delay_cycles and if statements,
 * I would like to use interrupts. This would be more efficient, would allow for instantaneous
 * state transitions instead of having to hold the button, and would slim the code.
 */

