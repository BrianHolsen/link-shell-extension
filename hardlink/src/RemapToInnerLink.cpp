
/*
    Copyright (C) 1999 - 2020, Hermann Schinagl, Hermann.Schinagl@gmx.net
    Extension 2020 by Su Laus @gitlab.com
*/

#include "stdafx.h"

#include "hardlink_types.h"
#include "DbgHelpers.h"

// Include interfaces from hardlink.lib
#include "HardlinkLib.h"

#include <Windows.h>
#include <list>
#include <string>
#include <Shlwapi.h> //StrStrIW() etc.  + Shlwapi.lib!
#include <PathCch.h> //PathCchRemoveBackslash() etc.  + pathcch.lib!

#include "Reparse.h"
#include "RemapToInnerLink.h"

#pragma comment(lib, "pathcch.lib")

#pragma pack(1)

/*-- Define DeviceControl() return datastructure for FSCTL_GET_REPARSE_POINT --
    see also https://docs.microsoft.com/en-us/windows-hardware/drivers/ddi/ntifs/ns-ntifs-_reparse_data_buffer?redirectedfrom=MSDN

*/
/**--- Microsoft definition from   */
/**--
The substitute name is the name the client MUST use to access this file if it requires to follow the symbolic link.
The print name is the user-friendly name the client MUST return to the application if it requests the name of the symbolic link target.
*/
/*------ Defined in reparse.h ---------
#define REPARSE_MOUNTPOINT_HEADER_SIZE   8
#define REPARSE_PATHBUFFER_MAX	(8*MAX_PATH)
typedef struct _REPARSE_DATA_BUFFER { //size (18) / 22 bytes if pack(1) else 24 bytes.
    ULONG  ReparseTag;
    USHORT ReparseDataLength;		// Size, in bytes, of the reparse data in the buffer that DataBuffer points to. Data buffer starts after ReparseDataLength-parameter.
    USHORT Reserved;				// Length, in bytes, of the unparsed portion of the file name pointed to by the FileName member of the associated file object. For more information about the FileName member, see FILE_OBJECT<. This member is only valid for create operations when the I/O fails with STATUS_REPARSE. For all other purposes, such as setting or querying a reparse point for the reparse data, this member is treated as reserved.
    USHORT SubstituteNameOffset;	// Offset, in bytes, of the substitute name string in the PathBuffer array. Note that this offset must be divided by sizeof(WCHAR) to get the array index.
    USHORT SubstituteNameLength;	// Length, in bytes, of the substitute name string. If this string is NULL-terminated, SubstituteNameLength does not include space for the UNICODE_NULL terminator.
    USHORT PrintNameOffset;			// Offset, in bytes, of the print name string in the PathBuffer array. Note that this offset must be divided by sizeof(WCHAR) to get the array index.
    USHORT PrintNameLength;			// Length, in bytes, of the print name string. If this string is NULL-terminated, PrintNameLength does not include space for the UNICODE_NULL terminator.
    union {
        struct {
            ULONG  Flags;			// Indicates whether the symbolic link is absolute or relative. If Flags contains SYMLINK_FLAG_VALUE_RELATIVE, the symbolic link contained in the PathBuffer array (at offset SubstituteNameOffset) is processed as a relative symbolic link; otherwise, it is processed as an absolute symbolic link.
            WCHAR  PathBuffer[REPARSE_PATHBUFFER_MAX];	// First character of the path string. This character is followed in memory by the remainder of the string. The path string contains the substitute name string and print name string. The substitute name and print name strings can appear in any order in the PathBuffer. To locate the substitute name and print name strings in the PathBuffer, use the SubstituteNameOffset, SubstituteNameLength, PrintNameOffset, and PrintNameLength members.
        } SymbolicLinkPathBuffer;
        struct {
            WCHAR  PathBuffer[REPARSE_PATHBUFFER_MAX];
        } MountPointPathBuffer;
    } DUMMYUNIONNAME;
} REPARSE_DATA_BUFFER, * PREPARSE_DATA_BUFFER;
-----------*/


#define SYMLINK_FLAG_VALUE_RELATIVE 0x00000001

#define MAXFILENAMELENGTH HUGE_PATH


