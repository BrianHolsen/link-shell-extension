//{{NO_DEPENDENCIES}}
// Microsoft Developer Studio generated include file.
// Used by ln.rc
//

#define STR_VALUE(arg)    #arg
#define STR_NAME(name)    STR_VALUE(name)

# define MAJOR_VERSION			2
# define MINOR_VERSION			9
# define PATCH_VERSION			2
# define HOTFIX_VERSION		  1

#define MAJOR_VERSION_STRING			STR_NAME(MAJOR_VERSION)
#define MINOR_VERSION_STRING			STR_NAME(MINOR_VERSION)
#define PATCH_VERSION_STRING			STR_NAME(PATCH_VERSION)
#define HOTFIX_VERSION_STRING			STR_NAME(HOTFIX_VERSION)
#define VERSION_DOT "."

#define VERSION_STRING MAJOR_VERSION_STRING VERSION_DOT MINOR_VERSION_STRING VERSION_DOT PATCH_VERSION_STRING VERSION_DOT HOTFIX_VERSION_STRING
#define VERSION_NUMBER MAJOR_VERSION_STRING VERSION_DOT MINOR_VERSION_STRING PATCH_VERSION_STRING HOTFIX_VERSION_STRING

// Next default values for new objects
// 
#ifdef APSTUDIO_INVOKED
#ifndef APSTUDIO_READONLY_SYMBOLS
#define _APS_NEXT_RESOURCE_VALUE        101
#define _APS_NEXT_COMMAND_VALUE         40001
#define _APS_NEXT_CONTROL_VALUE         1000
#define _APS_NEXT_SYMED_VALUE           101
#endif
#endif
