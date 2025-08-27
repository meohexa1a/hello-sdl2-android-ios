#include <SDL.h>

#include "narc/core/narc_core.h"

void init()
{
    
}

int main()
{
    NarcCoreApplication::init(Task(init));

    return 0;
}

