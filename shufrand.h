#pragma once

#include <stdlib.h>
#include <stdio.h>
#include <cstdint>

#ifdef _MSC_VER
#include <intrin.h>
#else
#include <x86intrin.h>
#endif

#ifdef _MSC_VER
#define SR_ALIGN(v) __declspec(align(v))
#else
#define SR_ALIGN(v) __attribute__((aligned(v)))
#endif

namespace shufrand
{

extern const SR_ALIGN(16) uint8_t sr_shuffles[256][16];

struct pcg_state4
{
	uint32_t state[4];
};

inline pcg_state4 pcg_rand4x32(pcg_state4& rng_state)
{
	pcg_state4 temp;

	for (uint32_t i = 0; i < 4; i++)
	{
		const uint32_t state = rng_state.state[i];
		rng_state.state[i] = rng_state.state[i] * 747796405u + 2891336453u;

		temp.state[i] = state;
	}

	uint32_t mask = 0;
	for (uint32_t j = 0; j < 16; j++)
		mask |= ((((const uint8_t*)(&temp.state))[j] >> 7) << j);

	mask ^= (mask >> 8);
	mask &= 0xFF;

	pcg_state4 res;

	const uint8_t* pShuf = &sr_shuffles[mask][0];
	for (uint32_t j = 0; j < 16; j++)
		((uint8_t*)&res.state)[j] = ((const uint8_t*)&temp)[pShuf[j]];

	for (uint32_t j = 0; j < 4; j++)
	{
		res.state[j] ^= temp.state[j];

		uint32_t word = res.state[j] * 277803737u;
		res.state[j] = (word >> 22u) ^ word;
	}

	return res;
}

inline __m128i shufrand_next(__m128i& rng_state)
{
	__m128i state = rng_state;
	rng_state = _mm_mullo_epi32(rng_state, _mm_set1_epi32(747796405));
	rng_state = _mm_add_epi32(rng_state, _mm_set1_epi32((int)2891336453));

	__m128i word = state;

	uint32_t mask = _mm_movemask_epi8(word);
	mask ^= (mask >> 8);
	mask &= 0xFF;

	__m128i shuf = _mm_load_si128((const __m128i*)&sr_shuffles[mask][0]);
	word = _mm_shuffle_epi8(word, shuf);

	word = _mm_xor_si128(word, state);

	word = _mm_mullo_epi32(word, _mm_set1_epi32(277803737));

	__m128i sword = _mm_srli_epi32(word, 22);
	word = _mm_xor_si128(sword, word);

	return word;
}

} // namespace shufrand