void PrintError(DWORD dwErr, LPCTSTR userText, LPCTSTR userText2)
{
  TCHAR szMsg[1024];
  DWORD dwFlags = FORMAT_MESSAGE_IGNORE_INSERTS |
                  FORMAT_MESSAGE_MAX_WIDTH_MASK |
                  FORMAT_MESSAGE_FROM_SYSTEM;

  if (!::FormatMessage(dwFlags, NULL, dwErr, 0, szMsg, SIZEOF_NUMARRAYELEMENTS(szMsg), NULL))
    _tcscpy(szMsg, TEXT("Unknown error."));
  if (dwErr != 0) {
    fwprintf(stdout, TEXT("%s %s: %s"), userText, userText2, szMsg);
  } else {
    fwprintf(stdout, TEXT("%s %s"), userText, userText2);
  }
} //-- PrintError() --


/*******************************************************************************
** Name        : SetReparseInfo()
**
** Purpose     : Initializes/ sets additional ReparseInfo-properties of the _ArgvPath-Object for .Argv :
**                 ReparseTag; blnIsRelativeSymlink;
** Prerequisite: ArgvPath.Argv is a fully qualified path and the ArgvPath.FileAttribute is alerady set.
**
** Return      : true=success; false=something went wrong;
**
*******************************************************************************/
bool SetReparseInfo(_ArgvPath& ArgvPath)
{
    long      rc;
    wchar_t** lpwzFilePart = {NULL};
    DWORD     attrib;
    bool      blnInvalidHandle = false;
    DWORD     dwRet;

    BYTE                      reparseBuffer[MAX_REPARSE_SIZE];
    PREPARSE_GUID_DATA_BUFFER reparseInfo     = (PREPARSE_GUID_DATA_BUFFER)reparseBuffer;
    PREPARSE_DATA_BUFFER      ReparseBufferMS = (PREPARSE_DATA_BUFFER)reparseBuffer;

    wchar_t wzTempPath[HUGE_PATH];

    attrib = ArgvPath.FileAttribute;
    if (attrib == INVALID_FILE_ATTRIBUTES || attrib == 0) {
        PrintError(GetLastError(), TEXT("\nFileAttribute not set for \" "), ArgvPath.Argv.c_str());
        return false;
    }

    /* open file or directory to access */
    HANDLE hDir = ::CreateFile(ArgvPath.Argv.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING,
        FILE_FLAG_OPEN_REPARSE_POINT | FILE_FLAG_BACKUP_SEMANTICS, NULL);
    if (hDir == INVALID_HANDLE_VALUE) {
        PrintError(GetLastError(), TEXT("\nSetReparseInfo()-CreateFile() Error from "), ArgvPath.Argv.c_str());
        return false;
    }

    blnInvalidHandle = (hDir == INVALID_HANDLE_VALUE);
    if (!blnInvalidHandle) {
        /*-- Retrieve information from resparse points --
         *	Be aware, that hard-links are NOT resparse points in this case below
         *	but only symbolic-links for files and directories as well as junctions.
        */
        memset(reparseBuffer, 0, sizeof(reparseBuffer)); //reset ReparseBuffer data
        rc = ::DeviceIoControl(hDir, FSCTL_GET_REPARSE_POINT, NULL, 0, ReparseBufferMS,
            sizeof(reparseBuffer), &dwRet, NULL);
        /*-- GetLastError() : 4390 = Is no reparse-point!*/
        if (rc == 0 && GetLastError() != 4390) {
            PrintError(GetLastError(), TEXT("\nDeviceIoControl() Error:"), TEXT(""));
            return false;
        } else if (rc > 0) {
            ArgvPath.ReparseTag           = ReparseBufferMS->ReparseTag;
            bool blnIsReparseMntPoint     = (ReparseBufferMS->ReparseTag == (DWORD)IO_REPARSE_TAG_MOUNT_POINT);
            bool blnIsSymlink             = (ReparseBufferMS->ReparseTag == (DWORD)IO_REPARSE_TAG_SYMLINK);
            bool blnIsReparseTagMicrosoft = IsReparseTagMicrosoft(ReparseBufferMS->ReparseTag);
            if (blnIsSymlink) {
                /*-- Symlink target has four bytes as flags */
                ArgvPath.blnIsRelativeSymlink = (ReparseBufferMS->SymbolicLinkReparseBuffer.Reserved == SYMLINK_FLAG_VALUE_RELATIVE);
                if (ReparseBufferMS->SymbolicLinkReparseBuffer.PrintNameLength > 0) {
                    wcsncpy_s(wzTempPath, ReparseBufferMS->SymbolicLinkReparseBuffer.PathBuffer + (ReparseBufferMS->SymbolicLinkReparseBuffer.PrintNameOffset / sizeof(wchar_t)), (ReparseBufferMS->SymbolicLinkReparseBuffer.PrintNameLength / sizeof(wchar_t)));
                } else {
                    wcsncpy_s(wzTempPath, ReparseBufferMS->SymbolicLinkReparseBuffer.PathBuffer + (ReparseBufferMS->SymbolicLinkReparseBuffer.SubstituteNameOffset / sizeof(wchar_t)), (ReparseBufferMS->SymbolicLinkReparseBuffer.SubstituteNameLength / sizeof(wchar_t)));
                }
            } else {
                if (ReparseBufferMS->JunctionReparseBuffer.PrintNameLength > 0) {
                    wcsncpy_s(wzTempPath, ReparseBufferMS->JunctionReparseBuffer.PathBuffer + (ReparseBufferMS->JunctionReparseBuffer.PrintNameOffset / sizeof(wchar_t)), (ReparseBufferMS->JunctionReparseBuffer.PrintNameLength / sizeof(wchar_t)));
                } else {
                    wcsncpy_s(wzTempPath, ReparseBufferMS->JunctionReparseBuffer.PathBuffer + (ReparseBufferMS->JunctionReparseBuffer.SubstituteNameOffset / sizeof(wchar_t)), (ReparseBufferMS->JunctionReparseBuffer.SubstituteNameLength / sizeof(wchar_t)));
                }
            }
            ArgvPath.ArgvReparseTarget = wzTempPath;
        }
        CloseHandle(hDir);
    }

    return true;
}  //-- SetReparseInfo() --

