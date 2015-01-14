#include <Features/ComInitialize.h>

#include <Windows.h>

namespace Features {

  ComInitialize::ComInitialize()
     : _success(false)
  {
    HRESULT hres =  CoInitializeEx(0, COINIT_MULTITHREADED);
    if (hres == RPC_E_CHANGED_MODE)
      hres =  CoInitializeEx(0, COINIT_APARTMENTTHREADED);

    this->_success = SUCCEEDED(hres);
  }

  ComInitialize::~ComInitialize()
  {
    if (this->_success)
      CoUninitialize();
  }

  bool ComInitialize::isSuccess()
  {
    return this->_success;
  }

}
