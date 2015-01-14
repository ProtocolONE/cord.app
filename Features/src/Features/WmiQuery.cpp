#include <Features/WmiQuery.h>
#include <Features/ComInitialize.h>

#include <Windows.h>
#include <Wbemidl.h>
#include <comdef.h>
#include <atlbase.h>
#include <string>

namespace Features {

  WmiQuery::WmiQuery()
  {
  }

  WmiQuery::~WmiQuery()
  {
  }

  bool WmiQuery::getFromTable(
    const std::wstring& serviceName,
    const std::wstring& tableName,
    const std::vector<std::wstring>& keys,
    std::vector<std::unordered_map<std::wstring, std::wstring> >& result)
  {
    if (keys.empty())
      return false;

    std::wstring wql(L"select ");
    for (size_t i = 0; i < keys.size(); ++i) {
      if (i != 0)
        wql.append(L",");

      wql.append(keys[i]);
    }

    wql.append(L" from ");
    wql.append(tableName);

    return this->execWql(serviceName, wql, keys, result);
  }

  bool WmiQuery::execWql(
    const std::wstring& serviceName, 
    const std::wstring& query, 
    const std::vector<std::wstring>& keys, 
    std::vector<std::unordered_map<std::wstring, std::wstring> >& result)
  {
    if (keys.empty())
      return false;

    HRESULT hres;
    ComInitialize init;
    if (!init.isSuccess())
      return false;

    hres = CoInitializeSecurity( 
      NULL,
      -1,                          // COM authentication
      NULL,                        // Authentication services
      NULL,                        // Reserved
      RPC_C_AUTHN_LEVEL_DEFAULT,   // Default authentication
      RPC_C_IMP_LEVEL_IMPERSONATE, // Default Impersonation
      NULL,                        // Authentication info
      EOAC_NONE,                   // Additional capabilities
      NULL                         // Reserved
      );

    if (FAILED(hres) && hres != RPC_E_TOO_LATE)
      return false;

    CComPtr<IWbemLocator> wbemLocator;
    hres = CoCreateInstance(
      CLSID_WbemAdministrativeLocator,
      NULL,
      CLSCTX_INPROC_SERVER | CLSCTX_LOCAL_SERVER,
      IID_IUnknown,
      (void **)&wbemLocator);

    if (FAILED(hres))
      return false;

    CComBSTR serviceNameBSTR(serviceName.c_str());
    CComPtr<IWbemServices> service;
    hres = wbemLocator->ConnectServer(
      serviceNameBSTR,// Namespace
      NULL,           // Userid
      NULL,           // PW
      CComBSTR("MS_409"),// Locale
      0,              // flags
      NULL,           // Authority
      NULL,           // Context
      &service);

    if (hres == WBEM_E_INVALID_PARAMETER) {
      hres = wbemLocator->ConnectServer(
        serviceNameBSTR,// Namespace
        NULL,           // Userid
        NULL,           // PW
        NULL,           // Locale
        0,              // flags
        NULL,           // Authority
        NULL,           // Context
        &service);
    }

    if (FAILED(hres))
      return false;

    hres = CoSetProxyBlanket(
      service,                     // Indicates the proxy to set
      RPC_C_AUTHN_WINNT,           // RPC_C_AUTHN_xxx
      RPC_C_AUTHZ_NONE,            // RPC_C_AUTHZ_xxx
      NULL,                        // Server principal name
      RPC_C_AUTHN_LEVEL_CALL,      // RPC_C_AUTHN_LEVEL_xxx
      RPC_C_IMP_LEVEL_IMPERSONATE, // RPC_C_IMP_LEVEL_xxx
      NULL,                        // client identity
      EOAC_NONE                    // proxy capabilities
      );
    if (FAILED(hres))
      return false;

    CComBSTR queryBSTR(query.c_str());
    CComPtr<IEnumWbemClassObject> enumObject;
    if (service->ExecQuery(L"WQL", queryBSTR, WBEM_FLAG_RETURN_IMMEDIATELY, NULL, &enumObject) != S_OK)
      return false;

    if (FAILED(enumObject->Reset()))
      return false;

    IWbemClassObject *classObject = nullptr;

    unsigned long returned = 0;
    unsigned long count = 1;

    while (!FAILED(enumObject->Next(WBEM_INFINITE, count, &classObject, &returned))) {
      if (0 == returned)
        break;

      std::unordered_map<std::wstring, std::wstring> row;
      for (size_t i = 0; i < keys.size(); ++i) {
        std::wstring keyValue;
        const std::wstring& key(keys[i]);
        VARIANT value;

        CIMTYPE cimType;
        if (classObject->Get(key.c_str(), 0, &value, &cimType, 0) == S_OK) {
          if (value.vt != VT_NULL) {

            bool cimUnsigned = 
              cimType == CIM_UINT8
              || cimType == CIM_UINT16
              || cimType == CIM_UINT32
              || cimType == CIM_UINT64;
            bool vtSigned = 
              value.vt == VT_I1
              || value.vt == VT_I2
              || value.vt == VT_I4
              || value.vt == VT_I8
              || value.vt == VT_INT;

            bool numberConverted = false;
            if (cimUnsigned && vtSigned) {
              // HACK тут мы позверствуем и скастуем куда что надо...
              // сходу скастим после этого в сторку

              unsigned long long num = 0;
              switch(value.vt) {
              case VT_I1:
                numberConverted = true;
                num = (unsigned long long)((unsigned char)value.cVal);
                break;

              case VT_I2:
                numberConverted = true;
                num = (unsigned long long)((unsigned short)value.iVal);
                break;

              case VT_INT: // sizeof(int) == 4
              case VT_I4:
                numberConverted = true;
                num = (unsigned long long)((unsigned long)value.lVal);
                break;

              case VT_I8:
                numberConverted = true;
                num = (unsigned long long)(value.llVal);
                break;
              }

              if (numberConverted)
                keyValue = std::to_wstring(num);
            }

            if (!numberConverted) {
              VARIANT valueStr;
              VariantInit(&valueStr);

              if (VariantChangeType(&valueStr, &value, 0, VT_BSTR) == S_OK) {
                keyValue = std::wstring(_bstr_t(valueStr.bstrVal));
                VariantClear(&valueStr);
              }
            }
          }
        }

        VariantClear(&value);
        row[key] = keyValue;
      }

      classObject->Release();
      result.push_back(row);
    }

    return true;
  }

}