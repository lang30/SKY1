#include "pos_io.h"

void SetValveIO(bool AOn) small {
    if (AOn) {
     P26 = 1;
     return;
    }
    P26 = 0;
}