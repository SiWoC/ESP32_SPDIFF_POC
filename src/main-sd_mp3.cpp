#include "board_settings.h"

#include "AudioTools.h"

#include <SPI.h>
#include <SD.h>
#include "AudioTools.h"
#include "AudioCodecs/CodecMP3Helix.h"

AudioInfo info(44100,2,16);
//CsvOutput<int16_t> csvStream(Serial);
//EncodedAudioStream decoder(&csvStream, new MP3DecoderMAD()); // Decoding mp3 to the volume-stream
#define OUTPUT_TYPE_SPDIF
#ifdef OUTPUT_TYPE_SPDIF
SPDIFOutput out;
#else
I2SStream out; // final output of decoded stream
#endif

VolumeStream vol(out); // VolumeStream is doing it's thing "over" the out output stream
//EncodedAudioStream decoder(&vol, new MP3DecoderMAD()); // Decoding mp3 to the volume-stream
EncodedAudioStream decoder(&vol, new MP3DecoderHelix()); // Decoding mp3 to the volume-stream
StreamCopy copier;
File audioFile;

void setup(){
  Serial.begin(115200);
  AudioLogger::instance().begin(Serial, AudioLogger::Info);  

  // Open file from SD
  SD.begin(SD_CS);
  audioFile = SD.open("/Lap.mp3");

  #ifdef OUTPUT_TYPE_SPDIF
  auto config = out.defaultConfig();
  config.pin_data = I2S_DOUT;
  #else
  // setup I2S based on sampling rate provided by decoder
  auto config = out.defaultConfig(TX_MODE);
  config.pin_bck = I2S_BCLK;
  config.pin_ws = I2S_LRC;
  config.pin_data = I2S_DOUT;
  #endif
  out.begin(config);

  // setup volume
  vol.begin(config);
  vol.setVolume(0.5);  // half the volume

  // add volumeStream as subscriber to decoder.AudioChange
  // don't know if/why this is needed
  // decoder.setNotifyAudioChange(vol);
  decoder.begin();

  // begin copy
  copier.begin(decoder, audioFile);

}

void loop(){
  copier.copy();
}
