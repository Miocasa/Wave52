#pragma once
#include <config.h>
#include "GxEPD2_4G_4G.h"
// #include "GxEPD2.h"


#define ONE_BIT(n) (n | (1 << 7))	// 1000 0000, 0X80 + n
#define TWO_BIT(n) (n | (1 << 6))   // 0100 0000, 0X40 + n
#define TWO_COLOR(n) (n | (1 << 5)) // 0010 0000, 0X20 + n


//! Allowed to use from 0x00 (0) to 0x1F (31), for each display types
enum Display_Types : uint8_t
{
	// 2 bit color displays
	Weact4_2_2bit = TWO_BIT(0),

	// 2 color displays
	Weact4_2_2color = TWO_BIT(0),

	// 1 bit color displays
	Weact4_2 = ONE_BIT(0),
}; // Weact4_2_2color and Weact4_2 not implemented yet, it needs just for example

#ifndef Display
#error Display must be defined in config.h
#endif

#if Display_Types == Weact4_2_2bit
#define _display GxEPD2_420_GDEY042T81
#else
#error display type must be specified
#endif
