#pragma once

#include <QtCore/QString>
#include <QtCore/QSettings>

#include <stdint.h>
#include <string>

namespace P1 {

  class ApplicationArea
  {
  public:

    enum class SupportedArea : uint32_t
    {
      Live = 0,
      Pts = 1,
      Tst = 2,
      Live2 = 3, // 2live
      Tst2 = 4,
      Tst3 = 5,
    };

    friend bool operator ==(const uint32_t &a, const ApplicationArea::SupportedArea &b);
    friend bool operator !=(const uint32_t &a, const ApplicationArea::SupportedArea &b);

    const uint32_t DefaultArea = static_cast<uint32_t>(SupportedArea::Live);

    ApplicationArea()
      : _area(DefaultArea)
    {
    }

    ApplicationArea(uint32_t value)
      : _area(isSupportedArea(value) ? value : DefaultArea)
    {
    }

    ApplicationArea(const std::string& value)
    {
      this->parse(value);
    }

    ApplicationArea(SupportedArea value)
      : _area(static_cast<uint32_t>(value))
    {
    }

    ApplicationArea(const ApplicationArea&) = default;
    ApplicationArea& operator=(const ApplicationArea& other) = default;
    
    ApplicationArea& operator=(const ApplicationArea::SupportedArea& other) {
      this->_area = static_cast<uint32_t>(other);
      return *this;
    }

    bool isSupportedArea(uint32_t value)
    {
      switch (value)
      {
      case SupportedArea::Live:
      case SupportedArea::Pts:
      case SupportedArea::Live2:
      case SupportedArea::Tst:
      case SupportedArea::Tst2:
      case SupportedArea::Tst3:
        return true;
      default:
        return false;
      }
    }

    bool isPublicTest() const
    {
      return this->_area == SupportedArea::Pts;
    }

    bool isLive() const
    {
      return this->_area == SupportedArea::Live;
    }

    bool isTestVersion() const 
    {
      if (this->_area == SupportedArea::Live)
        return false;

      if (this->_area == SupportedArea::Pts)
        return false;

      if (this->_area == SupportedArea::Live2)
        return false;

      return true;
    }

    operator uint32_t() const
    {
      return _area;
    }

    operator QString() const
    {
      return QString::fromStdString(this->operator std::string());
    }

    operator std::string() const
    {
      switch (this->_area)
      {
      case SupportedArea::Live:
          return std::string("live");
      case SupportedArea::Pts:
          return std::string("pts");
      case SupportedArea::Live2:
        return std::string("2live");
      case SupportedArea::Tst:
        return std::string("tst");
      case SupportedArea::Tst2:
        return std::string("tst2");
      case SupportedArea::Tst3:
        return std::string("tst3");
      default:
        return std::string();
      }
    }

    bool parse(const QString& value)
    {
      return parse(value.toStdString());
    }

    bool parse(const std::string& value)
    {
      this->_area = DefaultArea;

      if (value == "live")
        this->_area = static_cast<uint32_t>(SupportedArea::Live);
      else if (value == "pts")
        this->_area = static_cast<uint32_t>(SupportedArea::Pts);
      else if (value == "2live")
        this->_area = static_cast<uint32_t>(SupportedArea::Live2);
      else if (value == "tst")
        this->_area = static_cast<uint32_t>(SupportedArea::Tst);
      else if (value == "tst2")
        this->_area = static_cast<uint32_t>(SupportedArea::Tst2);
      else if (value == "tst3")
        this->_area = static_cast<uint32_t>(SupportedArea::Tst3);
      else return false;

      return true;
    }

    void load()
    {
      this->_area = DefaultArea;

      QSettings settings("HKEY_LOCAL_MACHINE\\SOFTWARE\\GGS\\QGNA", QSettings::NativeFormat);
      bool ok = false;
      int area = settings.value("Repository", 0).toInt(&ok);
      if (!ok)
        return;

      if (this->isSupportedArea(area))
        this->_area = area;
    }

    void save()
    {
      QSettings settings("HKEY_LOCAL_MACHINE\\SOFTWARE\\GGS\\QGNA", QSettings::NativeFormat);
      settings.setValue("Repository", this->_area);
    }

  private:
    uint32_t _area;
  };

  inline bool operator ==(const uint32_t &a, const ApplicationArea::SupportedArea &b)
  {
    return a == static_cast<uint32_t>(b);
  }

  inline bool operator !=(const uint32_t &a, const ApplicationArea::SupportedArea &b)
  {
    return !(a == static_cast<uint32_t>(b));
  }

  inline bool operator ==(const ApplicationArea::SupportedArea &a, const ApplicationArea &b)
  {
    return b == a;
  }


}