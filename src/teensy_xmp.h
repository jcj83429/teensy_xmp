#ifndef TEENSY_XMP_H
#define TEENSH_XMP_H

#if !defined(__MK20DX256__) && !defined(__MK64FX512__) && !defined(__MK66FX1M0__) && !defined(__IMXRT1052__) && !defined(__IMXRT1062__)
#error	This platform is not supported.
#endif

#include "Arduino.h"
#include "AudioStream.h"
#include "xmp.h"

#define TEENSY_XMP_BUF_SMPS 1024
#define TEENSY_XMP_BUF_BYTES (TEENSY_XMP_BUF_SMPS * 2 * sizeof(int16_t))

enum TeensyXmpState {STOP, PLAY, PAUSE};

class TeensyXmp : public AudioStream
{
public:
    TeensyXmp(void) : AudioStream(0, NULL){
        playState = TeensyXmpState::STOP;
        posMs = 0;
        decodeBuf[0] = decodeBuf[1] = NULL;
        decodeBufSmps[0] = decodeBufSmps[1] = 0;
        playingBuf = 0;
    }

    bool playModuleInMemory(void *buf, int size);
    bool playModuleWithCallbacks(struct xmp_io_callbacks *cb);
    void stop(void);
    bool pause(bool paused);
    bool isPlaying(void){
        return playState != TeensyXmpState::STOP;
    }
    int positionMs(void){
        return posMs;
    }

    void update(void);
    
    int lastErr;
private:
    xmp_context xmpctx;
    TeensyXmpState playState;
    int posMs;
    int16_t *decodeBuf[2];
    // samples remaining in decodeBuf[i], each sample is 4 bytes (int16_t * 2)
    int decodeBufSmps[2];
    int playingBuf;
    
    friend void decodeModule(void);
};

#endif