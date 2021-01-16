#include "teensy_xmp.h"

// using the same interrupt as the Audio Codecs library
#define IRQ_AUDIO			IRQ_SOFTWARE	// see AudioStream.cpp
#if defined(__IMXRT1052__) && !defined(__IMXRT1062__)
#define IRQ_AUDIOCODEC		IRQ_Reserved1
#else
#define IRQ_AUDIOCODEC		55				// use a "reserved" (free) interrupt vector
#endif

#define IRQ_AUDIOCODEC_PRIO	240				// lowest priority

#define NVIC_STIR			(*(volatile uint32_t *)0xE000EF00) //Software Trigger Interrupt Register
#define NVIC_TRIGGER_INTERRUPT(x)    NVIC_STIR=(x)
#define NVIC_IS_ACTIVE(n)	(*((volatile uint32_t *)0xE000E300 + ((n) >> 5)) & (1 << ((n) & 31)))

void __attribute__((weak)) *xmp_malloc(size_t size){
    return malloc(size);
}

void __attribute__((weak)) xmp_free(void *ptr){
    return free(ptr);
}

void __attribute__((weak)) *xmp_realloc(void *ptr, size_t size){
	return realloc(ptr, size);
}

void *xmp_calloc(size_t n, size_t size){
    void *mem = xmp_malloc(n * size);
    if(mem != NULL){
        memset(mem, 0, n * size);
    }
    return mem;
}

TeensyXmp *playingTeensyXmp = NULL;
void decodeModule(void);

bool TeensyXmp::playModuleInMemory(void *buf, int size){
    if(playState != TeensyXmpState::STOP){
        stop();
    }
    
    decodeBuf[0] = (int16_t*)malloc(sizeof(int16_t) * 2 * TEENSY_XMP_BUF_SMPS);
    if(decodeBuf[0] == NULL){
        Serial.println("playModuleInMemory failed to allocate decodeBuf");
        return false;
    }

    decodeBuf[1] = (int16_t*)malloc(sizeof(int16_t) * 2 * TEENSY_XMP_BUF_SMPS);
    if(decodeBuf[0] == NULL){
        Serial.println("playModuleInMemory failed to allocate decodeBuf");
        goto fail1;
    }
    
    int err;
    xmpctx = xmp_create_context();
    if(xmpctx == NULL){
        Serial.print("xmp_create_context returned null");
        goto fail2;
    }
    err = xmp_load_module_from_memory(xmpctx, buf, size);
    if(err){
        Serial.print("xmp_load_module_from_memory failed with error ");
        Serial.println(err);
        goto fail3;
    }
    err = xmp_start_player(xmpctx, 44100, 0);
    if(err){
        Serial.print("xmp_start_player failed with error ");
        Serial.println(err);
        goto fail4;
    }
    decodeBufSmps[0] = decodeBufSmps[1] = 0;
    playingBuf = 0;
    lastErr = 0;

    playingTeensyXmp = this;
    playState = TeensyXmpState::PLAY;
    decodeModule();
    if(playState != TeensyXmpState::PLAY){
        // something went wrong in decodeModule
        return false;
    }

    attachInterruptVector((IRQ_NUMBER_t)IRQ_AUDIOCODEC, decodeModule);
    if(NVIC_GET_PRIORITY(IRQ_AUDIO) >= IRQ_AUDIOCODEC_PRIO){
        NVIC_SET_PRIORITY(IRQ_AUDIO, IRQ_AUDIOCODEC_PRIO-16);
    }
    NVIC_SET_PRIORITY(IRQ_AUDIOCODEC, IRQ_AUDIOCODEC_PRIO);
    NVIC_ENABLE_IRQ(IRQ_AUDIOCODEC);

    return true;
    
fail4:
    xmp_release_module(xmpctx);
fail3:
    xmp_free_context(xmpctx);
fail2:
    free(decodeBuf[1]);
fail1:
    free(decodeBuf[0]);
    return false;
}

