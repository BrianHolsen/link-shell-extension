#ifndef __MULTILANGUAGE_H_
#define __MULTILANGUAGE_H_

#include "MultiLang.h"

#define MLG_PREFIX L"#MLG#"
#define MLG_PREFIX_SIZE sizeof("#MLG#") - 1

class CMultiLanguage
{
  protected:
    WORD    m_LangCode;
    HINSTANCE m_Instance;

  public:
    CMultiLanguage();
	  virtual ~CMultiLanguage();

    HRESULT 
    ReplaceWindowTexts(
      HWND hWnd
    );

    LPCWSTR 
    Replace(
      wchar_t*  a_StringKey
    );

    void SetLangCode(WORD a_LangCode) { m_LangCode = a_LangCode; };
    WORD GetLangCode() { return m_LangCode; };

};


//////////////////////////////////////////////////////////////////////
// Common Stuff
//////////////////////////////////////////////////////////////////////

struct EnumWinData
{
	CMultiLanguage *pLangObj;
};



#endif //__MULTILANGUAGE_H_
