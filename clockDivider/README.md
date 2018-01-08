# Clock Divider

## Overview

Basically, this is a clock counter.  This one relies on an external clock so we are running all the counters on an interrupt.  The clocking is done on either the rising or falling edge of the clock.  The width of the clock cycles can be either 50% or just one main clock cycle (trigger mode).

Inspired by the [4MS clock divider](http://www.4mspedals.com/rcd.php).  In fact, I used to their manual to figure out what the counter really should do (as far as upbeat/downbeat and trigger modes).  Their project does the rotation part as well.  At [95 bucks for the kit](http://www.4mspedals.com/rcd-kit.php), personally, I would just buy that if I was planning a eurorack setup.

I have only played with this up to the breadboard stage.

![Upbeat](https://github.com/robstave/miniProProjects/blob/master/clockDivider/images/proMiniDiv.png)



## Design considerations

The Arduino is going to have limitations when it comes to clock speed.
If you are just doing drum beats and sequences, then this is not a problem.
If you are hoping to crank it up to audio frequencies, then a little more upfront design is needed.

If your looking for something 10k and above, its probibly better to just build a simple circuit from CMOS chips.  In that case, your are not really clocking for sequences, but for sub octaves and the like.

This project is based on an external clock (one less thing to manage) which keys off an interrupt routine.  The top speed is really going to be based on the size of the interrupt routine.  Crank up th frequency...and when it craps out...thats your top freq.

If you were to build this to a working project, consider adding a clock to the circuit. Perhaps a CD40106 based clock or a simple CV controlled LFO.

## Specs

There are four basic modes for the counting.

* Upbeat Gate - Counting for each divisor starts on the up beat.  Duty Cycle 50%.
* Upbeat Trigger - Duty Cycle based on input clock.
* Downbeat Gate - Counting for each divisor starts on the down beat. Gate Duty Cycle 50%.
* Downbeat Trigger Duty Cycle based on input clock.

## Upbeat Gate

The first is to count on the upbeat with a 50% duty cycle (gate mode).  Note that there is some lead in for the
first part of each clock.

![Upbeat](https://github.com/robstave/miniProProjects/blob/master/clockDivider/images/upbeatGate1.0.png)
                    
## Upbeat Trigger

Similar, but the clock just outputs a trigger the size of the main clock

![Upbeat](https://github.com/robstave/miniProProjects/blob/master/clockDivider/images/upbeatTrigger1.0.png)

## Upbeat Gate

In this case, the counting starts on the downbeat. So All clocks fire on the first beat.

![Upbeat](https://github.com/robstave/miniProProjects/blob/master/clockDivider/images/downbeatGate1.0.png)

## Downbeat Trigger

![Upbeat](https://github.com/robstave/miniProProjects/blob/master/clockDivider/images/downbeatTrigger1.0.png)





## Similar projects

* [4MS clock divider](http://www.4mspedals.com/rcd.php) - Most similar
* [The Count] (https://www.moffenzeefmodular.com/store/count) - Arduino based clock divider with coarse and fine tuning as well as CV controls








