/****************************************************************************
** This file is a part of Syncopate Limited GameNet Application or it parts.
**
** Copyright (©) 2011 - 2013, Syncopate Limited and/or affiliates. 
** All rights reserved.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
****************************************************************************/

#include <shlobj.h>
#include <Propvarutil.h>
#include <Propsys.h>

#include <Features/TaskList.h>
#include <QtCore/QString>
#include <QCoreApplication>
#include <qDebug>

namespace Features {

  TaskList::TaskList() {
    CoInitialize(NULL);
  }

  TaskList::~TaskList() {
    Q_FOREACH(TaskItemPrivate item, this->_taskItems.values()) {
      item._shellLink->Release();
    }

    CoUninitialize();
  }

  bool isWinVistaOrLater() {
    DWORD version = GetVersion();
    DWORD major = (DWORD)(LOBYTE(LOWORD(version)));

    return (major >= 6);
  }

  void TaskList::commit() {
    if (!isWinVistaOrLater())
      return;

    UINT max_count = 0;
    IObjectArray* objectArray;
    ICustomDestinationList* destinationList;

    CoCreateInstance(CLSID_DestinationList, NULL, CLSCTX_INPROC, IID_ICustomDestinationList, reinterpret_cast<void**> (&(destinationList)));

    destinationList->BeginList(&max_count, IID_IObjectArray, reinterpret_cast<void**> (&(objectArray)));

    IObjectCollection* obj_collection;
    CoCreateInstance(CLSID_EnumerableObjectCollection, NULL,
      CLSCTX_INPROC, IID_IObjectCollection, reinterpret_cast<void**> (&(obj_collection)));

    IObjectArray* object_array;
    obj_collection->QueryInterface(IID_IObjectArray, reinterpret_cast<void**> (&(object_array)));
    
    Q_FOREACH(IShellLink* shell_link, this->_tasksLinks) {
      obj_collection->AddObject(shell_link);
    }

    destinationList->AddUserTasks(object_array);
    obj_collection->Clear();

    Q_FOREACH(TaskItemPrivate item, this->_taskItems.values()) {
      if (item.isInstalled)
        obj_collection->AddObject(item._shellLink);
    }

    destinationList->AppendCategory(this->_allGamesCaption.toStdWString().c_str(), object_array);
    obj_collection->Clear();

    Q_FOREACH(TaskItemPrivate item, this->_taskItems.values()) {
      if (!item.isInstalled)
        obj_collection->AddObject(item._shellLink);
    }

    destinationList->AppendCategory(this->_moreGamesCaption.toStdWString().c_str(), object_array);
    destinationList->CommitList();

    object_array->Release();
    obj_collection->Release();
    objectArray->Release();
    destinationList->Release();
  }

  void TaskList::setMoreGamesCaption(QString label)
  {
    this->_moreGamesCaption = label;
  }

  void TaskList::setAllGamesCaption(QString label)
  {
    this->_allGamesCaption = label;
  }

  void TaskList::addTask(const QString& commandLine, const QString& title) 
  {
    if (!isWinVistaOrLater())
      return;

    QString appPath = QCoreApplication::applicationFilePath();
    IShellLink* shell_link = NULL;
    CoCreateInstance(CLSID_ShellLink, NULL, CLSCTX_INPROC_SERVER, IID_IShellLink, reinterpret_cast<void**>(&(shell_link)));

    shell_link->SetPath(appPath.toStdWString().c_str());
    shell_link->SetArguments(commandLine.toStdWString().c_str());

    PROPVARIANT pv;
    IPropertyStore* prop_store = NULL;
    shell_link->QueryInterface(IID_IPropertyStore, reinterpret_cast<void**>(&(prop_store)));

    PROPERTYKEY PKEY_Title;
    CLSIDFromString(L"{F29F85E0-4FF9-1068-AB91-08002B27B3D9}", &(PKEY_Title.fmtid));
    PKEY_Title.pid = 2;

    InitPropVariantFromString(title.toStdWString().c_str(), &pv);
    prop_store->SetValue(PKEY_Title, pv);
    prop_store->Commit();


    this->_tasksLinks.append(shell_link);
  }

  void TaskList::addItem(
    QString serviceId,
    QString title, 
    QString args,
    QString iconPath,       
    QString description,
    bool isInstalled)
  {
    if (!isWinVistaOrLater())
      return;

    IShellLink* shell_link = NULL;
    CoCreateInstance(CLSID_ShellLink, NULL, CLSCTX_INPROC_SERVER, IID_IShellLink, reinterpret_cast<void**> (&(shell_link)));

    shell_link->SetPath(QCoreApplication::applicationFilePath().toStdWString().c_str());
    shell_link->SetArguments(args.toStdWString().c_str());
    shell_link->SetIconLocation(iconPath.toStdWString().c_str(), 0);
    shell_link->SetDescription(description.toStdWString().c_str());

    PROPVARIANT pv;
    IPropertyStore* prop_store = NULL;
    shell_link->QueryInterface(IID_IPropertyStore, reinterpret_cast<void**> (&(prop_store)));

    PROPERTYKEY PKEY_Title;
    CLSIDFromString(L"{F29F85E0-4FF9-1068-AB91-08002B27B3D9}", &(PKEY_Title.fmtid));
    PKEY_Title.pid = 2;

    InitPropVariantFromString(title.toStdWString().c_str(), &pv);
    prop_store->SetValue(PKEY_Title, pv);
    prop_store->Commit();

    TaskItemPrivate taskItem;
    taskItem.serviceId = serviceId;
    taskItem.isInstalled = isInstalled;
    taskItem._shellLink = shell_link;

    this->_taskItems[serviceId] = taskItem;
  }

  void TaskList::killTasksList() {
    this->_tasksLinks.clear();
  }

  void TaskList::updateItem(QString serviceId, bool isInstalled) {
    this->_taskItems[serviceId].isInstalled = isInstalled;
  }

}