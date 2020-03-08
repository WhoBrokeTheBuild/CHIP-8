#include "timer.h"

uint8_t DT = 0;
uint8_t ST = 0;

void tick_timer()
{
    if (DT > 0) {
        --DT;
    }

    if (ST > 0) {
        --ST;
    }
}