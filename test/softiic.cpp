#include <Arduino.h>
#include <SlowSoftI2CMaster.h>

// TM1650 tm;
SlowSoftI2CMaster si = SlowSoftI2CMaster(4, 5);

void setup() {
  // delay(100);

  Serial.begin(9600);

  pinMode(11, OUTPUT);
  digitalWrite(11, LOW);

  // Wire.setPins(4, 5);
  // Wire.begin();

  // Wire.beginTransmission(0x48);
  // Wire.write(0b01110001);
  // Wire.endTransmission();

  // Wire.beginTransmission(0x68);
  // Wire.write(0xFF);
  // Wire.endTransmission();

  // Wire.beginTransmission(0x6A);
  // Wire.write(0x00);
  // Wire.endTransmission();

  // Wire.beginTransmission(0x6C);
  // Wire.write(0xFF);
  // Wire.endTransmission();

  si.i2c_init();
  si.i2c_start_wait(0x48);
  si.i2c_write(0b01110001);
  si.i2c_stop();

  si.i2c_start_wait(0x68);
  si.i2c_write(0xFF);
  si.i2c_stop();

  si.i2c_start_wait(0x6A);
  si.i2c_write(0x00);
  si.i2c_stop();

  si.i2c_start_wait(0x6C);
  si.i2c_write(0xFF);
  si.i2c_stop();

  si.i2c_start_wait(0x6E);
  si.i2c_write(0x00);
  si.i2c_stop();

  // si.i2c_start_wait(0x48);
  // si.i2c_write(0b01110000);
  // si.i2c_stop();
}

void loop() {
  // put your main code here, to run repeatedly:
  // digitalWrite(11, LOW);
}
