
/**
   ProMini Div by N
   Divide the clock signal by 1,2,3,4...

   We will put an interrupt procedure on the int0 pin to count clock
   cycles.
   The  approach here can obviously be improved if you need speed.
   For example, rather than writing to each pin, figure out your
   values and write to say PortB/PortD all at once.

         +-----------------------+
         | 1 PD1             raw |
         | 0 PD0             gnd |
         | rst          PC5  rst |
         | gnd               vcc |
  clk    | 2  PD2     A3 PC3  17 |
  reset  | 3* PD3     A2 PC2  16 |
  div 1  | 4  PD4     A1 PC1  15 | useUpBeat
  div 2  | 5* PD5     A0 PC0  14 | useTrigger
  div 3  | 6* PD6        PB5  13 |
  div 4  | 7  PD7        PB4  12 |
  div 5  | 8  PB0        PB3 *11 | div 8
  div 6  | 9* PB1        PB2 *10 | div 7
         +-----------------------+

   V 1.0.0  -  First Version
       Simple counts
       Reset
       Up and down beats
       Gate/Trigger mode


   Note: This sketch has been written specifically for pro mini.
   It will probibly work just fine for uno and such, but
   always check pinouts

   Rob Stave (Rob the fiddler) 2018 copyleft
*/

const int int0 = 0;  // interrupt 0 - Clock interrupt
const int int1 = 1;  // interrupt 1 - Reset interrupt

#define BIT_OFF(port,pin) port &= ~(1<<(pin))
#define BIT_ON(port,pin) port |= (1<<(pin))
#define BIT_FLIP(port,pin) port ^= (_BV(pin))


const byte DIV1PIN = PD4;
const byte DIV2PIN = PD5;
const byte DIV3PIN = PD6;
const byte DIV4PIN = PD7;
const byte DIV5PIN = PB0;
const byte DIV6PIN = PB1;
const byte DIV7PIN = PB2;
const byte DIV8PIN = PB3;

// Flag to indicate gate/trigger mode.
volatile boolean useTrigger = false;
volatile boolean useUpBeat = true;


//Counters for divide by n
volatile  byte counts[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };

void incrementCounts () {
  counts[0]++;
  counts[1]++;
  counts[2]++;
  counts[3]++;
  counts[4]++;
  counts[5]++;
  counts[6]++;
  counts[7]++;
}

void setup()
{
  DDRB = B00111111; // set PORTB (digital 13~8)
  DDRD = B11110000; // set PORTD (digital 7~0)

  resetCounts ();

  //  clockCounter is our interrupt, function is called when
  //  invoked on either clock edge
  attachInterrupt(int0, clockCounter, CHANGE);

  // reset is an interrupt as well
  attachInterrupt(int1, resetCounts, RISING);
}

/**
   Clock Interrupt
   Increment counters and divide based on one of
   four strategies.
*/
void clockCounter()
{
  incrementCounts();

  if (useUpBeat == true) {
    if (useTrigger == false) {
      divUpBeat();
    } else {
      divUpBeatTrigger();
    }
  } else {
    if (useTrigger == false) {
      divDownBeat();
    } else {
      divDownBeatTrigger();
    }
  }
}


/**
   Clear all counts/ports
*/
void resetCounts () {

  memset_volatile(counts, 0, sizeof(counts));

  PORTB = B00000000;
  PORTD = B00000000;
}

