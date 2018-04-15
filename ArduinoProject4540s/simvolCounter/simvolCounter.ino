
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

#ifdef QWERTYUIOP
// Simvol counter C++
#include <iostream>
#include <string>

float cenaRUS = 0.10;
float cenaENG = 0.20;

char simvol;
int simvolInt;
int textIterator = 0;
int stroka = 0;
unsigned int RUSsimvolCounter = 0;
unsigned int ENGsimvolCounter = 0;

unsigned int sumRUSsimvolCounter = 0;
unsigned int sumENGsimvolCounter = 0;

bool flagCiklaWhile = 1;

int main()
{
    std::string textVal;

    while(1){

    	std::cout << "enter text >> ";

	    getline (std::cin, textVal);
	    flagCiklaWhile = 1;
	    textIterator = 0;
	    RUSsimvolCounter = 0;
	    ENGsimvolCounter = 0;
	    stroka ++;
	    while(flagCiklaWhile){

	    	simvolInt = textVal[textIterator];
	    	if(simvolInt > 0){
	    		ENGsimvolCounter ++;
	    	}
	    	else if(simvolInt < 0){
	    		RUSsimvolCounter ++;
	    	}
	    	else if(simvolInt == 0){
	    		flagCiklaWhile = 0;
	    	}

	    	textIterator ++;
	    }
	    RUSsimvolCounter /= 4;
	    sumRUSsimvolCounter += RUSsimvolCounter;
	    sumENGsimvolCounter += ENGsimvolCounter;

	    std::cout << "\n" << "cenaRUS = " << cenaRUS << " grn" << "\n" << "cenaENG =" << cenaENG << " grn" << "\n\n";
	    std::cout << "simvolov RUS = " << RUSsimvolCounter << " / " << RUSsimvolCounter * cenaRUS << " grn" << "\n";
	    std::cout << "simvolov ENG = " << ENGsimvolCounter << " / " << ENGsimvolCounter * cenaENG << " grn" << "\n\n";
	    std::cout << "simvolov VSEGO = " << ENGsimvolCounter + RUSsimvolCounter << " / " << (ENGsimvolCounter * cenaENG) + (RUSsimvolCounter * cenaRUS) << " grn" << "\n";
	    std::cout << "---------------------------------" << "\n\n";
	    std::cout << "summa vseh simvolov" << "\n";
	    std::cout << "SUM_RUS = " << sumRUSsimvolCounter << " / " << sumRUSsimvolCounter * cenaRUS << " grn" << "\n";
	    std::cout << "SUM_ENG = " << sumENGsimvolCounter << " / " << sumENGsimvolCounter * cenaENG << " grn" << "\n\n";
	    std::cout << "SUM_VSEGO = " << sumENGsimvolCounter + sumRUSsimvolCounter << " / " << (sumENGsimvolCounter * cenaENG) + (sumRUSsimvolCounter * cenaRUS) << " grn" << "\n";
	    std::cout << "=================================" << "\n";
	    std::cout << "strok " << stroka << "\n";
	    std::cout << "=================================" << "\n\n\n\n\n\n\n";
	}
}

#endif