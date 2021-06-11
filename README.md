# `calibrate_debounce`

A utility to calibrate the debounce delay for a mechanical switch (e.g., a
tactile/button switch).

## Circuit

This program expects an Arduino-compatible board with internal pull-up
resistors (i.e., digital pin mode `INPUT_PULLUP` supported), with the following
connections:

    Digital pin `kPinSwitch` ---- switch ---- ground

## How to build

### Option 1: Use the Arduino IDE

I've never done this before, so you're on your own :-)

### Option 2: Use the Makefile

If you are using an Arduino Uno–compatible board, simply run

    make

to compile the program, and then

    make upload

to flash your controller.

The heavy lifting is done by
[Arduino-Makefile](https://github.com/sudar/Arduino-Makefile), so
if you are using a non-Uno board, you will need to supply the build arguments
`BOARD_TAG` and `BOARD_SUB` as described in the Arduino-Makefile documentation.
For example:

    make BOARD_TAG=atmegang BOARD_SUB=atmega168
    make upload

Note that Arduino-Makefile depends on (the Arduino core libraries and)
pySerial.

## How to use

Slowly toggle the switch, and inspect the resulting serial printout.  I'd
recommend waiting at least 2 seconds between toggles.  (Printouts are held
in memory for 1 second before printing, so if the toggle time is less than
that, printouts will be lost.)

We define a "streak" to be a span of at least `kSteadyThresholdUsec`
microseconds during which the digital input has consistently read the same
value.  Shortly after the end of each streak, a message like the following
is printed to serial:

    Streak with value 0 started at timestamp 19496560 usec and ended at time 21879656 usec
    Time between streaks = 0 usec (+/- approx 8 usec)

More example outputs:

    Streak with value 0 started at timestamp 19496560 usec and ended at time 21879656 usec
    Time between streaks = 0 usec (+/- approx 8 usec)
    Streak with value 1 started at timestamp 21880396 usec and ended at time 24318016 usec
    Time between streaks = 740 usec (+/- approx 12 usec)
    Streak with value 0 started at timestamp 24318016 usec and ended at time 26807016 usec
    Time between streaks = 0 usec (+/- approx 12 usec)

The error term (`+/- approx __ usec`) is the amount of time between
consecutive reads (i.e., consecutive executions of `loop()`).
