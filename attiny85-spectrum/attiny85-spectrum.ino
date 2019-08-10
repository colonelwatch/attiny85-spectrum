#include "fix_fft.h"                  // 8-bit FFT library modified for Arduino
#include "ssd1306.h"                  // library for OLED
#include "nano_engine.h"              // library for nanoengine (which draws sector-by-sector on OLED because the ATtiny85 can't hold a full buffer)

// To get this program to operate, the SDA and SCL pins must be reassigned to 0 and 2 respectively
// The file is located in libraries\ssd1306\src\intf\i2c\ssd1306_i2c_conf.h
// Make sure to undo this after uploading

#define sampling_period_us 100 // Sampling period (inverse of sampling frequency)

int8_t data[128], buff[64];                                     //variables for the FFT
unsigned long microseconds;           // Used for timekeeping

NanoEngine<TILE_32x32_MONO> engine;

void setup()
{
  OSCCAL = 250;
  
  ADCSRA &= ~(bit (ADPS0) | bit (ADPS1) | bit (ADPS2)); // clear ADC prescaler bits
  ADCSRA |= bit (ADPS2); // sets ADC clock in excess of 10kHz

  ssd1306_128x64_i2c_init();
  ssd1306_clearScreen();
  
  engine.begin();
  engine.setFrameRate(100);
};

void loop()
{
  if (!engine.nextFrame()) return;
  for (int i = 0; i < 128; i++) {
    microseconds = micros();
    
    data[i] = analogRead(A3) >> 2 - 127;                          //each element of array is val/4-128
    while (micros() < (microseconds + sampling_period_us)) {
    }
  }
    
  fix_fftr(data, 7, 0);                            //perform the FFT on data
  /*
  for(int count = 0; count < 64; count++){
  if (data[count] < buff[count]) data[count] = abs((buff[count] + (data[count] * 7)) >> 3);
  else data[count] = abs((buff[count] + (data[count] * 6)) >> 3);
  buff[count] = data[count];
  }
  */
  
  engine.refresh();
  engine.canvas.clear();
  for(int i = 0; i < 16; i++){
    engine.canvas.drawVLine(i*2,31-(abs(data[i])+1),31);
  }
  engine.canvas.blt(0,32);
  engine.canvas.clear();
  for(int i = 0; i < 16; i++){
    if(abs(data[i]) > 31) engine.canvas.drawVLine(i*2,31-(abs(data[i])-31),31);
  }
  engine.canvas.blt(0,0);
  engine.canvas.clear();
  for(int i = 16; i < 32; i++){
    engine.canvas.drawVLine((i-16)*2,31-(abs(data[i])+1),31);
  }
  engine.canvas.blt(32,32);  
  engine.canvas.clear();
  for(int i = 16; i < 32; i++){
    if(abs(data[i]) > 31) engine.canvas.drawVLine((i-16)*2,31-(abs(data[i])-31),31);
  }
  engine.canvas.blt(32,0);
  engine.canvas.clear();
  for(int i = 32; i < 48; i++){
    engine.canvas.drawVLine((i-32)*2,31-(abs(data[i])+1),31);
  }
  engine.canvas.blt(64,32);
  engine.canvas.clear();
  for(int i = 32; i < 48; i++){
    if(abs(data[i]) > 31) engine.canvas.drawVLine((i-32)*2,31-(abs(data[i])-31),31);
  }
  engine.canvas.blt(64,0);
  engine.canvas.clear();
  for(int i = 48; i < 64; i++){
    engine.canvas.drawVLine((i-48)*2,31-(abs(data[i])+1),31);
  }
  engine.canvas.blt(96,32);
  engine.canvas.clear();
  for(int i = 48; i < 64; i++){
    if(abs(data[i]) > 31) engine.canvas.drawVLine((i-48)*2,31-(abs(data[i])-31),31);
  }
  engine.canvas.blt(96,0);
}
