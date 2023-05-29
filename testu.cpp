#include "shufrand.h"
#include <assert.h>
#include <memory.h>
#include <random>

#define USE_REV (0)

inline uint32_t rev32(uint32_t v)
{
    // https://graphics.stanford.edu/~seander/bithacks.html
    // swap odd and even bits
    v = ((v >> 1) & 0x55555555) | ((v & 0x55555555) << 1);
    // swap consecutive pairs
    v = ((v >> 2) & 0x33333333) | ((v & 0x33333333) << 2);
    // swap nibbles ...
    v = ((v >> 4) & 0x0F0F0F0F) | ((v & 0x0F0F0F0F) << 4);
    // swap bytes
    v = ((v >> 8) & 0x00FF00FF) | ((v & 0x00FF00FF) << 8);
    // swap 2-byte-long pairs
    v = ( v >> 16             ) | ( v               << 16);
    return v;
}

static __m128i g_state, g_cur;
static uint32_t g_c;

extern "C" unsigned int shufrand_func(void)
{
	if (!g_c)
		g_cur = shufrand::shufrand_next(g_state);
		
	uint32_t res = ((const uint32_t *)&g_cur)[g_c];

	g_c = (g_c + 1) & 3;

#if USE_REV
	res = rev32(res);
#endif		

	return res;
}

extern "C" void runtests();

int main()
{
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<uint32_t> dist(0, UINT32_MAX);
	g_state = _mm_set_epi32(dist(gen), dist(gen), dist(gen), dist(gen));
	
	for (uint32_t i = 0; i < 4; i++)
		shufrand_func();
		
	runtests();

    return 0;
}