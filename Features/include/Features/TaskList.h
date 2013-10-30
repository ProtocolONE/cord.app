/****************************************************************************
** This file is a part of Syncopate Limited GameNet Application or it parts.
**
** Copyright (©) 2011 - 2013, Syncopate Limited and/or affiliates. 
** All rights reserved.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
****************************************************************************/

#pragma once

#include <shlobj.h>
#include <QtCore/QString>
#include <QtCore/QList>
#include <QtCore/QMap>
#include <QtCore/QObject>

namespace Features {

  DEFINE_GUID(CLSID_DestinationList,0x77f10cf0,0x3db5,0x4966,0xb5,0x20,0xb7,0xc5,0x4f,0xd3,0x5e,0xd6);
  DEFINE_GUID(IID_IObjectArray,0x92ca9dcd,0x5622,0x4bba,0xa8,0x05,0x5e,0x9f,0x54,0x1b,0xd8,0xc9);
  DEFINE_GUID(IID_ICustomDestinationList,0x6332debf,0x87b5,0x4670,0x90,0xc0,0x5e,0x57,0xb4,0x08,0xa4,0x9e);

  DEFINE_PROPERTYKEY(PKEY_AppUserModel_IsDestListSeparator, 0x9F4C2855, 0x9F79, 0x4B39, 0xA8, 0xD0, 0xE1, 0xD4, 0x2D, 0xE1, 0xD5, 0xF3, 6);

  DECLARE_INTERFACE_(ICustomDestinationList, IUnknown)
  {
    STDMETHOD (SetAppID) (LPCWSTR pszAppID);
    STDMETHOD (BeginList) (UINT *pcMinSlots, REFIID riid, void **ppv) PURE;
    STDMETHOD (AppendCategory) (LPCWSTR pszCategory, IObjectArray *poa) PURE;
    STDMETHOD (AppendKnownCategory) (KNOWNDESTCATEGORY category) PURE;
    STDMETHOD (AddUserTasks) (IObjectArray *poa) PURE;
    STDMETHOD (CommitList) (void) PURE;
    STDMETHOD (GetRemovedDestinations) (REFIID riid, void **ppv) PURE;
    STDMETHOD (DeleteList) (LPCWSTR pszAppID) PURE;
    STDMETHOD (AbortList)  (void) PURE;

  };

  extern "C"
  {
    typedef HRESULT (WINAPI *t_SHCreateItemFromParsingName)(PCWSTR pszPath, IBindCtx *pbc, REFIID riid, void **ppv);
  }

  typedef ICustomDestinationList *LPICustomDestinationList;

  class TaskItemPrivate {
  public:
    QString serviceId;
    bool isInstalled;
    IShellLink* _shellLink;
  };

  class TaskList: public QObject
  {
    Q_OBJECT
  public:
    TaskList();
    ~TaskList();

  public slots:
    void addTask(const QString& commandLine, const QString& title);
    void killTasksList();

    void addItem(QString serviceId,
                  QString title,
                  QString args,
                  QString iconPath, 
                  QString description,
                  bool isInstalled);

    void updateItem(QString serviceId,
      bool isInstalled);

    void commit();

    void setMoreGamesCaption(QString label);
    void setAllGamesCaption(QString label);

  private:
    QList<IShellLink*> _tasksLinks;

    QMap<QString, TaskItemPrivate> _taskItems;

    QList<IShellLink*> _allGamesLinks;
    QList<IShellLink*> _installedGamesLinks;

    QString _allGamesCaption;
    QString _moreGamesCaption;
  };

}
