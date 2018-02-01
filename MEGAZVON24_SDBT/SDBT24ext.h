////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        /******************************************************************************************************\
          MEGAZVON24  Arduino MEGA2560 программа для автоматического перезвона колоколов по заданному времени.

          https://github.com/ss2s/MEGAZVON24_MEGA2560
        \******************************************************************************************************/

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  ТЕХНИЧЕСКИЙ РАЗДЕЛ SDBT
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// функции SDBT
#define DEFMaxMasSize 100
unsigned int masMelodiaMas [2][DEFMaxMasSize];

// 0 - flag
unsigned int masComboMas[9] = {0,0,0,0,0,0,0,0,0};
byte counterComboMas = 1;

bool flagBT = 0;

unsigned long BTstartMill = 1UL;
unsigned long BTstopMill = 1UL;

byte flagMenu = 1;
byte flagMenu2 = 1;

char b_d; // bluetooth data. входные данные с блютуз. переменная для хранения считывания первого управляющего байта
int bt_not = 0; // входные данные с блютуз. переменная для считывания хранения данных. формат: 1 - 24


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void BTloop(){
	while(1){
		if (Serial1.available()){
    		b_d = Serial1.read();  // считывает первый управляющий байт (символ'x') определяющий дальнейшие действия в переменную b_d
    
    		if (b_d == 'k'){       // если k то нажата кнопка колокола
    	  		bt_not = Serial1.parseInt(); //  считывает число int в переменную bt_not до первого символа
    	  		Serial.print("n ");
    	  		Serial.println(bt_not);
    		}
    		/*else if (b_d == 'e'){  // если e то неотпущена кнопка колокола
    		}*/
    		else if (b_d == 'p'){  // если p то отпущена кнопка колокола
    			Serial.println("p");
    		}
    		else if (b_d == 'S'){  // селект
    			break;
    		}
    		else if (b_d == 'L'){
    		}
    		else if (b_d = 'R'){
    		}
  		}
  		if(flagMenu == 1){
  			Serial1.print("*t"); // отправка признака строки меню
    		Serial1.println("button to play. press select to start"); // отправка строки меню
  		}else if(flagMenu == 2){
  			Serial1.print("*t"); // отправка признака строки меню
    		Serial1.println("play melody. press select to start"); // отправка строки меню
  		}else if(flagMenu == 3){
  			Serial1.print("*t"); // отправка признака строки меню
    		Serial1.println("record melody. press select to start"); // отправка строки меню
  		}else if(flagMenu == 4){
  			Serial1.print("*t"); // отправка признака строки меню
    		Serial1.println("save to file. press select to start"); // отправка строки меню
  		}
	}
}

void FmasComboMasClear(){
	for(int i=0; i <9; i++){
		masComboMas[i] = 0;
	}
}

void FmasMelodiaClear(){
	for(int i=0; i < DEFMaxMasSize; i++){
		masMelodiaMas[0][i] = 0;
		masMelodiaMas[1][i] = 0;
	}
}

void FmasMelodiaGetForBT(){}

void FmasMelodiaSetToFile(byte _indexM){}

void FmasMelodiaGetForFile(byte _indexM){
	if(masMelodiaMas[0][0] != 0){
		FmasMelodiaClear();
	}
}

void FmasMelodiaPlay(){
	masMelodiaMas[1][0] = 0;
	masMelodiaMas[0][0] = 0;
	for(int i=1; i < DEFMaxMasSize; i++){
		if(masMelodiaMas[0][i] == 0){
			break;
		}
		if(masMelodiaMas[1][i] > 0){
			if(masComboMas[0] == 0){
				nota(masMelodiaMas[0][i], masMelodiaMas[1][i]);
				masMelodiaMas[1][0] = masMelodiaMas[1][0] + masMelodiaMas[1][i];
				masMelodiaMas[0][0]++;
			}else{
				masComboMas[counterComboMas] = masMelodiaMas[0][i];
				if(counterComboMas == 2){
					combo2(masComboMas[1], masComboMas[2], masMelodiaMas[1][i]);
					masMelodiaMas[1][0] = masMelodiaMas[1][0] + masMelodiaMas[1][i];
					masMelodiaMas[0][0]++;
				}else if(counterComboMas == 3){
					combo3(masComboMas[1], masComboMas[2] ,masComboMas[3], masMelodiaMas[1][i]);
					masMelodiaMas[1][0] = masMelodiaMas[1][0] + masMelodiaMas[1][i];
					masMelodiaMas[0][0]++;
				}else if(counterComboMas == 4){
					combo4(masComboMas[1], masComboMas[2], masComboMas[3], masComboMas[4], masMelodiaMas[1][i]);
					masMelodiaMas[1][0] = masMelodiaMas[1][0] + masMelodiaMas[1][i];
					masMelodiaMas[0][0]++;
				}else if(counterComboMas == 5){
					combo5(masComboMas[1], masComboMas[2], masComboMas[3], masComboMas[4], masComboMas[5], masMelodiaMas[1][i]);
					masMelodiaMas[1][0] = masMelodiaMas[1][0] + masMelodiaMas[1][i];
					masMelodiaMas[0][0]++;
				}else if(counterComboMas == 6){
					combo6(masComboMas[1], masComboMas[2], masComboMas[3], masComboMas[4], masComboMas[5], masComboMas[6], masMelodiaMas[1][i]);
					masMelodiaMas[1][0] = masMelodiaMas[1][0] + masMelodiaMas[1][i];
					masMelodiaMas[0][0]++;
				}else if(counterComboMas == 7){
					combo7(masComboMas[1], masComboMas[2], masComboMas[3], masComboMas[4], masComboMas[5], masComboMas[6], masComboMas[7], masMelodiaMas[1][i]);
					masMelodiaMas[1][0] = masMelodiaMas[1][0] + masMelodiaMas[1][i];
					masMelodiaMas[0][0]++;
				}else if(counterComboMas == 8){
					combo8(masComboMas[1], masComboMas[2], masComboMas[3], masComboMas[4], masComboMas[5], masComboMas[6], masComboMas[7], masComboMas[8], masMelodiaMas[1][i]);
					masMelodiaMas[1][0] = masMelodiaMas[1][0] + masMelodiaMas[1][i];
					masMelodiaMas[0][0]++;
				}
				counterComboMas = 1;
				masComboMas[0] = 0;
				FmasComboMasClear();
			}
		}else{
			if(masComboMas[0] == 0){masComboMas[0] = 1;}
			masComboMas[counterComboMas] = masMelodiaMas[0][i];
			counterComboMas ++;
			masMelodiaMas[0][0]++;
			if(counterComboMas > 8){counterComboMas = 8;}
		}
	}
}

void FmasMelodiaPlayForFile(byte _indexM){
	byte indexM = _indexM;
	if(masMelodiaMas[0][0] != 0){
		FmasMelodiaClear();
	}

	FmasMelodiaGetForFile(indexM);
	FmasMelodiaPlay();
	FmasMelodiaClear();
}