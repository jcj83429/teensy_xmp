// This is an example of the most basic usage of TeensyXmp.
// Only a Teensy 4.0/4.1 with an I2S DAC connected is needed to run this example.
// Without using external memory, only small modules can be played.
// Tested on a T4.1 at 24Mhz and 600MHz

#include "ack.h"
#include "glit.h"
#include "teensy_xmp.h"
#include "Audio.h"

// GUItool: begin automatically generated code
AudioSynthWaveformSine   sine1;          //xy=517,349
AudioMixer4              mixer1;         //xy=780,367
AudioMixer4              mixer2;         //xy=780,459
AudioOutputI2S           i2s1;           //xy=1078,362
AudioConnection          patchCord1(sine1, 0, mixer1, 1);
AudioConnection          patchCord2(sine1, 0, mixer2, 1);
AudioConnection          patchCord3(mixer1, 0, i2s1, 0);
AudioConnection          patchCord4(mixer2, 0, i2s1, 1);
// GUItool: end automatically generated code
TeensyXmp teensyXMP;
AudioConnection patchCord101(teensyXMP, 0, mixer1, 0);
AudioConnection patchCord102(teensyXMP, 1, mixer2, 0);

unsigned char *mods[] = {
  ackerlight_1_mod,
  gaspode___glittering_xm,
  
};

unsigned int modSizes[] = {
  ackerlight_1_mod_len,
  gaspode___glittering_xm_len,
  
};

int numTracks = 2;
int currentTrack = 0;
unsigned long lastPrintTime = 0;

void setup() {
  Serial.begin(115200);
  while(!Serial);
  delay(500);
  Serial.println("press Send to skip to the next track");
  lastPrintTime = millis();
  AudioMemory(10);
  // check if DAC is working
  //sine1.frequency(1000);
  //sine1.amplitude(0.1);
}


void loop() {
  if(Serial.available()){
    Serial.read();
    teensyXMP.stop();
  }
  if(millis() - lastPrintTime > 1000){
    Serial.print("pos ");
    Serial.println(teensyXMP.positionMs());
    lastPrintTime += 1000;
  }
  if(!teensyXMP.isPlaying()){
    bool res = teensyXMP.playModuleInMemory(mods[currentTrack], modSizes[currentTrack]);
    Serial.print("playing track ");
    Serial.print(currentTrack);
    Serial.print(", success=");
    Serial.println(res);
    currentTrack = (currentTrack + 1) % numTracks;
  }
}
