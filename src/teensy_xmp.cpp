#include "teensy_xmp.h"

void __attribute__((weak)) *xmp_malloc(size_t size){
    return malloc(size);
}

void __attribute__((weak)) xmp_free(void *ptr){
    return free(ptr);
}

void *xmp_calloc(size_t n, size_t size){
    void *mem = xmp_malloc(n * size);
    if(mem != NULL){
        memset(mem, 0, n * size);
    }
    return mem;
}

bool TeensyXmp::playModuleInMemory(void *buf, int size){
    if(playState != TeensyXmpState::STOP){
        stop();
    }
    int err;
    xmpctx = xmp_create_context();
    err = xmp_load_module_from_memory(xmpctx, buf, size);
    if(err){
        Serial.print("xmp_load_module_from_memory failed with error ");
        Serial.println(err);
        goto fail1;
    }
    err = xmp_start_player(xmpctx, 44100, 0);
    if(err){
        Serial.print("xmp_start_player failed with error ");
        Serial.println(err);
        goto fail2;
    }
    playState = TeensyXmpState::PLAY;
    return true;
    
fail2:
    xmp_release_module(xmpctx);
fail1:
    xmp_free_context(xmpctx);
    return false;
}

void TeensyXmp::stop(){
    if(playState != TeensyXmpState::STOP){
        playState = TeensyXmpState::STOP;
        xmp_end_player(xmpctx);
        xmp_release_module(xmpctx);
        xmp_free_context(xmpctx);
    }
    posMs = 0;
}

void TeensyXmp::update(){
    if(playState != TeensyXmpState::PLAY){
        return;
    }
    audio_block_t	*block_left = allocate();
    if(!block_left){
        return;
    }
    audio_block_t	*block_right = allocate();
    if(!block_right){
        return;
    }
    
    int16_t playBuf[256]; // 128 samples per channel
    int err = xmp_play_buffer(xmpctx, playBuf, sizeof(playBuf), 1);

    for(int i = 0, j = 0; i < 128; i++){
        block_left->data[i] = playBuf[j++];
        block_right->data[i] = playBuf[j++];
    }
    transmit(block_left, 0);
    transmit(block_right, 1);
    release(block_left);
    release(block_right);

    if(err){
        if(err != -XMP_END){
            Serial.print("xmp_play_buffer returned error ");
            Serial.println(err);
        }
        stop();
    }else{
        struct xmp_frame_info fi;
        xmp_get_frame_info(xmpctx, &fi);
        posMs = fi.time;
    }
}