
#pragma once
#include <cstdint>

extern "C++" {
/**
 * @brief extendable class to parse errors
 */
enum class RESULTS : int8_t
{
	NO_ERROR = 0,

	// file
	FILE_OPEN_ERROR = -1,
	FILE_WRITE_ERROR = -2,
	FILE_READ_ERROR = -3,
	FILE_NOT_FOUND_ERROR = -3,
	// widget
	WIDGET_TYPE_ERROR = -10,
	WIDGET_CAST_ERROR = -11,

	// data
	INVALID_POINTER_ERROR = -22,

	ERROR_PLACEHOLDER = -12,
};

inline bool operator==(RESULTS lhs, int8_t rhs)
{
	return static_cast<int8_t>(lhs) == rhs;
}

inline bool operator==(int8_t lhs, RESULTS rhs)
{
	return lhs == static_cast<int8_t>(rhs);
}

inline bool operator!=(RESULTS lhs, int8_t rhs)
{
	return !(lhs == rhs);
}

inline bool operator!=(int8_t lhs, RESULTS rhs)
{
	return !(lhs == rhs);
}

inline bool operator<(RESULTS lhs, int8_t rhs)
{
	return static_cast<int8_t>(lhs) < rhs;
}

inline bool operator>(RESULTS lhs, int8_t rhs)
{
	return static_cast<int8_t>(lhs) > rhs;
}

inline bool operator<=(RESULTS lhs, int8_t rhs)
{
	return static_cast<int8_t>(lhs) <= rhs;
}

inline bool operator>=(RESULTS lhs, int8_t rhs)
{
	return static_cast<int8_t>(lhs) >= rhs;
}
}
