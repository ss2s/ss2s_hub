/* Arduino code to communicate with xbox 360 RF module.
Original work by (yaywoop) / additional ideas from Alexander Martinez - modified by dilandou (www.dilandou.com, www.diru.org/wordpress)
First sends LED initialisation code followed by LED startup animation code, then sleeps until a button press for sync command.
RF module must be powered with 3.3V, two diodes in series with USB 5v will do. Connect the USB wires to a host computer, and the data and serial wires to Arduino.
of course, make sure to have a common ground */

#include <avr/sleep.h>

#define sync_pin 2 //power button repurposed for sync button (pin 5 on the module)
#define data_pin 3 //data line (pin 6 on the module)
#define clock_pin 4 //clock line (pin 7 on module) 

#define READ_BUF_MAX_SIZE 32  // максимальный размер буфера

#define REBOOT_RECEIVER 0b0011110000
#define START_RECEIVER_LED_FUNCTIONS 0b0010000100
#define START_RECEIVER_LED_FUNCTIONS_CENTER_BLINKING 0b0010001100
#define START_RECEIVER_LED_FUNCTIONS_CENTER_OFF 0b0010001000
#define START_WITH_ANIMATION 0b0010000101
#define RECEIVER_SYNC 0b0000000100
#define RECEIVER_TURN_OFF_CONTROLLER 0b0000001001
#define TURN_TRANCEIVER_OFF 0b0000010000
#define TURN_TRANCEIVER_ON 0b0000010010
#define RECEIVER_ORANGE_ALL_ON 0b0011100001
#define RECEIVER_ORANGE_ALL_OFF 0b0011100000

#define RECEIVER_GREEN_LED 0b0010100000
#define RECEIVER_RED_LED   0b0010110000
#define RECEIVER_LED_1     0b0100
#define RECEIVER_LED_2     0b0001
#define RECEIVER_LED_3     0b1000
#define RECEIVER_LED_4     0b0010

int led_cmd[10] =  {0,0,1,0,0,0,0,1,0,0}; //Activates/initialises the LEDs, leaving the center LED lit.
int anim_cmd[10] = {0,0,1,0,0,0,0,1,0,1}; //Makes the startup animation on the ring of light.
int sync_cmd[10] = {0,0,0,0,0,0,0,1,0,0}; //Initiates the sync process.

uint16_t myreadBuf[READ_BUF_MAX_SIZE];
volatile boolean sync_enable = 0;

void sendData(int cmd_do[]) {
  pinMode(data_pin, OUTPUT);
  digitalWrite(data_pin, LOW);    //start sending data.
  int prev = 1;
  for(int i = 0; i < 10; i++){

    while (prev == digitalRead(clock_pin)){} //detects change in clock
    prev = digitalRead(clock_pin);
      // should be after downward edge of clock, so send bit of data now
    digitalWrite(data_pin, cmd_do[i]);

    while (prev == digitalRead(clock_pin)){} //detects upward edge of clock
    prev = digitalRead(clock_pin);
  }
  digitalWrite(data_pin, HIGH);
  pinMode(data_pin, INPUT);
}

void readData(){}

void readReceiverCommand(uint16_t *command) {
  uint16_t timeout_count = 0;

  for(int8_t i = 10; i >= 0; i--) {
    timeout_count = 0;
     // wait for falling edge or timeout
     while(digitalRead(clock_pin)) {
      delayMicroseconds(50);
      if(timeout_count == 500) {
        goto timeoutFaile;
      }
      timeout_count++;
    }

    *command |= digitalRead(data_pin) << i;
    while(!digitalRead(clock_pin)){delayMicroseconds(50);} // wait for rising edge
  }

  timeoutFaile:
  return;
}

void initLEDs(){
  sendData(led_cmd);
  delay(50);
  sendData(anim_cmd);
  delay(50);
}

void wakeUp(){
  sync_enable = 1;
}

void sleepNow() {
  set_sleep_mode(SLEEP_MODE_PWR_DOWN); // set sleep mode
  sleep_enable(); //enable sleep bit
  attachInterrupt(0, wakeUp, LOW);
  sleep_mode();
  sleep_disable(); //disable sleep bit
  detachInterrupt(0); // disables interrupt 0 on pin 2
}

void setup() {
  Serial.begin(9600);
  pinMode(sync_pin, INPUT);
  digitalWrite(sync_pin,HIGH);
  pinMode(data_pin, INPUT);
  pinMode(clock_pin, INPUT);
  delay(2000);

  initLEDs();
//  sendData(sync_cmd);
}

void loop(){
  Serial.println("Sleeping.");
  sleepNow();
  delay(200);
  if(sync_enable==1) {
    Serial.println("Syncing.");
    sendData(sync_cmd);

    readReceiverCommand(myreadBuf);
      Serial.println("incoming data");
      for(int8_t i = 0; i <= 10; i++){

        Serial.print(myreadBuf[i]);
        delay(1);
      }
      Serial.println();
      Serial.print("incoming data revers");
      for(int8_t i = 10; i >= 0; i--){

        Serial.print(myreadBuf[i]);
        delay(1);
      }
      Serial.println();

    sync_enable = 0;
  }
}
