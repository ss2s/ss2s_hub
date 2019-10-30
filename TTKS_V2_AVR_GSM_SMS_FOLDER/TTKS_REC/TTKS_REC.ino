	ComPort.println(F("-SMScommand prosessing-"));  // Выводим в ComPort для контроля, что ничего не потерялось
	ComPort.print(F("-Key: "));                // Выводим в ComPort для контроля, что ничего не потерялось
	ComPort.println(_sms_command);                // Выводим в ComPort для контроля, что ничего не потерялось

	bool correct = false;                                       // Для оптимизации кода, переменная корректности команды
	String msgToSend = "ok!!!";
	
	if (_sms_command.length() >= 4){
		String _first_simbol = (String)_sms_command[0] + (String)_sms_command[1] + (String)_sms_command[2] + (String)_sms_command[3];



		if(_first_simbol == "stat"){  // Исполняем команду STATUS
			ComPort.println(F("SMScommand status RUN"));  // Выводим в ComPort для контроля, что ничего не потерялось
			if(digitalRead(VOLTAGE_OUT_SENSOR_PIN) == Y_220){
				ComPort.println(F("__dvigatel zapushen"));

				uint32_t _temp_h_var = (millis() - vremya_zapuska_dvigatelya) / 1000 / 60 / 60;
				uint32_t _temp_m90_var = (millis() - vremya_zapuska_dvigatelya) / 1000 / 60;
				uint32_t _temp_m60_var = _temp_m90_var - (_temp_h_var * 60);
				ComPort.print(F("__dvigatel rabotaet "));
				ComPort.print(_temp_h_var);
				ComPort.print(F(":"));
				ComPort.println(_temp_m60_var);

				ComPort.print(F("obshaya_narabotka_v_motochasah - "));
				ComPort.println(obshaya_narabotka_v_motochasah);
				ComPort.print(F("obshaya_narabotka_v_motominutah - "));
				ComPort.println(obshaya_narabotka_v_motominutah);
			}
			else{
				ComPort.println(F("__dvigatel ostanovlen"));
			}

			ComPort.println(F("__informaciya o taimerah"));  //...


			correct = true;
			ComPort.println(F("SMScommand status COMPLETED"));
		}



		else if(_first_simbol == "star"){  // Исполняем команду START >
			ComPort.println(F("SMScommand start RUN"));  // Выводим в ComPort для контроля, что ничего не потерялось
			zapushen_s_popitki = 0;
			int16_t _strt_ansver = startEngineLoopF();

			if(_strt_ansver == 1){
				msgToSend = "generator zapushen s " + (String)zapushen_s_popitki + " popitki";
				vremya_zapuska_dvigatelya = millis();
			}
			else if(_strt_ansver == -1){msgToSend = "generator rabotaet, zapusk ne trebuetsa";}
			else if(_strt_ansver == 0){msgToSend = "generator ne zapushen, trebuetsa vmeshatelstvo operatora";}

			correct = true;
			ComPort.println(F("SMScommand start COMPLETED"));
		}



		else if(_first_simbol == "stop"){  // Исполняем команду STOP >
			ComPort.println(F("SMScommand stop RUN"));  // Выводим в ComPort для контроля, что ничего не потерялось
			int16_t _stp_ansver = stopGeneratorF();

			if(_stp_ansver == 1){msgToSend = "generator ostanovlen";}
			else if(_stp_ansver == -1){msgToSend = "x";}
			else if(_stp_ansver == 0){msgToSend = "generator ne ostanovilsa, trebuetsa vmeshatelstvo operatora";}

			correct = true;
			ComPort.println(F("SMScommand stop COMPLETED"));
		}



		else if(_first_simbol == "rese"){  // Исполняем команду RESET >
			ComPort.println(F("SMScommand reset RUN : sbrosit motochasi"));  // Выводим в ComPort для контроля, что ничего не потерялось
			obshaya_narabotka_v_motochasah = 0;
			obshaya_narabotka_v_motominutah = 0;
			EEPROM.put(OBSHAYA_NARABOTKA_V_MOTOCHASAH_ADDR, obshaya_narabotka_v_motochasah);
			EEPROM.put(OBSHAYA_NARABOTKA_V_MOTOMINUTAH_ADDR, obshaya_narabotka_v_motominutah);


			ComPort.print(F(" obshaya narabotka v motochasah = "));
			ComPort.println(obshaya_narabotka_v_motochasah);
			ComPort.print(F(" obshaya narabotka v motominutah = "));
			ComPort.println(obshaya_narabotka_v_motominutah);

			msgToSend = "leset OK:\nobshaya narabotka h = 0";

			correct = true;
			ComPort.println(F("SMScommand reset COMPLETED"));
		}



		else if(_first_simbol == "set_"){  // переходим к проверке пароля и настройкам set_pasms()()()
			ComPort.println(F("SMScommand setting RUN"));  // Выводим в ComPort для контроля, что ничего не потерялось
			String _pass = (String)_sms_command[4] + (String)_sms_command[5] + (String)_sms_command[6] + (String)_sms_command[7];
			if(_pass == sms_password){
				ComPort.println(F("pass_OK"));

				// количество повторных пусков:                                nsl=(x)
				// время работы стартера                                       tsl=(x)
				// прирост времени работы стартера при повторном пуске         gsl=(x)
				// время глушения двигателя:                                   srt=(x)

				ComPort.print(F("\n nsl = "));
				ComPort.println(number_starter_launch);
				ComPort.print(F(" tsl = "));
				ComPort.println(timeOut_starter_launch);
				ComPort.print(F(" gsl = "));
				ComPort.println(gain_starter_launch);
				ComPort.print(F(" srt = "));
				ComPort.println(stop_rele_time);
				ComPort.println(F("\n"));

				String _set_com = (String)_sms_command[9] + (String)_sms_command[10] + (String)_sms_command[11] + (String)_sms_command[12] + (String)_sms_command[13];
				String _set_val_str = "";
				for(int i=14; i<_sms_command.length(); i++){
					if((String)_sms_command[i] == ")"){break;}
				    _set_val_str += (String)_sms_command[i];
				}

				int _set_val_int = 0;
				for(int i=0; i<_set_val_str.length(); i++){
					_set_val_int *= 10;
				    _set_val_int += ((String)_set_val_str[i]).toInt();               // Получаем первую цифру
				}

				ComPort.print(F("...setting value = "));
				ComPort.println(_set_val_int);

				if(_set_com == "nsl=(" && _set_val_int <= 4){
					number_starter_launch = _set_val_int;
					EEPROM.put(NUMBER_STARTER_LAUNCH_ADDR, number_starter_launch);
				}
				if(_set_com == "tsl=(" && _set_val_int <= 4){
					timeOut_starter_launch = _set_val_int;
					EEPROM.put(TIMEOUT_STARTER_LAUNCH_ADDR, timeOut_starter_launch);
				}
				if(_set_com == "gsl=(" && _set_val_int <= 1){
					gain_starter_launch = _set_val_int;
					EEPROM.put(GAIN_STARTER_LAUNCH_ADDR, gain_starter_launch);
				}
				if(_set_com == "srt=(" && _set_val_int <= 10){
					stop_rele_time = _set_val_int;
					EEPROM.put(STOP_RELE_TIME_ADDR, stop_rele_time);
				}

				ComPort.print(F("\n nsl = "));
				ComPort.println(number_starter_launch);
				ComPort.print(F(" tsl = "));
				ComPort.println(timeOut_starter_launch);
				ComPort.print(F(" gsl = "));
				ComPort.println(gain_starter_launch);
				ComPort.print(F(" srt = "));
				ComPort.println(stop_rele_time);
				ComPort.println(F("\n"));
			}	
			ComPort.println(F("SMScommand setting COMPLETED"));
			correct = true;
		}



		else if(_first_simbol == "time"){  // переходим к настройке таймеров
			ComPort.println(F("SMScommand time RUN"));
			correct = true;
			ComPort.println(F("SMScommand time COMPLETED"));
		}



		else if(_first_simbol == "help"){  // справка
			ComPort.println(F("SMScommand help RUN"));  // Выводим в ComPort для контроля, что ничего не потерялось
			correct = true;
			ComPort.println(F("SMScommand help COMPLETED"));  // Выводим в ComPort для контроля, что ничего не потерялось
		}
	}

	if (!correct) {
		msgToSend = "Incorrect command: " + _sms_command;               // Статус исполнения
	}

	ComPort.print(F("-MSG to send: \" "));                             // Выводим результат в терминал
	ComPort.print(msgToSend);                                  // Выводим результат в терминал
	ComPort.println(F(" \""));                             // Выводим результат в терминал

	ComPort.print(F("-mph: "));                             // Выводим результат в терминал
	ComPort.print(masterPhones);                             // Выводим результат в терминал
	ComPort.print(F("  -ph: "));                             // Выводим результат в терминал
	ComPort.println(_phone);                             // Выводим результат в терминал

	// sendSMS(masterPhones, msgToSend);
	// sendSMS(masterPhones, "test");
	sendSMS(masterPhones, "test2");