/*******************************************************************************
** Name        : SetArgvPathProperties() 
**               -- copied from ln.cpp CheckArgument() and adapted --
**
** Purpose     : Initializes/ sets properties of the _ArgvPath-Object for .Argv :
**                 DriveType; FileAttribute; Flags;
**                 ReparseTag; blnIsRelativeSymlink;
**
** Return      : true=success; false=something went wrong;
**
*******************************************************************************/
bool SetArgvPathProperties(
    _ArgvPath* aArgvPath)
{
    DWORD attrib    = 0;
    bool  blnRetVal = false;
    if (aArgvPath->Argv.length()) {
        // Resolve a local UNC
        wchar_t  PathTmp[HUGE_PATH];
        wchar_t* pUncResolved = ResolveUNCPath(aArgvPath->Argv.c_str(), PathTmp);
        if (!pUncResolved)
            wcscpy_s(PathTmp, HUGE_PATH, aArgvPath->Argv.c_str());

        wchar_t* pArgvFilePart;
        wchar_t  ArgvPath[HUGE_PATH];

        // See if path is still UNC
        if (!PathIsUNC(PathTmp)) {
            // vshadow creates UNC names which are mounted into the Globalroot
            // e.g.: \\?\GLOBALROOT\Device\HarddiskVolumeShadowCopy4\tmp
            if (!IsVeryLongPath(PathTmp)) {
                // we only need to prefix \\?\ if it not already there
                wcscpy_s(ArgvPath, HUGE_PATH, PATH_PARSE_SWITCHOFF);
                GetFullPathName(PathTmp, HUGE_PATH - PATH_PARSE_SWITCHOFF_SIZE, &ArgvPath[PATH_PARSE_SWITCHOFF_SIZE], &pArgvFilePart);
            } else {
                // But there are situations e.g with vshadow, where \\?\ is already there.
                //
                // For testing: Persistent Snapshots are created by
                //     %VSHADOW% -p x:
                // They are deleted by
                //     %VSHADOW% -ds={SNAPSHOT ID}
                // One can map the snapshot onto a share
                //     %VSHADOW% -er={SNAPSHOT ID},sharename
                //     vshadow64 -er={c6107cb4-fdea-4470-8309-fe46af7ca2ee},2013-05-04
                GetFullPathName(PathTmp, HUGE_PATH, ArgvPath, &pArgvFilePart);
            }
        } else {
            // The path might still be UNC if it was not a local UNC path, which could not be resolved
            GetFullPathName(PathTmp, HUGE_PATH, ArgvPath, &pArgvFilePart);
        }

        //-- Check if NTFS and set DriveType property --
        bool b = CheckFileSystemType(ArgvPath, &aArgvPath->DriveType);

        // Prepare the path and get the attribute
        // By the end we should have a path which has no traling slash, except for x:\ 
        if (!wcsncmp(ArgvPath, PATH_LONG_GLOBALROOT, PATH_LONG_GLOBALROOT_SIZE)) {
        // It is \\?\ we need the traling slash to get proper attributes
        size_t Len = wcslen(ArgvPath);
        if (ArgvPath[Len - 1] != '\\')
            wcscat_s(ArgvPath, HUGE_PATH, L"\\");

        attrib = GetFileAttributes(ArgvPath);
        if (attrib == INVALID_FILE_ATTRIBUTES) {
            PrintError(GetLastError(), TEXT("SetArgvPathProperties() Error from "), ArgvPath);
        } else {
            aArgvPath->FileAttribute = attrib;
            blnRetVal                = true;
        }

        // But afterwards cut off the trailing slash. Do it on our own, because
        // the PathXXX from shlwapi.dll function works differently on all plattforms
        Len = wcslen(ArgvPath);
        if (ArgvPath[Len - 1] == '\\')
            ArgvPath[Len - 1] = 0x00;
    }
        else
        {
            // Do not chop of the traling \ if it is \\?Volume{
            if (wcsncmp(&ArgvPath[PATH_PARSE_SWITCHOFF_SIZE], PATH_VOLUME_GUID, PATH_VOLUME_GUID_SIZE)) {
                // This is either x:\ or x:\bla\foo\ 
              if (!PathIsRoot(&ArgvPath[PATH_PARSE_SWITCHOFF_SIZE]))
                {
                    // Remove a trailing slash. Do it on our own, because the PathXXX
                    // from shlwapi.dll function works differently on all plattforms
                    size_t Len = wcslen(ArgvPath);
                    if (ArgvPath[Len - 1] == '\\')
                        ArgvPath[Len - 1] = 0x00;
                }
                else
                {
                    // \\?\Volume
                }
            }
            
            attrib = GetFileAttributes(ArgvPath);
            if (attrib == INVALID_FILE_ATTRIBUTES) {
                PrintError(GetLastError(), TEXT("SetArgvPathProperties() Error from "), ArgvPath);
            } else {
                aArgvPath->FileAttribute = attrib;
                blnRetVal = true;
            }
        }

        aArgvPath->Argv = &ArgvPath[PATH_PARSE_SWITCHOFF_SIZE];
        blnRetVal  = SetReparseInfo(*aArgvPath);
    }

    return blnRetVal;
} //-- SetArgvPathProperties() --

