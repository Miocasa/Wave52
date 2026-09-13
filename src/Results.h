
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
	FILE_NOT_FOUND_ERROR = -4,
	// widget
	WIDGET_TYPE_ERROR = -10,
	WIDGET_CAST_ERROR = -11,


	SAVE_WIDGETS_ERROR = -12,
	SAVE_BACKGROUND_ERROR = -13,
	LOAD_WIDGETS_ERROR = -14,
	LOAD_BACKGROUND_ERROR = -15,

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


namespace RESULTS_HELPER
{
	constexpr bool is_success(RESULTS r)
	{
		return r == RESULTS::NO_ERROR;
	}

	constexpr bool is_error(RESULTS r)
	{
		return static_cast<int8_t>(r) < 0;
	}

	const char* to_string(RESULTS r);
}

inline const char* RESULTS_HELPER::to_string(RESULTS r)
{
	switch (r)
	{
	case RESULTS::NO_ERROR: return "No error";
	case RESULTS::FILE_OPEN_ERROR: return "File open error";
	case RESULTS::FILE_WRITE_ERROR: return "File write error";
	case RESULTS::FILE_READ_ERROR: return "File read error";
	case RESULTS::FILE_NOT_FOUND_ERROR: return "File not found";
	case RESULTS::WIDGET_TYPE_ERROR: return "Widget type error";
	case RESULTS::WIDGET_CAST_ERROR: return "Widget cast error";
	case RESULTS::INVALID_POINTER_ERROR: return "Invalid pointer";
	default: return "Unknown error";
	}
}
}
