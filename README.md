# teensy_xmp
Tracker module player for Teensy 4.x based on libxmp-lite

This is a port of libxmp-lite 4.4.1 for the teensy 4.x boards. 
Due to the nature of tracker modules, all of the pattern and instrument data needs to be loaded into memory.
If the amount of free memory available for malloc is less than the size of the module file, the module will most certainly fail to play.

The current version only supports reading modules from a memory pointer (PROGMEM is ok). Support for reading modules from SD card will be added later.

## Using custom malloc and PSRAM
Due to the high memory requirement of module playback, many modules can't be played with a bare teensy 4.0/4.1.
This library supports using a custom malloc to allocate memory in PSRAM or DTCM.
To use a custom malloc, implement `void *xmp_malloc(size_t size)`, `void xmp_free(void *ptr)` and `void *xmp_realloc(void *ptr, size_t size)`.
See examples/PlayModuleInMemoryWithPSRAM.
