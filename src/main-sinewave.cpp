#include <Arduino.h>

/**
 * @file streams-generator-i2s.ino
 * @author Phil Schatzmann
 * @brief see https://github.com/pschatzmann/arduino-audio-tools/blob/main/examples/examples-stream/streams-generator-spdif/README.md 
 * @author Phil Schatzmann
 * @copyright GPLv3
 * Sinus https://github.com/pschatzmann/arduino-audio-tools/tree/main/examples/examples-stream/streams-generator-spdif
 */


#include "board_settings.h"

#include "AudioConfigLocal-sinewave.h"
#include "AudioTools.h"

typedef int16_t sound_t;                                   // sound will be represented as int16_t (with 2 bytes)
AudioInfo info(44100, 2, 16);
#define SINE_MAX_AMP 12000
#define SINE_NOTE_1 N_C6
#define SINE_NOTE_2 N_A3
int amp = SINE_MAX_AMP;
SineWaveGenerator<sound_t> sineWave(amp);                // subclass of SoundGenerator with max amplitude of 32000
GeneratedSoundStream<sound_t> sound(sineWave);             // Stream generated from sine wave
SPDIFOutput out; 
StreamCopy copier(out, sound);                             // copies sound into i2s

// Arduino Setup
void setup(void) {  
  // Open Serial 
  Serial.begin(115200);
  AudioLogger::instance().begin(Serial, AudioLogger::Warning);

  // start I2S
  Serial.println("starting SPDIF...of SineWave");
  auto config = out.defaultConfig();
  config.copyFrom(info); 
  config.pin_data = SPDIF_DATA_PIN;
  out.begin(config);

  // Setup sine wave
  sineWave.begin(info, SINE_NOTE_1);
  Serial.println("started...");
}

long previousMillis = 0;
long interval = 4000;
float currentNote = SINE_NOTE_1;

// Arduino loop - copy sound to out 
void loop() {
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;
    if (currentNote == SINE_NOTE_1) {
      currentNote = SINE_NOTE_2;
    } else {
      currentNote = SINE_NOTE_1;
    }
    Serial.print("Setting frequency to:"); Serial.println(currentNote);
    sineWave.setFrequency(currentNote);
    /*
    amp = amp - 2000;
    if (amp <=0) {
      amp = SINE_MAX_AMP;
    }
    Serial.print("Setting amp to:"); Serial.println(amp);
    sineWave.setAmplitude(amp);
    */
  }
  copier.copy();
}