bool TeensyXmp::playModuleWithCallbacks(struct xmp_io_callbacks *cb){
    if(playState != TeensyXmpState::STOP){
        stop();
    }
    
    decodeBuf[0] = (int16_t*)malloc(sizeof(int16_t) * 2 * TEENSY_XMP_BUF_SMPS);
    if(decodeBuf[0] == NULL){
        Serial.println("playModuleInMemory failed to allocate decodeBuf");
        return false;
    }

    decodeBuf[1] = (int16_t*)malloc(sizeof(int16_t) * 2 * TEENSY_XMP_BUF_SMPS);
    if(decodeBuf[0] == NULL){
        Serial.println("playModuleInMemory failed to allocate decodeBuf");
        goto fail1;
    }
    
    int err;
    xmpctx = xmp_create_context();
    if(xmpctx == NULL){
        Serial.print("xmp_create_context returned null");
        goto fail2;
    }
    err = xmp_load_module_from_callbacks(xmpctx, cb);
    if(err){
        Serial.print("xmp_load_module_from_callbacks failed with error ");
        Serial.println(err);
        goto fail3;
    }
    err = xmp_start_player(xmpctx, 44100, 0);
    if(err){
        Serial.print("xmp_start_player failed with error ");
        Serial.println(err);
        goto fail4;
    }
    decodeBufSmps[0] = decodeBufSmps[1] = 0;
    playingBuf = 0;
    lastErr = 0;

    playingTeensyXmp = this;
    playState = TeensyXmpState::PLAY;
    decodeModule();
    if(playState != TeensyXmpState::PLAY){
        // something went wrong in decodeModule
        return false;
    }

    attachInterruptVector((IRQ_NUMBER_t)IRQ_AUDIOCODEC, decodeModule);
    if(NVIC_GET_PRIORITY(IRQ_AUDIO) >= IRQ_AUDIOCODEC_PRIO){
        NVIC_SET_PRIORITY(IRQ_AUDIO, IRQ_AUDIOCODEC_PRIO-16);
    }
    NVIC_SET_PRIORITY(IRQ_AUDIOCODEC, IRQ_AUDIOCODEC_PRIO);
    NVIC_ENABLE_IRQ(IRQ_AUDIOCODEC);

    return true;
    
fail4:
    xmp_release_module(xmpctx);
fail3:
    xmp_free_context(xmpctx);
fail2:
    free(decodeBuf[1]);
fail1:
    free(decodeBuf[0]);
    return false;
}

void TeensyXmp::stop(){
    NVIC_DISABLE_IRQ(IRQ_AUDIOCODEC);
    playingTeensyXmp = NULL;
	// at this point, it is not possible for updateModule to call stop()
	// so we are the only one that can be here.
    if(playState != TeensyXmpState::STOP){
        playState = TeensyXmpState::STOP;
        xmp_end_player(xmpctx);
        xmp_release_module(xmpctx);
        xmp_free_context(xmpctx);
    }
    if(decodeBuf[1]){
        free(decodeBuf[1]);
        decodeBuf[1] = NULL;
    }
    if(decodeBuf[0]){
        free(decodeBuf[0]);
        decodeBuf[0] = NULL;
    }
    posMs = 0;
	lenMs = 0;
}

bool TeensyXmp::pause(bool paused){
	if(playState == TeensyXmpState::STOP){
		return false;
	}
	if(paused){
		playState = TeensyXmpState::PAUSE;
	}else{
		playState = TeensyXmpState::PLAY;
	}
	return true;
}

bool TeensyXmp::seekSec(uint32_t timesec){
	if(!preSeek()){
		return false;
	}
	int newPosIdx = xmp_seek_time(xmpctx, timesec * 1000);
	Serial.print("xmp_seek_time returned ");
	Serial.println(newPosIdx);
	if(newPosIdx < 0){
		return false;
	}
	return postSeek();
}

