# shufrand
SSE 4.1 vectorized pseudorandom number generator ([PCG](https://en.wikipedia.org/wiki/Permuted_congruential_generator) variant)

This is a SSE 4.1 variant of the 32-bit “RXS-M-XS” PCG PRNG. Unfortunately,  _mm_srlv_epi32() (variable lane right shift) is AVX-2, not SSE 4.1, so I had to find a workaround compatible with SSE 4.1. One fast alternative is a table driven byte permutation, or _mm_shuffle_epi8(). This PRNG passes dieharder, TestU01 SmallCrush and Crush. (BigCrush is testing, let's see what happens.)



## Links:  
- https://www.pcg-random.org/
- https://www.pcg-random.org/pdf/hmc-cs-2014-0905.pdf
- https://www.flipcode.com/archives/07-15-2002.shtml
- https://jcgt.org/published/0009/03/02/
- https://www.reedbeta.com/blog/hash-functions-for-gpu-rendering/
- http://marc-b-reynolds.github.io/math/2017/10/13/IntegerBijections.html
- https://www.pcg-random.org/posts/how-to-test-with-testu01.html
- https://linux.die.net/man/1/dieharder
- https://github.com/imneme/pcg-c/blob/83252d9c23df9c82ecb42210afed61a7b42402d7/include/pcg_variants.h#L504

