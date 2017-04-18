#include <QtScript/QScriptEngine>

#include <QtCore/QResource>
#include <QtCore/QTextStream>
#include <QtCore/QCoreApplication>
#include <QtCore/QFile>

namespace Features {
  namespace Helper {

    inline bool checkDebugApiConfig(QString& result)
    {
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
      if (overrideApi.isError() || !overrideApi.isBool())
        return false;

      if (!overrideApi.toBool())
        return false;

      QScriptValue apiUrl = config.property("apiUrl");
      if (apiUrl.isError() || !apiUrl.isString())
        return false;

      result = apiUrl.toString();
      return !result.isEmpty();
    }

  }
}