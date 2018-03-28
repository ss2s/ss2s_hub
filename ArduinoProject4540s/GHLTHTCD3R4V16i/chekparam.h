////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//
//   ██████╗    ██╗  ██╗    ███████╗    ██╗  ██╗            ██████╗      █████╗     ██████╗      █████╗     ███╗   ███╗
//  ██╔════╝    ██║  ██║    ██╔════╝    ██║ ██╔╝            ██╔══██╗    ██╔══██╗    ██╔══██╗    ██╔══██╗    ████╗ ████║
//  ██║         ███████║    █████╗      █████╔╝             ██████╔╝    ███████║    ██████╔╝    ███████║    ██╔████╔██║
//  ██║         ██╔══██║    ██╔══╝      ██╔═██╗             ██╔═══╝     ██╔══██║    ██╔══██╗    ██╔══██║    ██║╚██╔╝██║
//  ╚██████╗    ██║  ██║    ███████╗    ██║  ██╗            ██║         ██║  ██║    ██║  ██║    ██║  ██║    ██║ ╚═╝ ██║
//   ╚═════╝    ╚═╝  ╚═╝    ╚══════╝    ╚═╝  ╚═╝            ╚═╝         ╚═╝  ╚═╝    ╚═╝  ╚═╝    ╚═╝  ╚═╝    ╚═╝     ╚═╝
//
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// main function
void chekLightLoop(){

	if(svetociklType == 0){  // off

		return;
	}else if(svetociklType == 1){  // svetocikl

		getDateDs1307(&second, &minute, &hour, &dayOfWeek, &dayOfMonth, &month, &year);  // запрос текущего времени

		if((hour == vremyaVkluchenieSveta[0]) 
			&& (minute >= vremyaVkluchenieSveta[1]) 
			&& (flagSvetocikla == 0) 
			&& ((hour < vremyaVikluchenieSveta[0]) 
				|| ((hour == vremyaVikluchenieSveta[0]) 
					&& (minute < vremyaVikluchenieSveta[1])))){

			digitalWrite(OUT_SVETTIMER_RELE_PIN, DW_H);
			flagSvetocikla = 1;
		}
		else if((hour == vremyaVikluchenieSveta[0]) 
			&& (minute >= vremyaVikluchenieSveta[1]) 
			&& (flagSvetocikla == 1)){

			digitalWrite(OUT_SVETTIMER_RELE_PIN, DW_L);
			flagSvetocikla = 0;
		}
	}else if(svetociklType == 2){  // dosvetka

		getDateDs1307(&second, &minute, &hour, &dayOfWeek, &dayOfMonth, &month, &year);  // запрос текущего времени

		if((hour == vremyaVkluchenieSveta[0]) 
			&& (minute >= vremyaVkluchenieSveta[1]) 
			&& (flagSvetocikla == 0) 
			&& (flagSvetocikla2 == 0)
			&& ((hour < vremyaVikluchenieSveta[0]) 
				|| ((hour == vremyaVikluchenieSveta[0]) 
					&& (minute < vremyaVikluchenieSveta[1])))){

			digitalWrite(OUT_SVETTIMER_RELE_PIN, DW_H);
			flagSvetocikla = 1;
		}
		else if((hour == vremyaVikluchenieSveta[0]) 
			&& (minute >= vremyaVikluchenieSveta[1]) 
			&& (flagSvetocikla == 1)
			&& (flagSvetocikla2 == 0)){

			digitalWrite(OUT_SVETTIMER_RELE_PIN, DW_L);
			flagSvetocikla = 0;
		}else if((hour == vremyaVkluchenieSveta2[0]) 
			&& (minute >= vremyaVkluchenieSveta2[1]) 
			&& (flagSvetocikla2 == 0)
			&& (flagSvetocikla == 0) 
			&& ((hour < vremyaVikluchenieSveta2[0]) 
				|| ((hour == vremyaVikluchenieSveta2[0]) 
					&& (minute < vremyaVikluchenieSveta2[1])))){

			digitalWrite(OUT_SVETTIMER_RELE_PIN, DW_H);
			flagSvetocikla2 = 1;
		}
		else if((hour == vremyaVikluchenieSveta2[0]) 
			&& (minute >= vremyaVikluchenieSveta2[1]) 
			&& (flagSvetocikla2 == 1)
			&& (flagSvetocikla == 0)){

			digitalWrite(OUT_SVETTIMER_RELE_PIN, DW_L);
			flagSvetocikla2 = 0;
		}
	}
}






