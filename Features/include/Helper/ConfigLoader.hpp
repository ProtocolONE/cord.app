#include <QScriptEngine>

#include <QtCore/QResource>
#include <QtCore/QTextStream>
#include <QtCore/QCoreApplication>
#include <QtCore/QFile>

namespace Features {
  namespace Helper {

    class DebugConfigLoader
    {
    public:
      DebugConfigLoader()
        : _isValid(false)
        , _debugLogEnabled(false)
      {
      }

      void init()
      {
        if (this->load())
          this->_isValid = true;
      }

      bool apiConfig(QString& result)
      {
        if (!this->_isValid || this->_apiUrl.isEmpty())
          return false;

        result = this->_apiUrl;
        return true;
      }

      bool debugApiEnabled(bool& result)
      {
        if (!this->_isValid)
          return false;

        result = this->_debugLogEnabled;
        return true;
      }

    private:
      bool load() {
        QString path = QCoreApplication::applicationDirPath();
        QString altConfigPth = path + "/Config.rcc";
        if (!QFile::exists(altConfigPth))
          return false;

        if (!QResource::registerResource(altConfigPth))
          return false;

        QString fileName = ":/ExternalConfig/Config.js";
        QFile scriptFile(fileName);
        if (!scriptFile.open(QIODevice::ReadOnly))
          return false;

        QTextStream stream(&scriptFile);

        QString contents = stream.readAll();
        scriptFile.close();

        QScriptEngine engine;
        engine.evaluate(contents);

        QScriptValue getAltConfig = engine.globalObject().property("getAltConfig");
        if (getAltConfig.isError() || !getAltConfig.isFunction())
          return false;

        QScriptValueList args;
        QScriptValue config = getAltConfig.call(QScriptValue(), args);
        if (config.isError() || !config.isObject())
          return false;

        QScriptValue overrideApi = config.property("overrideApi");
        if (!overrideApi.isError() && overrideApi.isBool() && overrideApi.toBool()) {
          QScriptValue apiUrl = config.property("apiUrl");
          if (!apiUrl.isError() && apiUrl.isString())
            this->_apiUrl = apiUrl.toString();
        }

        QScriptValue debugApi = config.property("debugApi");
        if (!debugApi.isError() && debugApi.isBool())
          this->_debugLogEnabled = debugApi.toBool();

        return true;
      }

      bool _isValid;
      QString _apiUrl;
      bool _debugLogEnabled;
    };
  }
}