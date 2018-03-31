
#define cenaRUS 5
#define cenaENG 10

float coefCenRUS = 100 / cenaRUS;
float coefCenENG = 100 / cenaENG;

char simvol;
unsigned int RUSsimvolCounter = 0;
unsigned int ENGsimvolCounter = 0;

unsigned int sumRUSsimvolCounter = 0;
unsigned int sumENGsimvolCounter = 0;

void setup(){

	Serial.begin(115200);
}

void loop(){

	if (Serial.available()){

		Serial.println("<");
		//Serial.println("");
		while(Serial.available()){

			simvol = Serial.read();
		
    		Serial.print(simvol);
    		Serial.print(" / ");
    		Serial.print((int)simvol);
    		Serial.print(" = ");

    		if(simvol < 0){

    			Serial.print("RUS / ");
    			RUSsimvolCounter ++;
    			Serial.println(RUSsimvolCounter);
    		}else{

    			Serial.print("ENG / ");
    			ENGsimvolCounter ++;
    			Serial.println(ENGsimvolCounter);
    		}
		}

    	//Serial.println("");
    	Serial.println(">");
		Serial.print("RUS = ");
		Serial.print(RUSsimvolCounter);
		Serial.print(" / ");
		Serial.print(RUSsimvolCounter / coefCenRUS);
		Serial.println(" GRN");

		Serial.print("ENG = ");
		Serial.print(ENGsimvolCounter);
		Serial.print(" / ");
		Serial.print(ENGsimvolCounter / coefCenENG);
		Serial.println(" GRN");

		Serial.print("VSEGO / ");
		Serial.print(RUSsimvolCounter + ENGsimvolCounter);
		Serial.print(" = ");
		Serial.print((RUSsimvolCounter / coefCenRUS) + (ENGsimvolCounter / coefCenENG));
		Serial.println(" GRN");
		Serial.println("-----------------------");

		sumRUSsimvolCounter += RUSsimvolCounter;
        sumENGsimvolCounter += ENGsimvolCounter;
        RUSsimvolCounter = 0;
        ENGsimvolCounter = 0;

		Serial.print("SUM_RUS = ");
		Serial.print(sumRUSsimvolCounter);
		Serial.print(" / ");
		Serial.print(sumRUSsimvolCounter / coefCenRUS);
		Serial.println(" GRN");

		Serial.print("SUM_ENG = ");
		Serial.print(sumENGsimvolCounter);
		Serial.print(" / ");
		Serial.print(sumENGsimvolCounter / coefCenENG);
		Serial.println(" GRN");

		Serial.print("SUM_VSEGO / ");
		Serial.print(sumRUSsimvolCounter + sumENGsimvolCounter);
		Serial.print(" = ");
		Serial.print((sumRUSsimvolCounter / coefCenRUS) + (sumENGsimvolCounter / coefCenENG));
		Serial.println(" GRN");
		Serial.println("-----------------------");
    }
}