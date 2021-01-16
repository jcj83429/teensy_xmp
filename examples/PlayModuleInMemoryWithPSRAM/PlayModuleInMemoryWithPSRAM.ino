// This is an example using TeensyXmp with external PSRAM.
// A 4.1 with an I2S DAC connected and at least one 8MB PSRAM chip is needed to run this example.
// Larger modules can be played.
// Tested on a T4.1 at 150MHz. 24MHz is not enough for this example.

#include "autumn.h"
#include "fly.h"
#include "kornhol.h"
#include "professional.h"
#include "teensy_xmp.h"
#include "Audio.h"
#include "tlsf.h"

EXTMEM uint8_t psram_heap[8*1024*1024];
tlsf_t psram_alloc;
int psram_used = 0;

void *xmp_malloc(size_t bytes){
  void *ptr = tlsf_malloc(psram_alloc, bytes);
  if(!ptr){
    Serial.print("xmp_malloc failed to allocate ");
    Serial.print(bytes);
    Serial.println("bytes");
  }else{
    psram_used += tlsf_block_size(ptr);
  }
  return ptr;
}

void xmp_free(void *ptr){
  psram_used -= tlsf_block_size(ptr);
  tlsf_free(psram_alloc, ptr);
}

void *xmp_realloc(void *ptr, size_t bytes){
  int old_block_size =  tlsf_block_size(ptr);
  ptr = tlsf_realloc(psram_alloc, ptr, bytes);
  if(!ptr){
    Serial.print("xmp_realloc failed to allocate ");
    Serial.print(bytes);
    Serial.println("bytes");
  }else{
    psram_used += tlsf_block_size(ptr) - old_block_size;
  }
  return ptr;
}

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
  autumn_in_budapest_xm,
  Tourach___Fly_By_Night_it,
  K_ORNHOL_S3M,
  hoffman_and_daytripper___professional_tracker_mod,
  
};

unsigned int modSizes[] = {
  autumn_in_budapest_xm_len,
  Tourach___Fly_By_Night_it_len,
  K_ORNHOL_S3M_len,
  hoffman_and_daytripper___professional_tracker_mod_len,
  
};

int numTracks = 4;
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

  psram_alloc = tlsf_create_with_pool(psram_heap, sizeof(psram_heap));
}


void loop() {
  if(Serial.available()){
    Serial.read();
    teensyXMP.stop();
    Serial.print("stopped, psram_used=");
    Serial.println(psram_used);
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
    Serial.print(res);
    Serial.print(", psram_used=");
    Serial.println(psram_used);
    currentTrack = (currentTrack + 1) % numTracks;
  }
}
