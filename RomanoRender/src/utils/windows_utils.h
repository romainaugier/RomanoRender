#pragma once

#include "Windows.h"


float CalculateCPULoad(unsigned long long idleTicks, unsigned long long totalTicks);
unsigned long long FileTimeToInt64(const FILETIME& ft);
float GetCPULoad();