# Intruder System Detection 
The purpose of this program is to implement a 3 state intruder system. 
 
## Prerequisites 
One must have an MSP430FR2533 available for use. 
 
## Modules 
* Code Composer Studio - 12.2.0  
 
## Function of the Program 
 
## Pre-main.c code 
The function of main.c includes the header file msp430.h 
The function begins by defining the 3 states of the alarm system. 0 for `ARMED_STATE`, 1 for `WARNING_STATE`, and 2 for `ALERT_STATE`. 
The character `state` is declared and assigned `ARMED_STATE`. This character will be used later to define what the MSP430 does.  
 
## main.c 
First `WDTCTL = WDTPW | WDTHOLD` is used to turn off the watchdog timer. If this is not done, the processor will reset, causing the alarm system to constantly reset to the armed state. 
The button P4.1 is enabled as an input with a pull-up resistor.  
The button P2.3 is enabled as an input with a pull-up resistor. 
Both ports P6.6 and P1.0 are initialized as outputs. P6.6 is the green LED and P1.0 is the red LED. 
`PM5CTL0 &= ~LOCKLPM5` is used to enable low power mode.  
The character `cnt` is declared and assigned a value of 0. This character will be used to time the state transisitions.  
 
## while(1) 
Contained in the while loop is a switch-case statement with 3 different cases for each state of the alarm. Every time the while loop is repeated, the state of the alarm is checked. Each state has a set of instructions that it will execute without executing code from the other states. This is done by adding a `break;` line at the end of each case.  
 
### case ARMED_STATE 
For `case ARMED_STATE:` the case begins by setting `cnt` to 0. This is so that if an intruder leaves before the alarm is alert, the cnt resets. It is also for when the alarm is alert, if the alarm is reset, `cnt` is assigned 0.  
Next is an if statement that checks if the button P2.3 is being pressed. The button is active low as it has a pull-up resistor. If the button is being pressed, `state` will be assigned `WARNING_STATE` and the case will break, transitioning to the next case, `WARNING_STATE`, with the next iteration of the while loop. 
If P2.3 is not being pressed, then the green LED, P6.6, is toggled. 
The red LED, P1.0, is turned off so that if the MSP430 is in `WARNING_STATE` or `ALERT_STATE` and a state transition occurs when the red LED is on, the LED will turn off in `ARMED_STATE`. Glitch prevention. 
`__delay_cycles(1500000)` delays the while loop by 1500000 microseconds or 1.5 seconds so that the green LED turns off and on every 1.5 seconds, resulting in the LED blinking every 3 seconds.  
 
### case WARNING_STATE 
For `case WARNING_STATE:` the case begins by interating `cnt` by 1.  
`cnt` is then used in an if statement where is cnt is greater than or equal to 20, then `state` is assigned `ALERT_STATE`, causing a state transition. The next code is not executed using a `break;` statement contained inside the if statement. 
Another if statement is used to check if the button P2.3 has been let go, or has returned to 1. If this happens, then `state` is assigned `ARMED_STATE`. This is done to represent the intruder leaving before the alarm has time to transition to the alert state. Next code is not executed using a `break;` statement. 
The red LED is toggled to represent that an intruder has been detected but that they still have time to leave. 
The green LED is turned off as to prevent the glitch of staying on if `state` transistioned from armed to warning while the green LED was on.  
`__delay_cycles(500000)` delays the while loop by 500000 microseconds or 0.5 seconds. This is done firstly because then `cnt` will be iterated once every 0.5 seconds. As the state only transitions when `cnt >= 20`, the intruder is given 10 seconds to leave. Secondly, the red LED is blinked every second, off for 0.5 seconds and on for the same amount of time.  
 
### case ALERT_STATE 
For `case ALERT_STATE:` the case begins by interating `cnt` by 1.  
Using an if statement, the alarm will still check if the intruder has left. This is done by checking if P2.3 has been let go and if `cnt < 30`. If both are true, then `state` is assigned `ARMED_STATE` reseting the alarm. The next code is not exeucted using a `break;` statement.   
If `cnt` is equal to or greater than 30, then the alarm will be stuck in the alert state and will need to be manually reset to return to the armed state.  
The red LED, P1.0, is kept solid to show that the alarm has reached the alert state. This mean that the intruder either has little time to leave or has run out of time all together.  
The green LED, P6.6, is turned off. 
`__delay_cycles(500000)` delays the while loop by 500000 microseconds or 0.5 seconds. This is done to iterate `cnt` at the same rate as what is performed in `WARNING_STATE`. `cnt` will be 20 upon entering the loop, so this leaves a 5 second window for the intruder to leave upon the alarm entering the alert state. 
 
# Improvements 
First, this is a test design for an alarm system. To use an IR sensor as a traditional alarm system would use, change the input port and pin from P2.3 to the port chosen for the IR sensor to be connected.  
Second, the technique used for detecting state transisitons is software polling. It would be much better to switch enable interrupts. That would allow for intantaneous state transisitons as the button must be held to cause a transition. Using interrupts would also allow the MSP430 to be more power efficient. 
