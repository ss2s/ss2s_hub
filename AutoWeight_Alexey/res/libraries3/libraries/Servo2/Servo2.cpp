
#include <Arduino.h>
#include "Servo2.h"
static void initISR();
static void writeChan(uint8_t chan, int pulsewidth);

#define FRAME_SYNC_INDEX   0		 // frame sync delay is the first entry in the channel array
#define FRAME_SYNC_PERIOD  20000	   // total frame duration in microseconds
#define FRAME_SYNC_DELAY   ((FRAME_SYNC_PERIOD - ( NBR_CHANNELS * DEFAULT_PULSE_WIDTH))/ 128) // number of iterations of the ISR to get the desired frame rate
#define DELAY_ADJUST	 8		 // number of microseconds of calculation overhead to be subtracted from pulse timings

static servo_t servos[NBR_CHANNELS+1];    // static array holding servo data for all channels

static volatile uint8_t Channel;   // counter holding the channel being pulsed
static volatile uint8_t ISRCount;  // iteration counter used in the interrupt routines;
uint8_t ChannelCount = 0;	    // counter holding the number of attached channels
static boolean isStarted = false;  // flag to indicate if the ISR has been initialised

ISR (TIMER2_OVF_vect)
{
  ++ISRCount; // increment the overlflow counter
  if (ISRCount ==  servos[Channel].counter ) // are we on the final iteration for this channel
  {
	TCNT2 =  servos[Channel].remainder;   // yes, set count for overflow after remainder ticks
  }
  else if(ISRCount >  servos[Channel].counter)
  {
	// we have finished timing the channel so pulse it low and move on
	if(servos[Channel].Pin.isActive == true)	     // check if activated
	    digitalWrite( servos[Channel].Pin.nbr,LOW); // pulse this channel low if active

	  Channel++;    // increment to the next channel
	ISRCount = 0; // reset the isr iteration counter
	TCNT2 = 0;    // reset the clock counter register
	if( (Channel != FRAME_SYNC_INDEX) && (Channel <= NBR_CHANNELS) ){	     // check if we need to pulse this channel
	    if(servos[Channel].Pin.isActive == true)	   // check if activated
		 digitalWrite( servos[Channel].Pin.nbr,HIGH); // its an active channel so pulse it high
	}
	else if(Channel > NBR_CHANNELS){
	   Channel = 0; // all done so start over
	}
   }
}

Servo2::Servo2()
{
   if( ChannelCount < NBR_CHANNELS)
	this->chanIndex = ++ChannelCount;  // assign a channel number to this instance
   else
	this->chanIndex = 0;  // todo	// too many channels, assigning 0 inhibits this instance from functioning
}

uint8_t Servo2::attach(int pin)
{
	if( isStarted == false)
	 initISR();
	if(this->chanIndex > 0)
	{
	 //debug("attaching chan = ", chanIndex);
	 pinMode( pin, OUTPUT) ;  // set servo pin to output
	 servos[this->chanIndex].Pin.nbr = pin;
	 servos[this->chanIndex].Pin.isActive = true;
	}
	return this->chanIndex ;
}

void Servo2::detach()
{
    servos[this->chanIndex].Pin.isActive = false;
}

void Servo2::write(int value)
{
  if(value < MIN_PULSE_WIDTH)
  {  // treat values less than 544 as angles in degrees (valid values in microseconds are handled as microseconds)
    if(value < 0) value = 0;
    if(value > 180) value = 180;
    value = map(value, 0, 180, MIN_PULSE_WIDTH,  MAX_PULSE_WIDTH);
  }
  this->writeMicroseconds(value);
}

void Servo2::writeMicroseconds(int pulsewidth)
{
   writeChan(this->chanIndex, pulsewidth); // call the static function to store the data for this servo
}

int Servo2::read() // return the value as degrees
{
  return  map( this->readMicroseconds()+1, MIN_PULSE_WIDTH, MAX_PULSE_WIDTH, 0, 180);
}

int Servo2::readMicroseconds()
{
  unsigned int pulsewidth;
   if( this->chanIndex > 0)
	pulsewidth =  servos[this->chanIndex].counter * 128 + ((255 - servos[this->chanIndex].remainder) / 2) + DELAY_ADJUST ;
   else
	 pulsewidth  = 0;
   return pulsewidth;
}

bool Servo2::attached()
{
    return servos[this->chanIndex].Pin.isActive ;
}

static void writeChan(uint8_t chan, int pulsewidth)
{
   // calculate and store the values for the given channel
   if( (chan > 0) && (chan <= NBR_CHANNELS) )   // ensure channel is valid
   {
	if( pulsewidth < MIN_PULSE_WIDTH )		    // ensure pulse width is valid
	    pulsewidth = MIN_PULSE_WIDTH;
	else if( pulsewidth > MAX_PULSE_WIDTH )
	    pulsewidth = MAX_PULSE_WIDTH;

	  pulsewidth -=DELAY_ADJUST;			 // subtract the time it takes to process the start and end pulses (mostly from digitalWrite)
	servos[chan].counter = pulsewidth / 128;
	servos[chan].remainder = 255 - (2 * (pulsewidth - ( servos[chan].counter * 128)));  // the number of 0.5us ticks for timer overflow
   }
}

static void initISR()
{
	for(uint8_t i=1; i <= NBR_CHANNELS; i++) {  // channels start from 1
	   writeChan(i, DEFAULT_PULSE_WIDTH);  // store default values
	}
	servos[FRAME_SYNC_INDEX].counter = FRAME_SYNC_DELAY;   // store the frame sync period

	Channel = 0;  // clear the channel index
	ISRCount = 0;  // clear the value of the ISR counter;

	/* setup for timer 2 */
	TIMSK2 = 0;  // disable interrupts
	TCCR2A = 0;  // normal counting mode
	TCCR2B = _BV(CS21); // set prescaler of 8
	TCNT2 = 0;     // clear the timer2 count
	TIFR2 = _BV(TOV2);  // clear pending interrupts;
	TIMSK2 =  _BV(TOIE2) ; // enable the overflow interrupt

	isStarted = true;  // flag to indicate this initialisation code has been executed
}  