# shufrand
Vectorized (SSE 4.1) non-cryptographic pseudorandom number generator ([PCG](https://en.wikipedia.org/wiki/Permuted_congruential_generator) variant)

This is a SSE 4.1 variant of O’Neill's 32-bit “RXS-M-XS” PCG (Permuted Congruential Generator) PRNG. A C implementation is also included for validation/porting. Unfortunately, `_mm_srlv_epi32()` (variable lane shift right), which is used by the stock scalar PCG implementation, requires AVX-2, not SSE 4.1, so I had to find a vectorized workaround compatible with SSE 4.1 that is still fast. One fast and simple alternative, which is a reversible integer function, is a table-driven byte permutation. This can be implemented using `_mm_movemask_epi8()`, some sort of table lookup based off a function of the returned 16 mask bits, followed by `_mm_shuffle_epi8()`.

This implementation uses 4 different 32-bit LCG's running in parallel, each with different states, so its period is still 2^32. However, the entropy of each independent 32-bit generator is spread between all the lanes in a 128-bit register, because doing so is cheap to do with a single shuffle. So it's a strange low period hybrid between a 32-bit generator, and something larger.

The trickiest part to getting this working (i.e. reliably passing various tests) was computing a suitable permutation table. The table precomputation approach I used has the following constraints (which are probably not optimal, but this seems to work):
- A byte in a lane cannot be permuted against itself (obviously, or it would always result in 0 once XOR'd against the input vector, losing information)
- "Low bytes" (the ones at offset 0 in each of the 4 lanes, i.e. the LSB) cannot be XOR'd into the low bytes of any other another lane. The lowest byte of LCG's have the least amount of entropy (they cycle relatively quickly), so we want to XOR higher entropy bytes into these low bytes. This betters diffuses the entropy we do have across all the lanes.
- For each of the 4 lanes, only 1 low byte from another lane is allowed to be XOR'd into that lane. This is to prevent XOR'ing in too many weak bytes into a lane.

Many other variations on this approach are possible.

This PRNG passes dieharder, TestU01 SmallCrush and Crush. BigCrush is still testing; let's see what happens. I'm doubtful it'll pass BigCrush because at its heart this is still just 4 32-bit generators ganged together with a fancy bitmix across lanes.

## Key Links/References:  
- https://www.pcg-random.org/
- https://www.pcg-random.org/pdf/hmc-cs-2014-0905.pdf
- https://www.flipcode.com/archives/07-15-2002.shtml
- https://jcgt.org/published/0009/03/02/
- https://www.reedbeta.com/blog/hash-functions-for-gpu-rendering/
- http://marc-b-reynolds.github.io/math/2017/10/13/IntegerBijections.html
- https://www.pcg-random.org/posts/how-to-test-with-testu01.html
- https://linux.die.net/man/1/dieharder
- https://github.com/imneme/pcg-c/blob/83252d9c23df9c82ecb42210afed61a7b42402d7/include/pcg_variants.h#L504
- https://en.wikipedia.org/wiki/Probability_box
