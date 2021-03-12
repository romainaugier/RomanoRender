#pragma once

#include <stdio.h>
#include <embree3/rtcore.h>

#ifndef EMBREE_UTILS
#define EMBREE_UTILS


struct alignas(16) Vertex { float x, y, z, a; };
struct Triangle { int v0, v1, v2; };


void errorFunction(void* userPtr, enum RTCError error, const char* str);


RTCDevice initializeDevice();


#endif