/**
    Divide count by up beat.
    State machine is a little weird because of those lead in zeros

    1010101010101010101010
    0000111000111000111000  div3
    0000000011111000001111  div5

*/
void divUpBeat() {

  //Always flip the first bit
  BIT_FLIP(PORTD, DIV1PIN);

  if (counts[0] > 1) {
    //Use 2 here to track on/off
    counts[0] = 0;
  }

  //div by 2
  if (counts[1] >= 3) {
    counts[1] = 1;
    BIT_FLIP(PORTD, DIV2PIN);
  }

  //div by 3
  if (counts[2] >= 5) {
    counts[2] = 2;
    BIT_FLIP(PORTD, DIV3PIN);
  }

  //div by 4
  if (counts[3] >= 7) {
    counts[3] = 3;
    BIT_FLIP(PORTD, DIV4PIN);
  }

  //div by 5
  if (counts[4] >= 9) {
    counts[4] = 4;
    BIT_FLIP(PORTB, DIV5PIN);
  }

  //div by 6
  if (counts[5] >= 11) {
    counts[5] = 5;
    BIT_FLIP(PORTB, DIV6PIN);
  }

  //div by 7
  if (counts[6] >= 13) {
    counts[6] = 6;
    BIT_FLIP(PORTB, DIV7PIN);
  }

  //div by 8
  if (counts[7] >= 15) {
    counts[7] = 7;
    BIT_FLIP(PORTB, DIV8PIN);
  }
}

/**
   Similar to the up beat, but the length of the counter is just
   a trigger the size of the clock

   Example:
   101010101010101010
   000010000010000010  div by 3
*/
void divUpBeatTrigger() {

  PORTD ^= (_BV(DIV1PIN));
  if (counts[0] >= 2) {
    counts[0] = 0;
  }

  //div by 2
  if (counts[1] == 3) {
    BIT_ON(PORTD, DIV2PIN);
  } else {
    BIT_OFF(PORTD, DIV2PIN);
  }
  if ( counts[1] >= 4) {
    counts[1] = 0;
  }

  //div by 3
  if (counts[2] == 5) {
    BIT_ON(PORTD, DIV3PIN);
  } else {
    BIT_OFF(PORTD, DIV3PIN);
  }
  if ( counts[2] >= 6) {
    counts[2] = 0;
  }

  //div by 4
  if (counts[3] == 7) {
    BIT_ON(PORTD, DIV4PIN);
  } else {
    BIT_OFF(PORTD, DIV4PIN);
  }
  if ( counts[3] >= 8) {
    counts[3] = 0;
  }

  //div by 5
  if (counts[4] == 9) {
    BIT_ON(PORTB, DIV5PIN);
  } else {
    BIT_OFF(PORTB, DIV5PIN);
  }
  if ( counts[4] >= 10) {
    counts[4] = 0;
  }

  //div by 6
  if (counts[5] == 11) {
    BIT_ON(PORTB, DIV6PIN);
  } else {
    BIT_OFF(PORTB, DIV6PIN);
  }
  if ( counts[5] >= 12) {
    counts[5] = 0;
  }

  //div by 7
  if (counts[6] == 13) {
    BIT_ON(PORTB, DIV7PIN);
  } else {
    BIT_OFF(PORTB, DIV7PIN);
  }
  if ( counts[6] >= 13) {
    counts[6] = 0;
  }

  //div by 8
  if (counts[7] == 15) {
    BIT_ON(PORTB, DIV8PIN);
  } else {
    BIT_OFF(PORTB, DIV8PIN);
  }
  if ( counts[7] >= 18) {
    counts[7] = 0;
  }

}

