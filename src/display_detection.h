#pragma once
#include <config.h>
#include "GxEPD2_4G_4G.h"
// #include "GxEPD2.h"

#if defined(GxEPD2_420_GDEY042T81) || defined(Weact4_2_2bit)
#define _display GxEPD2_420_GDEY042T81
#define _displayBase GxEPD2_4G_4G
#else
#error Display must be defined in config.h
#endif


