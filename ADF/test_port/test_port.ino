//  pinMode(pin, OUTPUT);
#define ADF_PMO13 DDRB |= B00100000 // B5
#define ADF_PMO12 DDRB |= B00010000 // B4

//  digitalWrite(pin, HIGH);
#define ADF_DWH13 PORTB |= B00100000 // B5

//  digitalWrite(pin, LOW);
#define ADF_DWL13 PORTB &= B11011111 // B5
#define ADF_DWL12 PORTB &= B11101111 // B4

//  pinMode(pin, INPUT_PULLUP);
#define ADF_PMIP12 DDRB &= B11101111; PORTB |= B00010000 // B4

void setup()
{
	pinMode(13, OUTPUT);
	Serial.begin(9600);
	// ADF_PMO13;
	// ADF_PMIP12;
}
void loop()
{
	while(1){
		digitalWrite(13, HIGH);
	    // ADF_DWH13;

	    delayMicroseconds(50);

		digitalWrite(13, LOW);
	    // ADF_DWL13;

	    delayMicroseconds(50);
	    Serial.println("ss2s");
	}
}




// int main()
// {
// 	ADF_PMO13;
// 	while(1){
// 	    ADF_DWH13;
// 	    ADF_DWL13;
// 	}
// }