/**
   Divide count by down beat

   Example
   101010101010
   110011001100
   111000111000
   111100001111
*/
void divDownBeat() {

  //Always flip the first bit
  BIT_FLIP(PORTD, DIV1PIN);

  if (counts[0] > 1) {
    counts[0] = 0;
  }

  // div by 2
  if (counts[1] < 3) {
    BIT_ON(PORTD, DIV2PIN);
  } else {
    BIT_OFF(PORTD, DIV2PIN);
    if (counts[1] >= 4) {
      counts[1] = 0;
    }
  }

  //div by 3
  if (counts[2] < 4) {
    BIT_ON(PORTD, DIV3PIN);
  } else {
    BIT_OFF(PORTD, DIV3PIN);
    if (counts[2] >= 6 ) {
      counts[2] = 0;
    }
  }

  //div by 4
  if (counts[3] < 5) {
    BIT_ON(PORTD, DIV4PIN);
  } else {
    BIT_OFF(PORTD, DIV4PIN);
    if (counts[3] >= 8 ) {
      counts[3] = 0;
    }
  }

  //div by 5
  if (counts[4] < 6) {
    BIT_ON(PORTB, DIV5PIN);
  } else {
    BIT_OFF(PORTB, DIV5PIN);
    if (counts[4] >= 10 ) {
      counts[4] = 0;
    }
  }

  //div by 6
  if (counts[5] < 7) {
    BIT_ON(PORTB, DIV6PIN);
  } else {
    BIT_OFF(PORTB, DIV6PIN);
    if (counts[5] >= 12 ) {
      counts[5] = 0;
    }
  }

  //div by 7
  if (counts[6] < 8) {
    BIT_ON(PORTB, DIV7PIN);
  } else {
    BIT_OFF(PORTB, DIV7PIN);
    if (counts[6] >= 14 ) {
      counts[6] = 0;
    }
  }

  //div by 8
  if (counts[7] < 9) {
    BIT_ON(PORTB, DIV8PIN);
  } else {
    BIT_OFF(PORTB, DIV8PIN);
    if (counts[5] >= 16 ) {
      counts[5]  = 0;

    }
  }
}

/**
   Similar to divDownBeat, but the size of the
   step is just one count

   101010101010
   100010001000
   100000100000
*/
void divDownBeatTrigger() {

  //Always flip the first bit
  BIT_FLIP(PORTD, DIV1PIN);


  if (counts[0] > 1) {
    //Use 2 here to track on/off
    counts[0] = 0;
  }

  // div by 2
  if (counts[1] < 2) {
    BIT_ON(PORTD, DIV2PIN);
  } else {
    BIT_OFF(PORTD, DIV2PIN);
    if (counts[1] >= 4) {
      counts[1] = 0;
    }
  }

  //div by 3
  if (counts[2] < 2) {
    BIT_ON(PORTD, DIV3PIN);
  } else {
    BIT_OFF(PORTD, DIV3PIN);
    if (counts[2] >= 6 ) {
      counts[2] = 0;
    }
  }

  //div by 4
  if (counts[3] < 2) {
    BIT_ON(PORTD, DIV4PIN);
  } else {
    BIT_OFF(PORTD, DIV4PIN);
    if (counts[3] >= 8 ) {
      counts[3] = 0;
    }
  }

  //div by 5
  if (counts[4] < 2) {
    BIT_ON(PORTB, DIV5PIN);
  } else {
    BIT_OFF(PORTB, DIV5PIN);
    if (counts[4] >= 10 ) {
      counts[4] = 0;
    }
  }

  //div by 6
  if (counts[5] < 2) {
    BIT_ON(PORTB, DIV6PIN);
  } else {
    BIT_OFF(PORTB, DIV6PIN);
    if (counts[5] >= 12 ) {
      counts[5] = 0;
    }
  }

  //div by 7
  if (counts[6] < 2) {
    BIT_ON(PORTB, DIV7PIN);
  } else {
    BIT_OFF(PORTB, DIV7PIN);
    if (counts[6] >= 14 ) {
      counts[6] = 0;
    }
  }

  //div by 8
  if (counts[7] < 2) {
    BIT_ON(PORTB, DIV8PIN);
  } else {
    BIT_OFF(PORTB, DIV8PIN);
    if (counts[5] >= 16 ) {
      counts[5] = 0;
    }
  }

}

void memset_volatile(volatile byte *s, byte c, size_t n)
{
  volatile byte *p = s;
  while (n-- > 0) {
    *p++ = c;
  }
}


void loop()
{

  byte inputPortC = PINC;
  useTrigger =  bitRead(inputPortC, 0);
  useUpBeat = bitRead(inputPortC, 1);
}
