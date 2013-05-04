#ifdef __cplusplus
#include <cstdlib>
#else
#include <stdlib.h>
#endif

#include <stdio.h>
#include "mines.h"

int main ( int argc, char** argv ) {
    Mines game;

    game.OnExecute();

    // all is well ;)
    printf("Exited cleanly\n");
    return 0;
}
