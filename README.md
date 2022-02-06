# one-touch-windows
Originally developed for the mark one MGF this file allows driver and passenger windows to be operated either as normal or one touch with just a short tap on the switch. 
There is an optional time out function providing power to the system for three minutes after the ignition is switched off and another which
uses the deadlocking function to operate the rollup of both windows.

Referring to the MGF wiring diagram,
both window switches must have their positive line fuses removed. This turns them into single pole centre off switches shorting to 0V on operation. 
Outputs on pin 6,7,8 and 9 operate a generic 4 way relay board. The relay feed is taken from the permanently live +12 Volt supply through a 10A self resetting fuse to each pair of relays. On operation, the appropriate relay switches from 0V to 12V.

Code is deliberately kept simple so it may be hacked to suit other cars. It is bulky but comprehensible and is capable of being written in a much smaller space but it runs well on a Uno.

The four counters, one for each switch control the one touch tap time and the roll up and down time.  The cycle time is based on a roughly 10 millisecond delay with each switch being processed at 40 millisecond intervals. If the delays are shortened, it won't work better and switch bounce may be an issue.
lines 180 and 186 and their counterparts in the other three switch processing routines may be adjusted to shorten or lengthen one touch and travel times.


The status register is used to hold the direction and one touch flags
so if a motor is in one touch mode and the reversing direction is touched, the motor will stop until the next touch.


The loop routines offtest, timeouttest and rolluptest are used with external circuitry to allow a timeout when the ignition is switched off and during this period, operating the dead locking function will cause both windows to be powered up for about six seconds. 
The main reason for its inclusion is to allow for a forgotten window to be closed  with the key out and to allow it to be opened slightly if the hood seals cause the door to stick - a typical MGF characteristic!

Pin 10 powers a transistor which operates relay that holds the power on from the permanently energised feed when the ignition is switched off, sensed through a potential divider of 2x3.3K Ohm resistors on pin 11 with a 4.7Volt Zener diode from pin 11 to ground. Pin 12 is the input from the indicators which counts two flashes  which confirms the deadlocking from the remote fob. 

As this may be operated with power removed from the circuit (ie. after the timeout period), pin 12 is connected to the output (pin 5 )of a 4n26 opto isolator, the LED being driven through a 330 Ohm resistor.
This protects the module input by preventing the input from being powered when the rest of the circuit isn't.

These functions  and the associated electronics may be  deleted by commenting out 423 to 425.

We are all grown ups. If you hurt yourself, anybody else or your vehicle it is your problem, not mine although if you use it to slay a politician I will gladly buy you a beer. This code is offered without any warranty or assertion of suitability for any purpose whatsoever. Have fun!