////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void vremyaViklucheniaNasosaFind(byte tHour, byte tMinute, byte addHour, byte addMinute){

	byte resVVNF[2] = {0, 0};

	resVVNF[0] = tHour;
	resVVNF[1] = tMinute;

	resVVNF[1] += addMinute;
	if(resVVNF[1] > 60){
		resVVNF[1] -= 60;
		resVVNF[0] += 1;
	}
	resVVNF[0] += addHour;
	if(resVVNF[0] > 24){
		resVVNF [0] -= 24;
	}

	vremyaViklucheniaNasosa[0] = resVVNF[0];
	vremyaViklucheniaNasosa[1] = resVVNF[1];
}

void chekPolivLoop(){

	if(polivaRejim == 0){ // otklucheno

		return;
	}else if(polivaRejim == 1){ // ciklicheskiy

		getDateDs1307(&second, &minute, &hour, &dayOfWeek, &dayOfMonth, &month, &year);  // запрос текущего времени

		if((hour == vremyaPoliva1[0]) 
			&& (minute >= vremyaPoliva1[1]) 
			&& (flagPoliva1 == 0) 
			&& ((hour < vremyaViklucheniaNasosa[0]) 
				|| ((hour == vremyaViklucheniaNasosa[0]) 
					&& (minute < vremyaViklucheniaNasosa[1])))){

			flagPoliva1 = 1;
			vremyaViklucheniaNasosaFind(hour, minute, vremyaRabotiNasosa[0], vremyaRabotiNasosa[1]);
			digitalWrite(OUT_POLIVTIMER_RELE_PIN, DW_H);
		}
		else if((hour == vremyaViklucheniaNasosa[0]) 
			&& (minute >= vremyaViklucheniaNasosa[1]) 
			&& (flagSvetocikla == 1)){

			digitalWrite(OUT_POLIVTIMER_RELE_PIN, DW_L);
			flagPoliva1 = 0;
		}
	}else if(polivaRejim == 2){ // po vremeni
		
		getDateDs1307(&second, &minute, &hour, &dayOfWeek, &dayOfMonth, &month, &year);  // запрос текущего времени

		if((hour == vremyaPoliva1[0])                               // 1
			&& (minute >= vremyaPoliva1[1]) 
			&& (flagPoliva1 == 0) 
			&& (flagPoliva2 == 0)
			&& (flagPoliva3 == 0)
			&& ((hour < vremyaViklucheniaNasosa[0]) 
				|| ((hour == vremyaViklucheniaNasosa[0]) 
					&& (minute < vremyaViklucheniaNasosa[1])))){

			flagPoliva1 = 1;
			vremyaViklucheniaNasosaFind(hour, minute, vremyaRabotiNasosa[0], vremyaRabotiNasosa[1]);
			digitalWrite(OUT_POLIVTIMER_RELE_PIN, DW_H);
		}
		else if((hour == vremyaViklucheniaNasosa[0]) 
			&& (minute >= vremyaViklucheniaNasosa[1]) 
			&& (flagPoliva1 == 1)
			&& (flagPoliva2 == 0)
			&& (flagPoliva3 == 0)){

			digitalWrite(OUT_POLIVTIMER_RELE_PIN, DW_L);
			flagPoliva1 = 0;
		}else if((hour == vremyaPoliva2[0])                         // 2
			&& (minute >= vremyaPoliva2[1]) 
			&& (flagPoliva1 == 0) 
			&& (flagPoliva2 == 0)
			&& (flagPoliva3 == 0)
			&& ((hour < vremyaViklucheniaNasosa[0]) 
				|| ((hour == vremyaViklucheniaNasosa[0]) 
					&& (minute < vremyaViklucheniaNasosa[1])))){

			flagPoliva2 = 1;
			vremyaViklucheniaNasosaFind(hour, minute, vremyaRabotiNasosa[0], vremyaRabotiNasosa[1]);
			digitalWrite(OUT_POLIVTIMER_RELE_PIN, DW_H);
		}
		else if((hour == vremyaViklucheniaNasosa[0]) 
			&& (minute >= vremyaViklucheniaNasosa[1]) 
			&& (flagPoliva1 == 0)
			&& (flagPoliva2 == 1)
			&& (flagPoliva3 == 0)){

			digitalWrite(OUT_POLIVTIMER_RELE_PIN, DW_L);
			flagPoliva2 = 0;
		}else if((hour == vremyaPoliva3[0])                         // 3
			&& (minute >= vremyaPoliva3[1]) 
			&& (flagPoliva1 == 0) 
			&& (flagPoliva2 == 0)
			&& (flagPoliva3 == 0)
			&& ((hour < vremyaViklucheniaNasosa[0]) 
				|| ((hour == vremyaViklucheniaNasosa[0]) 
					&& (minute < vremyaViklucheniaNasosa[1])))){

			flagPoliva3 = 1;
			vremyaViklucheniaNasosaFind(hour, minute, vremyaRabotiNasosa[0], vremyaRabotiNasosa[1]);
			digitalWrite(OUT_POLIVTIMER_RELE_PIN, DW_H);
		}
		else if((hour == vremyaViklucheniaNasosa[0]) 
			&& (minute >= vremyaViklucheniaNasosa[1]) 
			&& (flagPoliva1 == 0)
			&& (flagPoliva2 == 0)
			&& (flagPoliva3 == 1)){

			digitalWrite(OUT_POLIVTIMER_RELE_PIN, DW_L);
			flagPoliva3 = 0;
		}
	}else if(polivaRejim == 3){ // ot datchika

		getDateDs1307(&second, &minute, &hour, &dayOfWeek, &dayOfMonth, &month, &year);  // запрос текущего времени

		if(datchikPolivaTypeSet == 1){                             // цифровой датчик  почвы

			bool datchikPochviVal = digitalRead(EXTENDED_IN_DATCHIK_POCVI);

			if((datchikPochviVal == datchikPolivaValSet) && (flagPoliva1 == 0)){

				flagPoliva1 = 1;
				vremyaViklucheniaNasosaFind(hour, minute, vremyaRabotiNasosa[0], vremyaRabotiNasosa[1]);
				digitalWrite(OUT_POLIVTIMER_RELE_PIN, DW_H);
			}

			if((hour == vremyaViklucheniaNasosa[0]) 
			&& (minute >= vremyaViklucheniaNasosa[1]) 
			&& (flagSvetocikla == 1)){

			digitalWrite(OUT_POLIVTIMER_RELE_PIN, DW_L);
			flagPoliva1 = 0;
			}
		}else{                                                     // аналоговый датчик почвы

			int datchikPochviVal = analogRead(EXTENDED_IN_DATCHIK_POCVI);

			if((datchikPochviVal >= datchikPolivaValAnalogSet) && (flagPoliva1 == 0)){

				flagPoliva1 = 1;
				vremyaViklucheniaNasosaFind(hour, minute, vremyaRabotiNasosa[0], vremyaRabotiNasosa[1]);
				digitalWrite(OUT_POLIVTIMER_RELE_PIN, DW_H);
			}

			if((hour == vremyaViklucheniaNasosa[0]) 
			&& (minute >= vremyaViklucheniaNasosa[1]) 
			&& (flagSvetocikla == 1)){

			digitalWrite(OUT_POLIVTIMER_RELE_PIN, DW_L);
			flagPoliva1 = 0;
			}
		}
	}
}




