#pragma once

#include <cstdint>
#include <vector>


namespace qrcodegen {

	/* An appendable sequence of bits (0s and 1s). Mainly used by QrSegment. */
	class BitBuffer final : public std::vector<bool> 
	{
		
	public: BitBuffer(); // Creates an empty bit buffer (length 0).

			/*---- Method ----*/

			// Appends the given number of low-order bits of the given value
			// to this buffer. Requires 0 <= len <= 31 and val < 2^len.
	public: void appendBits(std::uint32_t val, int len);

	};

}
