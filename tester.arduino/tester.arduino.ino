#include <Wire.h>            // Used to establish serial communication on the I2C bus
#include "SparkFun_TMAG5273_Arduino_Library.h" // Used to send and recieve specific information from our sensor
#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
 #include <avr/power.h> // Required for 16 MHz Adafruit Trinket
#endif


TMAG5273 sensor; // Initialize hall-effect sensor
#define PIN        25
// I2C default address
uint8_t i2cAddress = 0x35;
Adafruit_NeoPixel pixels2(20, PIN, NEO_WRBG + NEO_KHZ800);

void setup() 
{
   pixels2.begin();
     pixels2.fill(pixels2.Color(1, 1, 1, 0), 0,100);
      pixels2.show(); 


  Wire.begin(21, 22);
  // Start serial communication at 115200 baud
  Serial.begin(115200);  

  // Begin example of the magnetic sensor code (and add whitespace for easy reading)
  Serial.println("TMAG5273 Example 1: Basic Readings");
  Serial.println("");

  // If begin is successful (0), then start example
  if(sensor.begin(i2cAddress, Wire) == 1)
  {
    Serial.println("Begin");
  }
  else // Otherwise, infinite loop
  {
    Serial.println("Device failed to setup - Freezing code.");
    while(1); // Runs forever
  }

}


void loop() 
{
   pixels2.fill(pixels2.Color(1, 1, 1, 0), 0,100);
    pixels2.show(); 
  // Checks if mag channels are on - turns on in setup
  if(sensor.getMagneticChannel() != 0) 
  {
    sensor.setTemperatureEn(true);

    float magX = sensor.getXData();
    float magY = sensor.getYData();
    float magZ = sensor.getZData();
    float temp = sensor.getTemp();

    Serial.print("(");
    Serial.print(magX);
    Serial.print(", ");
    Serial.print(magY);
    Serial.print(", ");
    Serial.print(magZ);
    Serial.println(") mT");
    Serial.print(temp);
    Serial.println(" C");
  }
  else
  {
    // If there is an issue, stop the magnetic readings and restart sensor/example
    Serial.println("Mag Channels disabled, stopping..");
    while(1);
  }

  delay(100);
}
