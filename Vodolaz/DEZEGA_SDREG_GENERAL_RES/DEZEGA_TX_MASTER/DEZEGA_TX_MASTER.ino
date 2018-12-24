// MEGA 2560

// calibration F

// CO2 val F
// O2 val F
// CO val F
// clock check. if sec != sec: generate data exchange with the slave and DEZEGA_RX
// clock check. if minute != minute: write data to SD
void setup(){
	Serial1.begin(250000);
	Serial2.begin(250000);
	delay(10);
}

void loop(){
}