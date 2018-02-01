
#define DEF_DEL_UNT_NEXT_NOTA 500

nota(unsigned int _kolokol = 1, unsigned int _delayUntilNextNota = DEF_DEL_UNT_NEXT_NOTA){
	unsigned int kolokol = _kolokol;
	unsigned int delayUntilNextNota = _delayUntilNextNota;
	Serial.print("nota ");
	Serial.print(kolokol);
	Serial.print(" delay ");
	Serial.println(delayUntilNextNota);
	delay(delayUntilNextNota);
}

combo2(int _kolokol1 = 1, int _kolokol2 = 2, unsigned int _delayUntilNextNota = DEF_DEL_UNT_NEXT_NOTA){
	int kolokol1 = _kolokol1;
	int kolokol2 = _kolokol2;
	unsigned int delayUntilNextNota = _delayUntilNextNota;
	Serial.print("combo2 ");
	Serial.print(kolokol1);
	Serial.print(", ");
	Serial.print(kolokol2);
	Serial.print(" delay ");
	Serial.println(delayUntilNextNota);
	delay(delayUntilNextNota);
}

combo3(int _kolokol1 = 1, int _kolokol2 = 2, int _kolokol3 = 3, unsigned int _delayUntilNextNota = DEF_DEL_UNT_NEXT_NOTA){
	int kolokol1 = _kolokol1;
	int kolokol2 = _kolokol2;
	int kolokol3 = _kolokol3;
	unsigned int delayUntilNextNota = _delayUntilNextNota;
	Serial.print("combo3 ");
	Serial.print(kolokol1);
	Serial.print(", ");
	Serial.print(kolokol2);
	Serial.print(", ");
	Serial.print(kolokol3);
	Serial.print(" delay ");
	Serial.println(delayUntilNextNota);
	delay(delayUntilNextNota);
}

#define DEFMaxMasSize 20
unsigned int masMelodiaMas [2][DEFMaxMasSize] = {
	{0,  1,  5,2,  8,  3,2,1,  2,1,  5,  4,2,  1,  9,0,0,0,0,0},
	{0,500,0,200,100,0,0,500,0,100,500,0,500,100,200,0,0,0,0,0}};

// 0 - flag
unsigned int masComboMas[9] = {0,0,0,0,0,0,0,0,0};

byte counterComboMas = 1;

void FmasComboMasClear(){
	for(int i=0; i <9; i++){
		masComboMas[i] = 0;
	}
}

void FmasMelodiaPlayForBT(){
	masMelodiaMas[1][0] = 0;
	masMelodiaMas[0][0] = 0;
	for(int i=1; i < DEFMaxMasSize; i++){
		Serial.print("i = ");
		Serial.println(i);
		if(masMelodiaMas[0][i] == 0){
			Serial.println("stop. koncilis noti. nota == 0. masMelodiaMas[0][i] == 0");
			break; 
		}
		if(masMelodiaMas[1][i] > 0){
			if(masComboMas[0] == 0){
				nota(masMelodiaMas[0][i], masMelodiaMas[1][i]);
				masMelodiaMas[1][0] = masMelodiaMas[1][0] + masMelodiaMas[1][i];
				masMelodiaMas[0][0]++;
				Serial.print(masMelodiaMas[1][0]);
  				Serial.println(" ms");
			}else{
				masComboMas[counterComboMas] = masMelodiaMas[0][i];
				if(counterComboMas == 2){
					combo2(masComboMas[1], masComboMas[2], masMelodiaMas[1][i]);
					masMelodiaMas[1][0] = masMelodiaMas[1][0] + masMelodiaMas[1][i];
					masMelodiaMas[0][0]++;
					Serial.print(masMelodiaMas[1][0]);
  					Serial.println(" ms");
				}else if(counterComboMas == 3){
					combo3(masComboMas[1], masComboMas[2] ,masComboMas[3], masMelodiaMas[1][i]);
					masMelodiaMas[1][0] = masMelodiaMas[1][0] + masMelodiaMas[1][i];
					masMelodiaMas[0][0]++;
					Serial.print(masMelodiaMas[1][0]);
  					Serial.println(" ms");
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
			if(counterComboMas > 3){counterComboMas = 3;}
		}
	}
	Serial.print(masMelodiaMas[0][0]);
  	Serial.println(" not");
  	Serial.print(masMelodiaMas[1][0]);
  	Serial.println(" ms");
}

void setup(){
	Serial.begin(9600);
  delay(50);
  Serial.println("");
  Serial.println("start test");
  delay(500);
  FmasMelodiaPlayForBT();
  delay(500);
  Serial.println("stop test");
  Serial.println("");
}

void loop(){
}
