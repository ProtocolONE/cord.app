#pragma once

namespace Features {

  class ComInitialize
  {
  public:
    ComInitialize();
    ~ComInitialize();

    bool isSuccess();

  private:
    bool _success;
  };

}