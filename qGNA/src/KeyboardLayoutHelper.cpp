#include "KeyboardLayoutHelper.h"

namespace P1 {

   KeyboardLayoutHelper::KeyboardLayoutHelper() : 
    _capsEnabled(false),
    _layoutCode(0),
    _layoutName(QString()) {

  }

  KeyboardLayoutHelper::~KeyboardLayoutHelper() {
  }

  QString KeyboardLayoutHelper::keyboardLayout() {
    return this->_layoutName;
  }

  bool KeyboardLayoutHelper::capsLockEnabled() {
    return this->_capsEnabled;
  }

  void KeyboardLayoutHelper::changeLayout()  {
    HKL nextLayout;

    int uLayouts = GetKeyboardLayoutList(0, NULL);
    HKL* lpList   = new HKL[uLayouts * sizeof(HKL)];
    uLayouts = GetKeyboardLayoutList(uLayouts, lpList);

    for(int i = 0; i < uLayouts; ++i) {
      if (lpList[i] != this->_layoutCode)
        continue;

      if (i < uLayouts - 1) {
        nextLayout = lpList[i + 1];
        break;
      }

      nextLayout = lpList[0];
      break;
    }

    ::ActivateKeyboardLayout(nextLayout, KLF_SETFORPROCESS);
    delete [] lpList;

    this->update();
  }

  void KeyboardLayoutHelper::update()
  {
    bool capsEnabled = ((GetKeyState(VK_CAPITAL) & 0x0001) != 0);

    if (_capsEnabled != capsEnabled) {
      this->_capsEnabled = capsEnabled;  

      emit this->keyboardCapslockChanged();
    }

    HKL layoutCode = GetKeyboardLayout(::GetCurrentThreadId());

    if (_layoutCode != layoutCode) {
      this->_layoutCode = layoutCode;

      wchar_t szBuf[512];
      int cid = MAKELCID(((UINT)layoutCode & 0xffffffff), SORT_DEFAULT);

      GetLocaleInfo(cid, LOCALE_SABBREVLANGNAME, szBuf, 512);
      QString localeName = QString::fromWCharArray(szBuf);
      localeName.resize(localeName.size() - 1);
      /*
      http://msdn.microsoft.com/en-us/library/windows/desktop/dd373831(v=vs.85).aspx

      LOCALE_SABBREVLANGNAME	Abbreviated name of the language. 
      In most cases, the name is created by taking the two-letter language abbreviation from ISO Standard 639 and 
      adding a third letter, as appropriate, to indicate the sublanguage. 
      For example, the abbreviated name for the language corresponding to the English (United States) locale is ENU.
      */

      this->_layoutName = localeName;

      emit this->keyboardLayoutChanged();
    }
  }
}