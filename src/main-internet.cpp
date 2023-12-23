#include "board_settings.h"

#include "AudioTools.h"
#include "AudioCodecs/CodecMP3Helix.h"

const char *urls[] = {
  "http://icecast-qmusicbe-cdp.triple-it.nl/q-maximum-hits.mp3?dist=vtuner",
  "http://streaming.swisstxt.ch/m/drsvirus/mp3_128",
  "http://stream.srg-ssr.ch/m/rsj/mp3_128",
  "http://stream.srg-ssr.ch/m/drs3/mp3_128",
  "http://stream.srg-ssr.ch/m/rr/mp3_128"
};
const char *wifi = "someNetwork";
const char *password = "somePassword";

URLStream urlStream(wifi, password);
AudioSourceURL source(urlStream, urls, "audio/mp3");
#define OUTPUT_TYPE_SPDIF
#ifdef OUTPUT_TYPE_SPDIF
SPDIFOutput out;
#else
I2SStream out;
#endif
MP3DecoderHelix decoder;
AudioPlayer player(source, out, decoder);

void setup(){
  Serial.begin(115200);
  AudioLogger::instance().begin(Serial, AudioLogger::Info);  

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

  // setup player
  player.begin();
  player.setVolume(0.5);

}

void loop(){
  player.copy();
}
