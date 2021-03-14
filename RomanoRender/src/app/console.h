#pragma once

#include <stdlib.h>
#include <ctype.h>
#include <stdio.h>

#include "imgui.h"

struct Console
{
	char InputBuf[512];
	ImVector<char*> Items;
	ImVector<const char*> Commands;
	ImVector<char*> History;
	int HistoryPos;
	ImGuiTextFilter Filter;
	bool AutoScroll;
	bool ScrollToBottom;


	Console()
	{
		ClearLog();
		memset(InputBuf, 0, sizeof(InputBuf));
		HistoryPos = -1;
		Commands.push_back("HELP");
		Commands.push_back("HISTORY");
		Commands.push_back("CLEAR");
		Commands.push_back("CLASSIFY");
		AutoScroll = true;
		ScrollToBottom = false;
		AddLog("Welcome To Romano Render !");
	}

	~Console()
	{
		ClearLog();
		for (int i = 0; i < History.Size; i++) free(History[i]);
	}

	static int   Stricmp(const char* s1, const char* s2) { int d; while ((d = toupper(*s2) - toupper(*s1)) == 0 && *s1) { s1++; s2++; } return d; }
	static int   Strnicmp(const char* s1, const char* s2, int n) { int d = 0; while (n > 0 && (d = toupper(*s2) - toupper(*s1)) == 0 && *s1) { s1++; s2++; n--; } return d; }
	static char* Strdup(const char* s) { size_t len = strlen(s) + 1; void* buf = malloc(len); IM_ASSERT(buf); return (char*)memcpy(buf, (const void*)s, len); }
	static void  Strtrim(char* s) { char* str_end = s + strlen(s); while (str_end > s && str_end[-1] == ' ') str_end--; *str_end = 0; }

	void ClearLog();
	void AddLog(const char* fmt, ...) IM_FMTARGS(2);
	void ExecCommand(const char* command_line);
	void Draw(const char* title, bool* p_open);
	static int TextEditCallbackStub(ImGuiInputTextCallbackData* data);
	int TextEditCallback(ImGuiInputTextCallbackData* data);
};