#pragma once
#include "imgui.h"

struct console
{
	char InputBuf[512];
	ImVector<char*> Items;
	ImVector<const char*> Commands;


};