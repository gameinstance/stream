/* Copyright (C) 2024  Bogdan-Gabriel Alecu  (GameInstance.com)
 *
 * stream - C++ streaming components
 *
 * This library is free software: you can redistribute it and/or modify it under the terms of the
 * GNU General Public License as published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without
 * even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along with this program.
 * If not, see <https://www.gnu.org/licenses/>.
 */

#ifndef STREAM_BIT
#define STREAM_BIT

#include <bit>
#include <cstdio>
#include <cstdint>


/*******************************************************************************************************
 *
 * @file  bit.hh
 *
 * @brief A bit streaming component.
 *
 */


namespace stream {
namespace bit {


template<typename INPUT_STREAM>
class input {
public:
	explicit input(INPUT_STREAM &istream);

	inline bool eos();

	inline uint64_t get_uint(size_t bit_count);
	inline uint8_t get_byte();
	inline int64_t get_int(size_t bit_count);

	inline void align();

private:
	INPUT_STREAM &_istream;
	uint8_t _buffer;
	uint8_t _length;

	inline void _get_byte();
};


#if __cplusplus >= 202002L
	using std::countl_zero;
#else
	template <typename INT/*, typename MASK_TYPE = typename remove_const<INT>::type*/>
	inline constexpr int countl_zero(const INT &value);
#endif


/******************************************************************************************************/


template<typename INPUT_STREAM>
input<INPUT_STREAM>::input(INPUT_STREAM &istream)
	: _istream{istream}, _buffer{0}, _length{0}
{
}


template<typename INPUT_STREAM>
inline bool input<INPUT_STREAM>::eos()
{
	if (_length > 0)
		return false;

	return _istream.eos();
}


template<typename INPUT_STREAM>
inline uint64_t input<INPUT_STREAM>::get_uint(size_t bit_count)
{
	auto res = uint64_t{};
	for (;;) {
		if (bit_count == 0)
			break;

		if (_length == 0)
			_get_byte();

		auto n_bits = (_length < bit_count)? _length : bit_count;
		res = res << n_bits | (_buffer >> (_length - n_bits));
		_length -= n_bits;
		_buffer &= (1 << _length) - 1;
		bit_count -= n_bits;
	}

	return res;
}


template<typename INPUT_STREAM>
inline uint8_t input<INPUT_STREAM>::get_byte()
{
	return get_uint(8);
}


template<typename INPUT_STREAM>
inline int64_t input<INPUT_STREAM>::get_int(size_t bit_count)
{
	int32_t uint_data = get_uint(bit_count);
	// 2's complement
	int64_t res = (uint_data << (32 - bit_count)) >> (32 - bit_count);
	return res;
}


template<typename INPUT_STREAM>
inline void input<INPUT_STREAM>::align()
{
	_length -= _length % 8;
}


template<typename INPUT_STREAM>
inline void input<INPUT_STREAM>::_get_byte()
{
	_buffer = static_cast<uint8_t>(_istream.get());
	_length = 8;
}


#if __cplusplus >= 202002L
#else
#include <climits>

template <typename INT/*, typename MASK_TYPE = typename remove_const<INT>::type*/>
inline constexpr int countl_zero(const INT &value)
{
	int bit_size{sizeof(INT) * CHAR_BIT};
	int i{bit_size};
	uint64_t/*MASK_TYPE*/ mask{1u << (bit_size - 1)};
	for (; i && !(value & mask); --i, mask >>= 1) {
	}
	return bit_size - i;
};
#endif


} // namespace bit
} // namespace stream


#endif // STREAM_BIT