bool TeensyXmp::seekNextPos(){
	if(!preSeek()){
		return false;
	}
	int newPosIdx = xmp_next_position(xmpctx);
	Serial.print("xmp_next_position returned ");
	Serial.println(newPosIdx);
	if(newPosIdx < 0){
		return false;
	}
	return postSeek();
}

bool TeensyXmp::seekPrevPos(){
	if(!preSeek()){
		return false;
	}
	int newPosIdx = xmp_prev_position(xmpctx);
	Serial.print("xmp_prev_position returned ");
	Serial.println(newPosIdx);
	if(newPosIdx < 0){
		return false;
	}
	return postSeek();
}

bool TeensyXmp::preSeek(){
	if(playState == TeensyXmpState::STOP){
		return false;
	}
	return pause(true);
}

bool TeensyXmp::postSeek(){
	// flush old samples
	decodeBufSmps[0] = decodeBufSmps[1] = 0;
	playingBuf = 0;
	pause(false);
	if (!NVIC_IS_ACTIVE(IRQ_AUDIOCODEC)){
        NVIC_TRIGGER_INTERRUPT(IRQ_AUDIOCODEC);
    }
	if(playState == TeensyXmpState::STOP){
		// something went wrong in decodeModule
		return false;
	}
	return true;
}

void TeensyXmp::update(){
    if(playState != TeensyXmpState::PLAY){
        return;
    }

    // chain the decode interrupt

    if (!NVIC_IS_ACTIVE(IRQ_AUDIOCODEC)){
        NVIC_TRIGGER_INTERRUPT(IRQ_AUDIOCODEC);
    }

    if(!playingTeensyXmp->decodeBufSmps[playingBuf]){
        if(playingTeensyXmp->decodeBufSmps[1 - playingBuf]){
            // other buffer is ready
            playingBuf = 1 - playingBuf;
        }else{
            // buffer underrun!
            return;
        }
    }
    
    int16_t *pb = playingTeensyXmp->decodeBuf[playingBuf] + 2 * (TEENSY_XMP_BUF_SMPS - playingTeensyXmp->decodeBufSmps[playingBuf]);

    audio_block_t	*block_left = allocate();
    if(!block_left){
        return;
    }
    audio_block_t	*block_right = allocate();
    if(!block_right){
        release(block_left);
        return;
    }
    for(int i = 0, j = 0; i < 128; i++){
        block_left->data[i] = pb[j++];
        block_right->data[i] = pb[j++];
    }
    transmit(block_left, 0);
    transmit(block_right, 1);
    release(block_left);
    release(block_right);
    
    playingTeensyXmp->decodeBufSmps[playingBuf] -= 128;
}

void decodeModule(void){
    if(!playingTeensyXmp){
        return;
    }
    if(playingTeensyXmp->playState != TeensyXmpState::PLAY){
        return;
    }
    int decodingBuf = 1 - playingTeensyXmp->playingBuf;
    if(playingTeensyXmp->decodeBufSmps[decodingBuf]){
        // nothing to do. wait until update() switches playingBuf.
        // we never partially fill a decode buffer.
        return;
    }

    int err = xmp_play_buffer(playingTeensyXmp->xmpctx, playingTeensyXmp->decodeBuf[decodingBuf], TEENSY_XMP_BUF_BYTES, 1);
    if(err){
        if(err != -XMP_END){
            Serial.print("xmp_play_buffer returned error ");
            Serial.println(err);
        }
        playingTeensyXmp->lastErr = err;
        playingTeensyXmp->stop();
    }else{
        struct xmp_frame_info fi;
        xmp_get_frame_info(playingTeensyXmp->xmpctx, &fi);
		playingTeensyXmp->lenMs = fi.total_time;
        playingTeensyXmp->posMs = fi.time;
        playingTeensyXmp->decodeBufSmps[decodingBuf] = TEENSY_XMP_BUF_SMPS;
        // now update() may use the new buffer
    }
}