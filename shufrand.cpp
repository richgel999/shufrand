#include "shufrand.h"

namespace shufrand
{
	const SR_ALIGN(16) uint8_t sr_shuffles[256][16] =
	{
		#include "shufrand_tab.inl"
	};

} // namespace shufrand

