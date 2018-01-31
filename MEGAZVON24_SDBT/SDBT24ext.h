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
	FmasMelodiaClear();
	masMelodiaMas[0][0] = 100;
}

void FmasMelodiaPlayForBT(){
	for(int i=1; i < DEFMaxMasSize; i++){
		if(masMelodiaMas[0][i] == 0){break;}
		//if(i > masMelodiaMas[0][0]){break;}
		if(masMelodiaMas[1][i] > 0){
			if(masComboMas[0] == 0){
				nota(masMelodiaMas[0][i], masMelodiaMas[1][i]);
			}else{
				if(counterComboMas == 2){
					combo2(masComboMas[1], masComboMas[2], masMelodiaMas[1][i]);
				}else if(counterComboMas == 3){
					combo3(masComboMas[1], masComboMas[2] ,masComboMas[3], masMelodiaMas[1][i]);
				}else if(counterComboMas == 4){
					combo4(masComboMas[1], masComboMas[2], masComboMas[3], masComboMas[4], masMelodiaMas[1][i]);
				}else if(counterComboMas == 5){
					combo5(masComboMas[1], masComboMas[2], masComboMas[3], masComboMas[4], masComboMas[5], masMelodiaMas[1][i]);
				}else if(counterComboMas == 6){
					combo6(masComboMas[1], masComboMas[2], masComboMas[3], masComboMas[4], masComboMas[5], masComboMas[6], masMelodiaMas[1][i]);
				}else if(counterComboMas == 7){
					combo7(masComboMas[1], masComboMas[2], masComboMas[3], masComboMas[4], masComboMas[5], masComboMas[6], masComboMas[7], masMelodiaMas[1][i]);
				}else if(counterComboMas == 8){
					combo8(masComboMas[1], masComboMas[2], masComboMas[3], masComboMas[4], masComboMas[5], masComboMas[6], masComboMas[7], masComboMas[8], masMelodiaMas[1][i]);
				}
				counterComboMas = 1;
				masComboMas[0] = 0;
				//FmasComboMasClear();
			}
		}else{
			if(masComboMas[0] == 0){masComboMas[0] = 1;}
			masComboMas[counterComboMas] = masMelodiaMas[0][i];
			counterComboMas ++;
			if(counterComboMas > 8){counterComboMas = 8;}
		}
	}
}

void FmasMelodiaPlay(byte _indexM){
	byte indexM = _indexM;
	if(masMelodiaMas[0][0] != 0){
		FmasMelodiaClear();
	}
	FmasMelodiaGetForFile(indexM);
	for(int i=1; i < DEFMaxMasSize; i++){
		if(masMelodiaMas[0][i] == 0){break;}
		//if(i > masMelodiaMas[0][0]){break;}
		nota(masMelodiaMas[0][i], masMelodiaMas[1][i]);
	}
	FmasMelodiaClear();
}