/*
 * Copyright (C) 1999 - 2019, Hermann Schinagl, hermann@schinagl.priv.at
 */

#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers

#if defined( _UNICODE ) && ! defined( UNICODE )
#define UNICODE
#endif


//disable warnings on 255 char debug symbols
#pragma warning (disable : 4786)
//disable warnings on extern before template instantiation
#pragma warning (disable : 4231)
#include <vector>
#include <algorithm>
#include <list>
#include <atomic>
#include <map>

using namespace std;

#include <windows.h>
#include <winbase.h>
#include <stdio.h>
#include <tchar.h>
#include <lm.h>
#include <direct.h>
#include <string.h>
#include <shlwapi.h>
#include <sys/stat.h>

#include <ShlObj.h>

#include "..\..\shared\tre-0.8.0\lib\regex.h" 

#include "hardlink_types.h"
#include "..\hardlink\src\mmfobject.h"

#include "multilang.h"

#include "resource.h"

#pragma comment(lib, "wsock32.lib")