////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void chekTempLoop(){

	if(tempRejim == 0){        // off

		return;
	}else if(tempRejim == 1){  // если охлаждение

		getDhtData(&TEMP, &HMDT);  // запрос текущей температуры и влажности

		float temperaturaUderjaniaAndGisterezis = temperaturaUderjania + gisterezisTemperaturi;

		if(TEMP >= temperaturaUderjaniaAndGisterezis){
			flagTermorele = 1;
			digitalWrite(OUT_TEMPERATURA_RELE_PIN, DW_H);
		}else if(TEMP < temperaturaUderjania){
			digitalWrite(OUT_TEMPERATURA_RELE_PIN, DW_L);
			flagTermorele = 0;
		}
	}else if(tempRejim == 2){  // если нагрев    

		getDhtData(&TEMP, &HMDT);  // запрос текущей температуры и влажности

		float temperaturaUderjaniaAndGisterezis = temperaturaUderjania - gisterezisTemperaturi;

		if(TEMP <= temperaturaUderjaniaAndGisterezis){
			flagTermorele = 1;
			digitalWrite(OUT_TEMPERATURA_RELE_PIN, DW_H);
		}else if(TEMP > temperaturaUderjania){
			digitalWrite(OUT_TEMPERATURA_RELE_PIN, DW_L);
			flagTermorele = 0;
		}
	}else if(tempRejim == 3){  // если настраиваемый

		getDhtData(&TEMP, &HMDT);  // запрос текущей температуры и влажности

		if(temperaturaVkluchenia > temperaturaVikluchenia){

			if(TEMP >= temperaturaVkluchenia){
				flagTermorele = 1;
				digitalWrite(OUT_TEMPERATURA_RELE_PIN, DW_H);
			}else if(TEMP < temperaturaVikluchenia){
				digitalWrite(OUT_TEMPERATURA_RELE_PIN, DW_L);
				flagTermorele = 0;
			}
		}
		else if(temperaturaVkluchenia == temperaturaVikluchenia){

			digitalWrite(OUT_TEMPERATURA_RELE_PIN, DW_L);
			flagTermorele = 0;
		}
		else if(temperaturaVkluchenia < temperaturaVikluchenia){

			if(TEMP <= temperaturaVkluchenia){
				flagTermorele = 1;
				digitalWrite(OUT_TEMPERATURA_RELE_PIN, DW_H);
			}else if(TEMP > temperaturaVikluchenia){
				digitalWrite(OUT_TEMPERATURA_RELE_PIN, DW_L);
				flagTermorele = 0;
			}
		}
	}
}





