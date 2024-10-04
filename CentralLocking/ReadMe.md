# About this repository

This repo is created to solve one very specific problem: 
BMWs have a feature where the central locking mechanism differentiates between locking the 
doors with a key, or from the inside.

This creates a problem when installing aftermarket remote sets: 
they usually only lock the doors like you lock it from the inside, thus breaking a window 
you can open the vehicle with the door handle inside. Whereas if you lock the doors with a 
key, you can't open them any other way.

So the body control module needs a separate signal indicating the key is present in the lock. 
This signal has to be delayed a bit (about 200ms). This signal can also be used to roll the 
windows up (when you hold the key in the locking position) 

# Hardware

First we need a microcontroller. In this case I used a "digispark" which is an 
AtTiny85 powered development board.

The input we get is the 12V signal from the remote, and the signal we need to produce 
is also 12V. the input is stepped down by a voltage divider to get about 5V out of it.
The output is switched through an NPN and a PNP transistor with an LED indicator (mainly for 
debug purposes)

# Result

By default the microcontroller is in a sleep state. Whenever it receives a signal on the input 
it wakes up and starts a counter. On receiving the first signal, a 500ms pulse is sent to the 
output to ensure proper locking. If it receives another signal within 5 seconds, it will send 
out a 15s signal to roll up the windows
