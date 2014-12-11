/*
	samp-commands
	Copyright (C) 2014, mrdrifter
	Thx to Gamer_Z, Pamdex, Zeex, SA-MP Team

	This program is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#define SAMPGDK_STATIC
#define SAMPGDK_AMALGAMATION

#include <stdio.h>
#include <stdlib.h>
#include <iomanip>
#include <algorithm>
#include <iostream>
#include <cmath>
#include <vector>
#include <unordered_map>
#include <list>
#include <cstring>
#include <map>
#include <limits>
#include <sstream>
#include <ctime>
#include <unordered_set>
#include <float.h>
#include <cctype>
#include <sstream>
#include <iterator>

#include "amx/amx.h"
#include "plugincommon.h"

#pragma warning( disable : 4018 )

typedef void (*logprintf_t)(char* format, ...);

logprintf_t logprintf;
extern void *pAMXFunctions;

using namespace std;

int ProcessTickTime=0;
int PluginEnabled = 1;
struct structRC {
	string rcFormat;
	int rcIdx;
	AMX* rcAmx;
	bool rcIsFormat;
};
std::map<string, structRC*> registerCommands;

 
std::list<AMX *> toExec;
std::list<AMX *> allAmx;

cell amx_addr[128] = {NULL};

int f_ClearMap(AMX* amx);
int f_GetSizeString(char * const input);
int f_SearchCommands(AMX* amx);
std::vector<std::string> string_split(std::string s, const char delimiter);
std::string f_MakeCallback(std::string cmd);
std::string f_MakeCmd(std::string cmd);
 
#define USENAMETABLE(hdr) \
((hdr)->defsize==sizeof(AMX_FUNCSTUBNT))

#define NUMENTRIES(hdr,field,nextfield) \
(unsigned)(((hdr)->nextfield - (hdr)->field) / (hdr)->defsize) 

#define GETENTRY(hdr,table,index) \
(AMX_FUNCSTUB *)((unsigned char*)(hdr) + (unsigned)(hdr)->table + (unsigned)index*(hdr)->defsize)

#define GETENTRYNAME(hdr,entry) \
(USENAMETABLE(hdr) ? \
(char *)((unsigned char*)(hdr) + (unsigned)((AMX_FUNCSTUBNT*)(entry))->nameofs) : \
((AMX_FUNCSTUB*)(entry))->name)

#define lprintf(n) logprintf(n); printf(n)

#define PVERSION "1.0ß"