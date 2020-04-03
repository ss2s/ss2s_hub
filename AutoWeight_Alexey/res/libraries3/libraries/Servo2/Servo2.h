#ifndef Servo2_h
#define Servo2_h

#include <inttypes.h>

typedef uint8_t byte;

#define MIN_PULSE_WIDTH	 544	  // the shortest pulse sent to a Servo2  
#define MAX_PULSE_WIDTH	2400	  // the longest pulse sent to a Servo2
#define DEFAULT_PULSE_WIDTH  1500	  // default pulse width when Servo2 is attached
#define FRAME_SYNC_PERIOD   20000	  // total frame duration in microseconds
#define NBR_CHANNELS 8			 // the maximum number of channels, don't change this

typedef struct  {
	uint8_t nbr	  :5 ;  // a pin number from 0 to 31
	uint8_t isActive   :1 ;  // false if this channel not enabled, pin only pulsed if true
   } ServoPin_t   ;  

typedef struct {
  ServoPin_t Pin;
  byte counter;
  byte remainder;
}  servo_t;

class Servo2
{
  public:
	Servo2();
	uint8_t attach(int pin);
	uint8_t attach(int pin, int min, int max);
    void detach();
    void write(int value);
    void writeMicroseconds(int value);
    int read();
    int readMicroseconds();
	bool attached();
 private:
	 uint8_t chanIndex;
};

#endif