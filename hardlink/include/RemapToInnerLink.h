/*
    Copyright (C) 1999 - 2020, Hermann Schinagl, Hermann.Schinagl@gmx.net
    Extension 2020 by Su Laus @gitlab.com
*/

#pragma once

#define SIZEOF_NUMARRAYELEMENTS(x) (sizeof(x)/sizeof(x[0]))

bool SetReparseInfo(_ArgvPath& ArgvPath);
bool SearchInnerLinkPath(_ArgvPath& ArgvPath, wstring &wInnerLinkPath);
void PrintError(DWORD dwErr, LPCTSTR userText, LPCTSTR userText2);
bool ReplaceToInnerJunction(_ArgvPath& ArgvPath);
bool ReplaceToInnerJunctionAll(wstring directoryName);

long ReplaceToInnerJunctionOld(wchar_t* wzInFolder);
long getDirectoryListing(const wchar_t* wzDirectoryName);

extern void PrintError(DWORD dwErr, LPCTSTR userText, LPCTSTR userText2);


