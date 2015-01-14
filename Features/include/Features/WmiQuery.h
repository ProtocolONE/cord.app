#pragma once

#include <string>
#include <unordered_map>
#include <vector>

namespace Features {

  class WmiQuery
  {
  public:
    WmiQuery();
    ~WmiQuery();

    bool getFromTable(
      const std::wstring& serviceName,
      const std::wstring& tableName,
      const std::vector<std::wstring>& keys,
      std::vector<std::unordered_map<std::wstring, std::wstring> >& result);

    bool execWql(
      const std::wstring& serviceName,
      const std::wstring& query,
      const std::vector<std::wstring>& keys,
      std::vector<std::unordered_map<std::wstring, std::wstring> >& result);

  };

}
