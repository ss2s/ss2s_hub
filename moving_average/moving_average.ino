float voltage_val = 0;
float moving_average[] = {6, 6, 6, 6, 6};
float voltage_sum = 0;
void drawVoltage(byte x = 13, byte y = 0) {
	voltage_sum = 0;
	voltage_val = analogRead(VOLTAGE_PIN);
	voltage_val = voltage_val / VOLTAGE_COEF;
	for(int i=0; i<5; i++){
		if(i<4){
	    	moving_average[i] = moving_average[i+1];
	    	voltage_sum += moving_average[i];
		}
		else{
			moving_average[i] = voltage_val;
			voltage_sum += moving_average[i];
		}
	}
	voltage_val = voltage_sum / 5;
	lcd.setCursor(x, y);
	if(voltage_val < 10){lcd.print(" ");}
	lcd.print(voltage_val, 1);
	lcd.print(" ");
	lcd.setCursor(x+5, y);
	lcd.print("V");
}