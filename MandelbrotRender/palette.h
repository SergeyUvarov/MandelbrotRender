#pragma once

#include <stdint.h>
#include "Photo.hpp"

extern uint8_t PALETTE[500][3];

COLORREF getPalette(double n, int32_t p);