/*******************************************************************************
** Name        : SplitToPathParts()
**
** Purpose     : Splits a File or Directory path into its parts.
**               If backslashes are available, the parts have trailing backslashes.
**
** Return      : number of parts  or -1 if an error occurs.
**               wzOutPath:        a buffer to hold the individual parts separated by zero-characters.
**               arrpOutPathParts: an array of pointers pointing into wzOutPath buffer to the
**                                 individual parts of the path.
**
*******************************************************************************/
int SplitToPathParts(const wchar_t* wzInPath, wchar_t* wzOutPath, int lenOutPath, wchar_t* arrpOutPathParts[], int lenOutPathParts)
{
    int      i, nParts = 0;
    long     len, lenInPath;
    long     rc;
    wchar_t* wzLocInPath;
    wchar_t* wzLocOutPath;
    wchar_t* pos;

    wzLocInPath = (wchar_t*)wzInPath;
    //PathCchRemoveBackslash(wzLocInPath, lenOutPath); //debugging
    //PathAddBackslashW(wzLocInPath); //debugging
    lenInPath = static_cast<long>(wcslen(wzInPath));
    /* first make copy of path to work with */
    rc                  = wcscpy_s(wzOutPath, lenOutPath, wzInPath);
    arrpOutPathParts[0] = wzOutPath;
    wzLocOutPath        = wzOutPath;

    /* replace \ in paht with \0 and save pointer to parts within arrayOutPathParts */
    for (i = 0; i < lenOutPathParts; i++) {
        len = static_cast<long>((wzLocInPath - wzInPath)); //debugging
        if (wzLocInPath - wzInPath >= lenInPath)
            break; // if end of inPath is reached.
        pos = wcsstr(wzLocInPath, L"\\");
        if (pos != NULL) {
            len = static_cast<long>(pos - wzLocInPath + 1); // inclusive backslash
            // check for buffer overrun
            if ((wzLocOutPath + len + 1 - wzOutPath) >= lenOutPath) {
                PrintError(0, TEXT("SplitToPathParts()-OutPathLength is too small"), TEXT(""));
                return (-1);
            }
            // copy part into output buffer and increment pointers
            wcsncpy(wzLocOutPath, wzLocInPath, len); //does not insert a trailing zero
            wzLocOutPath[len]   = L'\0';
            arrpOutPathParts[i] = wzLocOutPath;
            wzLocInPath         = pos + 1;
            wzLocOutPath += len + 1;
            nParts++;
        } else {
            /* handle last part, if there is no backslash */
            len = static_cast<long>(wcslen(wzLocInPath));
            if (len > 0) {
                wcscpy(wzLocOutPath, wzLocInPath);
                arrpOutPathParts[i] = wzLocOutPath;
                nParts++;
            }
            break;
        }
    } //-- for --
    return nParts;
} //--- SplitToPathParts() ---

