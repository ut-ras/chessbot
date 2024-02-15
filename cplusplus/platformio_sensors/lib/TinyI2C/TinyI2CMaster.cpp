/* TinyI2C v2.0.1

   David Johnson-Davies - www.technoblogy.com - 5th June 2022
   
   CC BY 4.0
   Licensed under a Creative Commons Attribution 4.0 International license: 
   http://creativecommons.org/licenses/by/4.0/
*/

#include "TinyI2CMaster.h"



TinyI2CMaster::TinyI2CMaster() {
}


/* *********************************************************************************************************************

   Minimal Tiny I2C Routines for most of the original ATmega processors, such as the ATmega328P used in
   the Arduino Uno, ATmega32U4, ATmega2560 used in the Arduino Mega 2560, and the ATmega1284P,
   plus a few unusual ATtiny processors that provide a TWI peripheral: ATtiny48/88.

********************************************************************************************************************* */


// 400kHz clock
uint32_t const F_TWI = 400000L;                                   // Hardware I2C clock in Hz

// Choose for 1MHz clock
//uint32_t const F_TWI = 1000000L;                                // Hardware I2C clock in Hz

uint8_t const TWSR_MTX_DATA_ACK = 0x28;
uint8_t const TWSR_MTX_ADR_ACK = 0x18;
uint8_t const TWSR_MRX_ADR_ACK = 0x40;
uint8_t const TWSR_START = 0x08;
uint8_t const TWSR_REP_START = 0x10;
uint8_t const I2C_READ = 1;
uint8_t const I2C_WRITE = 0;

void TinyI2CMaster::init () {
  digitalWrite(SDA, HIGH);                                        // Pullups on
  digitalWrite(SCL, HIGH);
  TWSR = 0;                                                       // No prescaler
  TWBR = (F_CPU/F_TWI - 16)/2;                                    // Set bit rate factor
}

uint8_t TinyI2CMaster::read (void) {
  if (I2Ccount != 0) I2Ccount--;
  TWCR = 1<<TWINT | 1<<TWEN | ((I2Ccount == 0) ? 0 : (1<<TWEA));
  while (!(TWCR & 1<<TWINT));
  return TWDR;
}

uint8_t TinyI2CMaster::readLast (void) {
  I2Ccount = 0;
  return TinyI2CMaster::read();
}

bool TinyI2CMaster::write (uint8_t data) {
  TWDR = data;
  TWCR = 1<<TWINT | 1 << TWEN;
  while (!(TWCR & 1<<TWINT));
  return (TWSR & 0xF8) == TWSR_MTX_DATA_ACK;
}

// Start transmission by sending address
bool TinyI2CMaster::start (uint8_t address, int32_t readcount) {
  bool read;
  if (readcount == 0) read = 0;                                   // Write
  else { I2Ccount = readcount; read = 1; }                        // Read
  uint8_t addressRW = address<<1 | read;
  TWCR = 1<<TWINT | 1<<TWSTA | 1<<TWEN;                           // Send START condition
  while (!(TWCR & 1<<TWINT));
  if ((TWSR & 0xF8) != TWSR_START && (TWSR & 0xF8) != TWSR_REP_START) return false;
  TWDR = addressRW;                                               // Send device address and direction
  TWCR = 1<<TWINT | 1<<TWEN;
  while (!(TWCR & 1<<TWINT));
  if (addressRW & I2C_READ) return (TWSR & 0xF8) == TWSR_MRX_ADR_ACK;
  else return (TWSR & 0xF8) == TWSR_MTX_ADR_ACK;
}

bool TinyI2CMaster::restart(uint8_t address, int32_t readcount) {
  return TinyI2CMaster::start(address, readcount);
}

void TinyI2CMaster::stop (void) {
  TWCR = 1<<TWINT | 1<<TWEN | 1<<TWSTO;
  while (TWCR & 1<<TWSTO); // wait until stop and bus released
}


// All versions

TinyI2CMaster TinyI2C = TinyI2CMaster();                          // Instantiate a TinyI2C object
