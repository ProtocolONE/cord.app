/****************************************************************************
** This file is a part of Syncopate Limited GameNet Application or it parts.
**
** Copyright (©) 2011 - 2012, Syncopate Limited and/or affiliates.
** All rights reserved.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
****************************************************************************/

#pragma  once

#include <QtCore/QObject>
#include <QtQml/QQmlNetworkAccessManagerFactory>
#include <QtNetwork/QNetworkAccessManager>

class CacheNetworkManagerFactory : public QObject, public QQmlNetworkAccessManagerFactory
{
  Q_OBJECT
public:
  explicit CacheNetworkManagerFactory(QObject *parent = 0);
  virtual QNetworkAccessManager *create(QObject *parent) override;
};