/*******************************************************************************
** Name        : CompareDirContents()
**
** Purpose     : Compares contents (only filenames) of two directories.
**               If both pathes are invalid, the two directories are assumed as identical.
**
** Return      : True, if contents of both directories are the same or both pathes are invalid.
**               False, if contents is different.
**
*******************************************************************************/
bool CompareDirContents(const wchar_t* wzBasePath, const wchar_t* wzTestPath)
{
    bool   blnRet;
    bool   blnRetCode = true;
    bool   blnBaseEmpty, blnTestEmpty;
    HANDLE hFindBase;
    WIN32_FIND_DATA fdataBase;
    wchar_t wzBaseSearch[MAXFILENAMELENGTH];
    wchar_t wzTestSearch[MAXFILENAMELENGTH];

    /*-- first check, if directories are not empty --*/
    /* also invalid pathes or non-directory pathes are not empty */
    blnBaseEmpty = PathIsDirectoryEmptyW(wzBasePath);
    blnTestEmpty = PathIsDirectoryEmptyW(wzTestPath);
    if (blnBaseEmpty && blnTestEmpty)
        return true;
    else if (blnBaseEmpty || blnTestEmpty)
        return false;

    wcscpy_s(wzBaseSearch, wzBasePath);
    blnRet = PathCchAppend(wzBaseSearch, SIZEOF_NUMARRAYELEMENTS(wzBaseSearch), TEXT("*.*"));

    if ((hFindBase = FindFirstFileW(wzBaseSearch, &fdataBase)) != INVALID_HANDLE_VALUE) {
        do {
            /* build same file/folder name on test path and test of existance */
            wcscpy_s(wzTestSearch, wzTestPath);
            PathCchAppend(wzTestSearch, SIZEOF_NUMARRAYELEMENTS(wzTestSearch), fdataBase.cFileName);
            if (!PathFileExistsW(wzTestSearch)) {
                blnRetCode = false;
                break;
            }
        } while (FindNextFile(hFindBase, &fdataBase) != 0);
        FindClose(hFindBase);
    }
    return blnRetCode;
}  //--CompareDirContents() --

