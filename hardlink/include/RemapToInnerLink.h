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
bool ReplaceToInnerJunctionAll(wstring directoryName, int& iNumRemapped);

extern void PrintError(DWORD dwErr, LPCTSTR userText, LPCTSTR userText2);


