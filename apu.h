#ifndef APU_H__
#define APU_H__

#include <stdint.h>
#include "console.h"

#define FRAME_COUNTER_CLOCK 60

APU_t* apu_init();
void apu_free(APU_t* apu);

#endif