/*******************************************************************************
** Name        : SearchInnerLinkPath()
**
** Purpose     : Searches around the Junction given in ArgvPath.Argv for real directories,
**               which fit the junction target directory.
**
** Return      : True = wInnerLinkPath was found; False = nothing fitting found.
**               wInnerLinkPath as the first found fitting "inner link" directory, which fits the junction.
**
*******************************************************************************/
bool SearchInnerLinkPath(_ArgvPath& ArgvPath, wstring& wInnerLinkPath)
{
#define MAXARRAY 200
    bool     blnRetCode = false;
    BOOL     blnRet;
    int      i, j, k;
    int      nPartsLinkPath, nPartsTargetPath;
    DWORD    attrib;
    wchar_t* wzPtr;
    wchar_t* arrpLinkPathParts[MAXARRAY];   //pointer array into wzReparseLinkPath
    wchar_t* arrpTargetPathParts[MAXARRAY]; //pointer array into wzReparseTargetPath
    wchar_t  wzReparseLinkPath[MAXFILENAMELENGTH];
    wchar_t  wzReparseTargetPath[MAXFILENAMELENGTH];
    wchar_t  wzStartPath[MAXFILENAMELENGTH];
    wchar_t  wzTestPath[MAXFILENAMELENGTH];

    /*-- Copies reparseInfo path into wzReparseXXPath and sets array of path-parts-pointers (arrpXXPathParts).
     *   Parts do have trailing backslashes.
    --*/
    nPartsLinkPath   = SplitToPathParts(ArgvPath.Argv.c_str(), wzReparseLinkPath, SIZEOF_NUMARRAYELEMENTS(wzReparseLinkPath), arrpLinkPathParts, MAXARRAY);
    nPartsTargetPath = SplitToPathParts(ArgvPath.ArgvReparseTarget.c_str(), wzReparseTargetPath, SIZEOF_NUMARRAYELEMENTS(wzReparseTargetPath), arrpTargetPathParts, MAXARRAY);


    /*--- first search from junction/link downwards for matching inner path ---*/
    /* build start path for inner path combinations */
    wzStartPath[0] = L'\0';
    for (k = 0; k <= (nPartsLinkPath - 2); k++)
        wcscat_s(wzStartPath, arrpLinkPathParts[k]);
    for (i = nPartsTargetPath - 1; i >= 0; i--) {
        /* build test path */
        wcscpy_s(wzTestPath, wzStartPath);
        for (k = i; k < nPartsTargetPath; k++)
            wcscat_s(wzTestPath, arrpTargetPathParts[k]);
        /* check, if file or directory exists */
        if (PathFileExistsW(wzTestPath)) {
            /*-- check, if it is the reparse point to be changed --*/
            wzPtr = StrStrIW(ArgvPath.Argv.c_str(), wzTestPath);
            if (wzPtr != NULL)
                continue;
            /*-- Test for reparse point. Do not allow re-linking to reparse points. --*/
            attrib = GetFileAttributesW(wzTestPath);
            if (attrib == INVALID_FILE_ATTRIBUTES) {
                PrintError(GetLastError(), TEXT("SearchInnerLinkPath() - Problem getting file-attributes from \" "), wzTestPath);
            }
            if ((attrib & FILE_ATTRIBUTE_REPARSE_POINT) != 0)
                continue;
            /*-- Test for same contents if it is a directory --*/
            if ((ArgvPath.FileAttribute & FILE_ATTRIBUTE_DIRECTORY) != 0) {
                blnRet = CompareDirContents(ArgvPath.Argv.c_str(), wzTestPath);
                if (!blnRet)
                    continue; //continue, if contents (filename compare only) is not identical. Broken links are compared as identical.
            }
            /* set inner link and return */
            wInnerLinkPath = wzTestPath;
            return true;
        }
    } //-- for(i) --

    /*--- now search through inner path combinations upwards ---*/
    for (j = nPartsLinkPath - 2; j >= 0; j--) {
        /* build start path for inner path combinations */
        wzStartPath[0] = L'\0';
        for (k = 0; k <= j; k++)
            wcscat_s(wzStartPath, arrpLinkPathParts[k]);
        for (i = nPartsTargetPath - 1; i >= 0; i--) {
            /* build test path */
            wcscpy_s(wzTestPath, wzStartPath);
            for (k = i; k < nPartsTargetPath; k++)
                wcscat_s(wzTestPath, arrpTargetPathParts[k]);
            /* check, if file or directory exists */
            if (PathFileExistsW(wzTestPath)) {
                /*-- check, if it is the reparse point to be changed --*/
                wzPtr = StrStrIW(wzTestPath, ArgvPath.Argv.c_str());
                if (wzPtr != NULL)
                    continue;
                /*-- Test for reparse point. Do not allow re-linking to reparse points. --*/
                attrib = GetFileAttributesW(wzTestPath);
                if (attrib == INVALID_FILE_ATTRIBUTES) {
                    PrintError(GetLastError(), TEXT("SearchInnerLinkPath() - Problem getting file-attributes from \" "), wzTestPath);
                }
                if ((attrib & FILE_ATTRIBUTE_REPARSE_POINT) != 0)
                    continue;
                /*-- Test for same contents if it is a directory --*/
                if ((ArgvPath.FileAttribute & FILE_ATTRIBUTE_DIRECTORY) != 0) {
                    blnRet = CompareDirContents(ArgvPath.Argv.c_str(), wzTestPath);
                    if (!blnRet)
                        continue; //continue, if contents (filename compare only) is not identical.
                }
                /* set inner link and return */
                wInnerLinkPath = wzTestPath;
                return true;
            }
        } //-- for(i) --
    }     //-- for(j) --

    return blnRetCode;
} //--- SearchInnerLinkPath() ---

