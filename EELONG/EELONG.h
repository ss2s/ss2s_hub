#include <EEPROM.h>
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// EEPROM read write
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// float
// чтение
float EEPROM_float_read(int addr) {    
  byte raw[4];
  for(byte i = 0; i < 4; i++) raw[i] = EEPROM.read(addr+i);
  float &num = (float&)raw;
  return num;
}

// запись
void EEPROM_float_write(int addr, float num) {
  byte raw[4];
  (float&)raw = num;
  for(byte i = 0; i < 4; i++) EEPROM.write(addr+i, raw[i]);
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// unsigned long
// чтение
unsigned long EEPROM_ulong_read(int addr) {    
  byte raw[4];
  for(byte i = 0; i < 4; i++) raw[i] = EEPROM.read(addr+i);
  unsigned long &num = (unsigned long&)raw;
  return num;
}

// запись
void EEPROM_ulong_write(int addr, unsigned long num) {
  byte raw[4];
  (unsigned long&)raw = num;
  for(byte i = 0; i < 4; i++) EEPROM.write(addr+i, raw[i]);
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// long
// чтение
long EEPROM_long_read(int addr) {    
  byte raw[4];
  for(byte i = 0; i < 4; i++) raw[i] = EEPROM.read(addr+i);
  long &num = (long&)raw;
  return num;
}

// запись
void EEPROM_long_write(int addr, long num) {
  byte raw[4];
  (long&)raw = num;
  for(byte i = 0; i < 4; i++) EEPROM.write(addr+i, raw[i]);
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// unsigned int
// чтение
unsigned int EEPROM_uint_read(int addr) {    
  byte raw[2];
  for(byte i = 0; i < 2; i++) raw[i] = EEPROM.read(addr+i);
  unsigned int &num = (unsigned int&)raw;
  return num;
}
// запись
void EEPROM_uint_write(int addr, unsigned int num) {
  byte raw[2];
  (unsigned int&)raw = num;
  for(byte i = 0; i < 2; i++) EEPROM.write(addr+i, raw[i]);
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// int
// чтение
int EEPROM_int_read(int addr) {    
  byte raw[2];
  for(byte i = 0; i < 2; i++) raw[i] = EEPROM.read(addr+i);
  int &num = (int&)raw;
  return num;
}

// запись
void EEPROM_int_write(int addr, int num) {
  byte raw[2];
  (int&)raw = num;
  for(byte i = 0; i < 2; i++) EEPROM.write(addr+i, raw[i]);
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// void setup() 
// { 
//   Serial.begin(9600);
//   EEPROM_float_write(0, 1.2568);
//   float a = EEPROM_float_read(0);
//   Serial.println(a, 4);
// }

// void loop(){}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// // с проверкой
// // чтение
// int EEPROM_int_read(int addr) {    
//   byte raw[2];
//   for(byte i = 0; i < 2; i++) raw[i] = EEPROM.read(addr+i);
//   int &num = (int&)raw;
//   return num;
// }

// // запись
// void EEPROM_int_write(int addr, int num) {
//   if (EEPROM_int_read(addr)!= num){//если сохраняемое отличается
//     byte raw[2];
//     (int&)raw = num;
//     for(byte i = 0; i < 2; i++) EEPROM.write(addr+i, raw[i]);
//   }
// }
// ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// raw[i] = EEPROM.read((addr * 4)+i); // Внутри функций
// EEPROM.write((addr * 4) + i, raw[i]); // Обратить внимание на (addr * 4) вместо addr 
// ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// EEPROM ulong READ WRITE F
// unsigned long EEPROM_ulong_read(int fAddr){  // чтение из EEPROM 4 байта unsigned long 
// 	int rAddr = fAddr*4;
// 	byte raw[4];
//   	for(byte i = 0; i < 4; i++){raw[i] = EEPROM.read(rAddr+i);}
//   	unsigned long &data = (unsigned long&)raw;
//   	return data;
// }
// //**********************************************************************************************************************
// void EEPROM_ulong_write(int fAddr, unsigned long data){  // запись в EEPROM 4 байта unsigned long
// 	int rAddr = fAddr*4;
//   	byte raw[4];
//   	(unsigned long&)raw = data;
//   	for(byte i = 0; i < 4; i++){EEPROM.write(rAddr+i, raw[i]);}
// }
// ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////