#include <Arduino.h>

/**
 * @file streams-generator-i2s.ino
 * @author Phil Schatzmann
 * @brief see https://github.com/pschatzmann/arduino-audio-tools/blob/main/examples/examples-stream/streams-generator-spdif/README.md 
 * @author Phil Schatzmann
 * @copyright GPLv3
 * Sinus https://github.com/pschatzmann/arduino-audio-tools/tree/main/examples/examples-stream/streams-generator-spdif
 */

//#define SINE_WAVE_TEST

#include "board_settings.h"

#ifdef SINE_WAVE_TEST
#include "AudioConfigLocal.h"
#include "AudioTools.h"

typedef int16_t sound_t;                                   // sound will be represented as int16_t (with 2 bytes)
AudioInfo info(44100, 2, 16);
SineWaveGenerator<sound_t> sineWave(32000);                // subclass of SoundGenerator with max amplitude of 32000
GeneratedSoundStream<sound_t> sound(sineWave);             // Stream generated from sine wave
SPDIFOutput out; 
StreamCopy copier(out, sound);                             // copies sound into i2s

// Arduino Setup
void setup(void) {  
  // Open Serial 
  Serial.begin(115200);
  AudioLogger::instance().begin(Serial, AudioLogger::Info);

  // start I2S
  Serial.println("starting SPDIF...of SineWave");
  auto config = out.defaultConfig();
  config.copyFrom(info); 
  config.pin_data = I2S_DOUT;
  out.begin(config);

  // Setup sine wave
  sineWave.begin(info, N_B4);
  Serial.println("started...");
}

// Arduino loop - copy sound to out 
void loop() {
  copier.copy();
}
#else 
// MP3_SD_TEST
#include <SPI.h>
#include <SD.h>
#include "AudioTools.h"
#include "AudioCodecs/CodecMP3Helix.h"

const int chipSelect = SD_CS;
//AudioInfo info(44100, 2, 16);
SPDIFOutput out; 
EncodedAudioStream decoder(&out, new MP3DecoderHelix()); // Decoding stream
StreamCopy copier; 
File audioFile;

void setup(){
  Serial.begin(115200);
  AudioLogger::instance().begin(Serial, AudioLogger::Info);  

  // setup file
  SD.begin(chipSelect);
  audioFile = SD.open("/TheGame.mp3");
  Serial.print("audiofile name: ");
  Serial.println(audioFile.name());

  // start I2S
  Serial.println("starting SPDIF...of SD MP3 /TheGame.mp3");
  auto config = out.defaultConfig();
  //config.copyFrom(info); 
  config.pin_data = I2S_DOUT;
  out.begin(config);

  // setup I2S based on sampling rate provided by decoder
  decoder.setNotifyAudioChange(out);
  decoder.begin();

  // begin copy
  copier.begin(decoder, audioFile);

}

void loop(){
  if (!copier.copy()) {
    stop();
  }
}
#endif