////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void chekHumidityLoop(){

	if(vlajnostRejim == 0){        // off

		return;
	}else if(vlajnostRejim == 1){  // если сушка

		getDhtData(&TEMP, &HMDT);  // запрос текущей температуры и влажности

		float vlajnostUderjaniaAndGisterezis = vlajnostUderjania + gisterezisVlajnosti;

		if(HMDT >= vlajnostUderjaniaAndGisterezis){
			flagHumidityrele = 1;
			digitalWrite(OUT_VLAJNOST_RELE_PIN, DW_H);
		}else if(HMDT < vlajnostUderjania){
			digitalWrite(OUT_VLAJNOST_RELE_PIN, DW_L);
			flagHumidityrele = 0;
		}
	}else if(vlajnostRejim == 2){  // если увлажнение   

		getDhtData(&TEMP, &HMDT);  // запрос текущей температуры и влажности

		float vlajnostUderjaniaAndGisterezis = vlajnostUderjania - gisterezisVlajnosti;

		if(HMDT <= vlajnostUderjaniaAndGisterezis){
			flagHumidityrele = 1;
			digitalWrite(OUT_VLAJNOST_RELE_PIN, DW_H);
		}else if(HMDT > vlajnostUderjania){
			digitalWrite(OUT_VLAJNOST_RELE_PIN, DW_L);
			flagHumidityrele = 0;
		}
	}else if(vlajnostRejim == 3){  // если настраиваемый   

		getDhtData(&TEMP, &HMDT);  // запрос текущей температуры и влажности

		if(vlajnostVkluchenia > vlajnostVikluchenia){

			if(HMDT >= vlajnostVkluchenia){
				flagHumidityrele = 1;
				digitalWrite(OUT_VLAJNOST_RELE_PIN, DW_H);
			}else if(HMDT < vlajnostVikluchenia){
				digitalWrite(OUT_VLAJNOST_RELE_PIN, DW_L);
				flagHumidityrele = 0;
			}
		}
		else if(vlajnostVkluchenia == vlajnostVikluchenia){

			digitalWrite(OUT_VLAJNOST_RELE_PIN, DW_L);
			flagHumidityrele = 0;
		}
		else if(vlajnostVkluchenia < vlajnostVikluchenia){

			if(HMDT <= vlajnostVkluchenia){
				flagHumidityrele = 1;
				digitalWrite(OUT_VLAJNOST_RELE_PIN, DW_H);
			}else if(HMDT > vlajnostVikluchenia){
				digitalWrite(OUT_VLAJNOST_RELE_PIN, DW_L);
				flagHumidityrele = 0;
			}
		}
	}
}



	
void chekParam(){

	// chek param
	chekLightLoop();
	chekPolivLoop();
	chekTempLoop();
	chekHumidityLoop();
}	