/*******************************************************************************
** Name        : ReplaceToInnerJunction()
**
** Purpose     : Remaps a junction to its next fitting directory, which is not a reparse point 
** Prerequisite: ArgvPath properties and .ArgvReparseTarget have to be set before calling ReplaceToInnerJunction()
**
** Return      : true=success; false=something went wrong;
**
*******************************************************************************/
bool ReplaceToInnerJunction(_ArgvPath& ArgvPath)
{
    bool    blnRet = false;
    int     intRet = 0;
    PWSTR   wzPtr  = 0;
    wstring wInnerLinkPath;

    if (PathIsUNC(ArgvPath.Argv.c_str()))
        ArgvPath.Argv += L"\\";

    if (((ArgvPath.FileAttribute & FILE_ATTRIBUTE_REPARSE_POINT) != 0) && !ArgvPath.blnIsRelativeSymlink) {
        // ArgvPath.ArgvReparseTarget has to be set before calling SearchInnerLinkPath()
        blnRet = SearchInnerLinkPath(ArgvPath, wInnerLinkPath);
        if (blnRet) {
            /*-- check, if it is not already the inner link path --*/
            // cope with \\?\ prepended paths, so compare twice
            blnRet = false;
            wzPtr  = StrStrIW(ArgvPath.ArgvReparseTarget.c_str(), wInnerLinkPath.c_str());
            if (wzPtr == NULL)
                wzPtr = StrStrIW(wInnerLinkPath.c_str(), ArgvPath.ArgvReparseTarget.c_str());
            if (wzPtr == NULL) {
                /*-- set new target to junction or absolute Symlink --*/
                if ((ArgvPath.ReparseTag == (DWORD)IO_REPARSE_TAG_SYMLINK)) {
                    // ToDo later : -- re-set Symlink to inner Symlink --
                    DWORD isDirectory = ((ArgvPath.FileAttribute & FILE_ATTRIBUTE_DIRECTORY) != 0) ? SYMLINK_FLAG_DIRECTORY : 0;
#ifdef SymReplacement_Raw
                    blnRet = true;
                    intRet = CreateSymboliclinkRaw(Argv1Path.Argv.c_str(), wInnerLinkPath.c_str(), SYMLINK_FLAG_ALLOW_UNPRIVILEGED_CREATE | isDirectory);
                    if (intRet != 0) {
                        blnRet = false;
                        PrintError(intRet, TEXT("Symlink RemapToInnerLink: "), Argv1Path.Argv.c_str());
                    }
#elif SymReplacement_Normal
                    blnRet = true;
                    intRet = CreateSymboliclink(Argv1Path.Argv.c_str(), wInnerLinkPath.c_str(), SYMLINK_FLAG_ALLOW_UNPRIVILEGED_CREATE | isDirectory);
                    if (intRet != 0) {
                        blnRet = false;
                        PrintError(intRet, TEXT("Symlink RemapToInnerLink: "), Argv1Path.Argv.c_str());
                    }
#else
                    PrintError(0, TEXT("Symlink replacement not implemented yet."), TEXT(""));
                    blnRet = false;
#endif
                } else {
                    /*-- re-set junction to new inner link target --*/
                    blnRet = true;
                    intRet = CreateJunction(ArgvPath.Argv.c_str(), wInnerLinkPath.c_str());
                    if (intRet != 0) {
                        blnRet = false;
                        PrintError(intRet, TEXT("Junction RemapToInnerLink: "), ArgvPath.Argv.c_str());
                    }
                }
            }
        } else { //-- if (SearchInnerLinkPath = true) --
          blnRet = true;
        }
    }     //-- if (blnIsReparsePointAttr) --
    return blnRet;
} //-- ReplaceToInnerJunction() --


