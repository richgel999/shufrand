#include "shufrand.h"

#include <assert.h>
#include <memory.h>
#include <random>

#define OUTPUT_BINARY_TO_STDOUT (1)

static void fatal(const char* pMsg)
{
	fprintf(stderr, "%s", pMsg);
	exit(EXIT_FAILURE);
}

int main(int argc, char *argv[])
{
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<uint32_t> dist(0, UINT32_MAX);

	__m128i state = _mm_set_epi32(dist(gen), dist(gen), dist(gen), dist(gen));

#if !OUTPUT_BINARY_TO_STDOUT
	printf("Seeds: %X %X %X %X\n", _mm_extract_epi32(state, 0), _mm_extract_epi32(state, 1), _mm_extract_epi32(state, 2), _mm_extract_epi32(state, 3));
#endif
		
	shufrand::pcg_state4 state2;
	state2.state[0] = _mm_extract_epi32(state, 0);
	state2.state[1] = _mm_extract_epi32(state, 1);
	state2.state[2] = _mm_extract_epi32(state, 2);
	state2.state[3] = _mm_extract_epi32(state, 3);
	
	shufrand::shufrand_next(state);
	shufrand::pcg_rand4x32(state2);

	const uint64_t N = OUTPUT_BINARY_TO_STDOUT ? 1024ULL*1024ULL*1024ULL*32ULL : 16;
	for (uint64_t i = 0; i < N; i++)
	{
		__m128i x = shufrand::shufrand_next(state);

		shufrand::pcg_state4 x2 = shufrand::pcg_rand4x32(state2);

		if ((uint32_t)_mm_extract_epi32(x, 0) != x2.state[0])
			fatal("Fail 0\n");

		if ((uint32_t)_mm_extract_epi32(x, 1) != x2.state[1])
			fatal("Fail 1\n");

		if ((uint32_t)_mm_extract_epi32(x, 2) != x2.state[2])
			fatal("Fail 2\n");

		if ((uint32_t)_mm_extract_epi32(x, 3) != x2.state[3])
			fatal("Fail 3\n");

#if OUTPUT_BINARY_TO_STDOUT
		const uint8_t* p = (const uint8_t*)&x;
		for (uint32_t j = 0; j < 16; j++)
			fputc(p[j], stdout);
#else
		printf("%X %X %X %X\n", _mm_extract_epi32(x, 0), _mm_extract_epi32(x, 1), _mm_extract_epi32(x, 2), _mm_extract_epi32(x, 3));
		printf("%X %X %X %X\n", x2.state[0], x2.state[1], x2.state[2], x2.state[3]);
#endif
	}
	
	return EXIT_SUCCESS;
}