#define starter_rele 4
#define airdamper_rele 5
#define stop_rele 6
#define in_220_sensor 7

#define high_starter_rele 0
#define low_starter_rele 1

#define high_airdamper_rele 0
#define low_airdamper_rele 1

#define high_stop_rele 0
#define low_stop_rele 1

#define high_in_220_sensor 0
#define low_in_220_sensor 1

#define control_starter_rele 8
#define control_airdamper_rele 9
#define control_stop_rele 10
#define control_in_220_sensor 11

#define control_airdamper_and_starter 13

boolean state_starter_rele = 0;
boolean state_airdamper_rele = 0;
boolean state_stop_rele = 0;

boolean firstLaunchFlag = 0;

void setup()
{
	// линии выходов входов эмулятора
	pinMode(starter_rele, INPUT);
	pinMode(airdamper_rele, INPUT);
	pinMode(stop_rele, INPUT);
	pinMode(in_220_sensor, OUTPUT);
	digitalWrite(in_220_sensor, low_in_220_sensor);

	// линии контроля для индикаторов, дублируют входы эмолятора с 1 в high
	pinMode(control_starter_rele, OUTPUT);
	pinMode(control_airdamper_rele, OUTPUT);
	pinMode(control_stop_rele, OUTPUT);
	pinMode(control_in_220_sensor, OUTPUT);

	pinMode(control_airdamper_and_starter, OUTPUT);
	digitalWrite(control_airdamper_and_starter, LOW);
}

void loop()
{
	while(1){

	    state_starter_rele = digitalRead(starter_rele);
		state_airdamper_rele = digitalRead(airdamper_rele);
		state_stop_rele = digitalRead(stop_rele);

		if(state_starter_rele == high_starter_rele){
			digitalWrite(control_starter_rele, HIGH);

			delay(200);

			// with first launch control
			if(firstLaunchFlag == 0){
				delay(5000);
				firstLaunchFlag = 1;
			}
			else{
				digitalWrite(in_220_sensor, high_in_220_sensor);
				digitalWrite(control_in_220_sensor, HIGH);
			}

			// without first launch control
			// digitalWrite(in_220_sensor, high_in_220_sensor);
			// digitalWrite(control_in_220_sensor, HIGH);
		}
		else{
			digitalWrite(control_starter_rele, LOW);
		}

		if(state_airdamper_rele == high_airdamper_rele){
			digitalWrite(control_airdamper_rele, HIGH);
		}
		else{
			digitalWrite(control_airdamper_rele, LOW);
		}

		if(state_stop_rele == high_stop_rele){
			digitalWrite(control_stop_rele, HIGH);

			digitalWrite(in_220_sensor, low_in_220_sensor);
			digitalWrite(control_in_220_sensor, LOW);
		}
		else{
			digitalWrite(control_stop_rele, LOW);
		}

		if((state_starter_rele == high_starter_rele) && (state_airdamper_rele == high_airdamper_rele)){
			digitalWrite(control_airdamper_and_starter, HIGH);
		}
		else{
			digitalWrite(control_airdamper_and_starter, LOW);
		}
	}
}