/*******************************************************************************
** Name        : ReplaceToInnerJunctionALL()
**
** Purpose     : Searches below folder for all junctions and remaps those junction to its next fitting directory, 
**               which is not a reparse point 
**
** Return      : iNumRemapped = number of remapped junctions;
**
*******************************************************************************/
bool ReplaceToInnerJunctionAll(wstring directoryName, int& iNumRemapped)
{
    bool blnRet    = true;
    bool blnRetLoc = false;

    WIN32_FIND_DATA fdata;
    HANDLE          hFind;
    _ArgvPath       ArgvPath;
    wstring         dirNameSearch;
    wchar_t         c;

    iNumRemapped = 0;

    //-- Check directory name end --
    if (PathIsUNC(directoryName.c_str()))
        directoryName += L"\\";

    //-- Append file search pattern to the directory path --
    c = directoryName.at(directoryName.length() - 1);
    if (c != '\\') 
        directoryName += L"\\";
    dirNameSearch = directoryName + L"*.*";

    //-- Search for reparse point below given folder
    if ((hFind = FindFirstFileW(dirNameSearch.c_str(), &fdata)) != INVALID_HANDLE_VALUE) {
        do {
            //-- If it is a reparse point -> remap to inner link
            if ((fdata.dwFileAttributes & FILE_ATTRIBUTE_REPARSE_POINT) != 0) {
                // build full pathname, if directoryName is not current directory
                ArgvPath.Argv = directoryName + fdata.cFileName;
                blnRetLoc = SetArgvPathProperties(&ArgvPath);
                if (blnRetLoc) {
                  blnRetLoc = ReplaceToInnerJunction(ArgvPath);
                  if (blnRetLoc)  iNumRemapped++;
                } else {
                  blnRet = false;
                  PrintError(0, TEXT("ReplaceToInnerJunctionAll() Could not set properties -> break: "), ArgvPath.Argv.c_str());
                  break;
                }
            } else if ((fdata.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0) {
                //-- Check directory name end for . and .. and avoid endless loop --
                if (!(wcscmp(fdata.cFileName, L".") == 0 || wcscmp(fdata.cFileName, L"..") == 0)) {
                  //-- If it is a directory, continue searching there --
                  // build full pathname, if directoryName is not current directory
                  ArgvPath.Argv = directoryName + fdata.cFileName;
                  blnRet        = ReplaceToInnerJunctionAll(ArgvPath.Argv.c_str(), iNumRemapped);
                    if (!blnRet) {
                    PrintError(0, TEXT("ReplaceToInnerJunctionAll() Sub-Directory not successfully handled -> break: "), ArgvPath.Argv.c_str());
                      break;
                    }
                }
            } // if(dwFileAttributes...)
            fdata.dwReserved0      = 0; //FindNextFile() does not reset all bits in dwReserved0, so reset them here.
            fdata.dwReserved1      = 0;
            fdata.dwFileAttributes = 0;
        } while (FindNextFile(hFind, &fdata) != 0);
        FindClose(hFind);
    }
    return blnRet;
} //-- ReplaceToInnerJunctionAll() --

