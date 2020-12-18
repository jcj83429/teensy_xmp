#ifndef TEENSY_XMP_H
#define TEENSH_XMP_H

#include "Arduino.h"
#include "AudioStream.h"
#include "xmp.h"

enum TeensyXmpState {STOP, PLAY, PAUSE};

class TeensyXmp : public AudioStream
{
public:
    TeensyXmp(void) : AudioStream(0, NULL){
        playState = TeensyXmpState::STOP;
        posMs = 0;
    }

    bool playModuleInMemory(void *buf, int size);
    void stop(void);
    bool isPlaying(void){
        return playState != TeensyXmpState::STOP;
    }
    int positionMs(void){
        return posMs;
    }

    void update(void);
private:
    xmp_context xmpctx;
    TeensyXmpState playState;
    int posMs;
};

#endif