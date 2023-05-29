#include "TestU01.h"

unsigned int shufrand_func(void);

void runtests(void)
{
    // Create TestU01 PRNG object for our generator
    unif01_Gen* gen = unif01_CreateExternGenBits("shufrand", shufrand_func);

    // Run the tests.
    //bbattery_SmallCrush(gen);
	bbattery_Crush(gen);

    // Clean up.
    unif01_DeleteExternGenBits(gen);
}
