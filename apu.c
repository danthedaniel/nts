#include <stdlib.h>
#include "apu.h"

APU_t* apu_init() {
    APU_t* apu = (APU_t*) malloc(sizeof(APU_t));

    return apu;
}

void apu_free(APU_t* apu) {
    free(apu);
}
