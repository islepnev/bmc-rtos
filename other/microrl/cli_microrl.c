#include <stdio.h>

#include "microrl.h"

// print callback for microrl library
static void print (const char * str)
{
    fprintf (stdout, "%s", str);
}

// execute callback for microrl library
// do what you want here, but don't write to argv!!! read only!!
static int execute (int argc, const char * const * argv)
{
    return 0;
}

void cli_microrl_init(void)
{
    static microrl_t rl;
    static microrl_t * prl = &rl;
    microrl_init (prl, print);
    microrl_set_execute_callback (prl, execute);
}
