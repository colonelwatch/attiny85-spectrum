#include "fix_fft.h"                  // 8-bit FFT library modified for Arduino
#include "ssd1306.h"                  // library for OLED
#include "nano_engine.h"              // library for nanoengine (which draws sector-by-sector on OLED because the ATtiny85 can't hold a full buffer)

// To get this program to operate, the SDA and SCL pins must be reassigned to 0 and 2 respectively in the library header file
// The file is located in libraries\ssd1306\src\intf\i2c\ssd1306_i2c_conf.h
// Make sure to undo this if the library will be used again in the future

#define SAMPLING_FREQUENCY 24000  // Sampling frequency (Actual max frequency captured is half)
const unsigned int sampling_period_us = round(1000000 * (2.0 / SAMPLING_FREQUENCY)); // Sampling period (doubled to account for overclock)

int8_t data[64], buff[32];                                     // used to store FFT input/output and past data
unsigned long microseconds;                                    // used for timekeeping
int summ, avg;                                                 // used for DC bias elimination

NanoEngine<TILE_32x32_MONO> engine;                            // declares nanoengine

void setup()
{
  OSCCAL = 240; // Overclocks the MCU to around 30 MHz, set lower if this causes instability, raise if you can/want
  
  ADCSRA &= ~(bit (ADPS0) | bit (ADPS1) | bit (ADPS2));       // clear ADC prescaler bits
  ADCSRA |= bit (ADPS2);                                      // sets ADC clock in excess of 10kHz

  ssd1306_128x64_i2c_init();                                  // initializes OLED
  ssd1306_clearScreen();                                      // clears OLED
  
  engine.begin();                                             // inititalizes nanoengine
};

void loop()
{
  summ = 0;
  for (int i = 0; i < 64; i++) {
    microseconds = micros();
    
    data[i] = (analogRead(A3)) >> 2 - 128;                          // Fitting analogRead data (range:0 - 1023) to int8_t array (range:-128 - 127)
    summ += data[i];
    while (micros() < (microseconds + sampling_period_us)) {        // Timing out uC ADC to fulfill sampling frequency requirement
    }
  }

  // Eliminating remaining DC component (produces usable data in FFT bin #0, which is usually swamped by DC bias)
  avg = summ/64;
  for (int i = 0; i < 64; i++){
    data[i] -= avg;
  }
    
  fix_fftr(data, 6, 0);                             // Performing real FFT

  // Time smoothing by a factor of 0.5 on rise and 0.625 on fall and scaling by factor of 2
  for(int count = 0; count < 32; count++){
  if(data[count] <= 0) data[count] = 0;
  else data[count] *= 2;
  if (data[count] < buff[count]) data[count] = (buff[count] * 4 + (data[count] * 4) >> 3);
  else data[count] = (buff[count] * 5 + (data[count] * 3) >> 3);
  buff[count] = data[count];
  }

  data[0] /= 2;
  
  // Output to SSD1306 using nanoengine canvas from library
  engine.refresh();                                               // Mark entire screen to be refreshed
  engine.canvas.clear();                                          // Clear canvas as previous data
  for(int i = 0; i < 8; i++){
    engine.canvas.drawVLine(i*4,31-(data[i]+1),31);          // Draw to canvas data for lower-leftest sector (FHT bins 0 - 7, lower half)
  }
  engine.canvas.blt(0,32);                                        // Outputs canvas to OLED with an offset (x pixels, y pixels)
  engine.canvas.clear();
  for(int i = 0; i < 8; i++){
    if(abs(data[i]) > 31) engine.canvas.drawVLine(i*4,31-(data[i]-31),31);     // Draw to canvas data for upper-leftest sector (FHT bins 0 - 7, upper half)
  }
  engine.canvas.blt(0,0);
  engine.canvas.clear();
  for(int i = 8; i < 16; i++){
    engine.canvas.drawVLine((i-8)*4,31-(abs(data[i])+1),31);      // FHT bins 8 - 15, lower half
  }
  engine.canvas.blt(32,32);  
  engine.canvas.clear();
  for(int i = 8; i < 16; i++){
    if(abs(data[i]) > 31) engine.canvas.drawVLine((i-8)*4,31-(abs(data[i])-31),31);   // FHT bins 9 - 15, upper half
  }
  engine.canvas.blt(32,0);
  engine.canvas.clear();
  for(int i = 16; i < 24; i++){
    engine.canvas.drawVLine((i-16)*4,31-(abs(data[i])+1),31);     // FHT bins 16 - 23, lower half
  }
  engine.canvas.blt(64,32);
  engine.canvas.clear();
  for(int i = 16; i < 24; i++){
    if(abs(data[i]) > 31) engine.canvas.drawVLine((i-16)*4,31-(abs(data[i])-31),31);  // FHT bins 16 - 23, upper half 
  }
  engine.canvas.blt(64,0);
  engine.canvas.clear();
  for(int i = 24; i < 32; i++){
    engine.canvas.drawVLine((i-24)*4,31-(abs(data[i])+1),31);     // FHT bins 24 - 31, lower half
  }
  engine.canvas.blt(96,32);
  engine.canvas.clear();
  for(int i = 24; i < 32; i++){
    if(abs(data[i]) > 31) engine.canvas.drawVLine((i-24)*4,31-(abs(data[i])-31),31);  // FHT bins 24 - 31, upper half
  }
  engine.canvas.blt(96,0);
}
