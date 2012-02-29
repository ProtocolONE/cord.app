#include "traywindow.h"

#include <QGraphicsObject>
#include <QDeclarativeContext>
#include <QApplication>
#include <QDesktopWidget>

#define SIGNAL_CONNECT_CHECK(X) { bool result = X; Q_ASSERT_X(result, __FUNCTION__ , #X); }

TrayWindow::TrayWindow(QWidget *parent)
  : QMainWindow(parent)
  , _menuType(TrayWindow::FullMenu)
{
  this->setWindowFlags(Qt::FramelessWindowHint | Qt::Popup | Qt::WindowStaysOnTopHint);  

  this->_trayMenuContainer = new QDeclarativeView();
  this->_trayMenuContainer->rootContext()->setContextProperty("installPath", "file:///" + QCoreApplication::applicationDirPath() + "/");
  this->_trayMenuContainer->rootContext()->setContextProperty("trayMenu", this);
  this->_trayMenuContainer->setSource(QUrl("qrc:/Blocks//Tray.qml"));

  this->_trayMenuContainer->setAlignment(Qt::AlignCenter);                                                                               
  this->_trayMenuContainer->setResizeMode(QDeclarativeView::SizeRootObjectToView);   

  this->setCentralWidget(this->_trayMenuContainer);  

  QIcon icon = QIcon(QCoreApplication::applicationDirPath() + "//images//icon.png");  

  this->_systray = new QSystemTrayIcon(this);
  this->_systray->setIcon(icon);
  this->_systray->show();

  QObject *item = this->_trayMenuContainer->rootObject();

  SIGNAL_CONNECT_CHECK(connect(this->_systray, SIGNAL(activated(QSystemTrayIcon::ActivationReason)), this, SLOT(mouseClicked(QSystemTrayIcon::ActivationReason))));
  SIGNAL_CONNECT_CHECK(connect(item, SIGNAL(menuClick(int)), this, SIGNAL(menuClick(int))));
  SIGNAL_CONNECT_CHECK(connect(item, SIGNAL(menuClick(int)), this, SLOT(menuMouseClicked(int))));  
  SIGNAL_CONNECT_CHECK(connect(item, SIGNAL(menuClick(int)), this, SLOT(hide())));

  this->_lastMouseClick = QPoint(0, 0);
  this->setMenuType(TrayWindow::OnlyQuitMenu);
}

TrayWindow::~TrayWindow()
{
  this->_systray->hide();
}

void TrayWindow::keyPressEvent(QKeyEvent* event)
{
  switch(event->key()){
  case Qt::Key_Alt:
  case Qt::Key_Escape: 
    this->hide();
    break;
  case Qt::Key_Up:
  case Qt::Key_Down:       
  case Qt::Key_Enter:
  case Qt::Key_Return:
    emit this->keyPressed(event->key());  
    break;
  }

  QMainWindow::keyPressEvent(event);      
}

void TrayWindow::hideEvent(QHideEvent* event) 
{
  emit this->hideMenu(); 
  QMainWindow::hideEvent(event);
}

int TrayWindow::menuType() const
{
  return this->_menuType;
}

void TrayWindow::resizeWindow(int w, int h)
{
  this->resize(w, h);
}

void TrayWindow::setMenuType(ContextMenuType menuType)
{
  if (this->_menuType == menuType)
    return;

  this->_menuType = menuType;
  emit this->menuTypeChanged();

  this->moveToTray();
}

void TrayWindow::moveToTray()
{
  int space = 20;
  QRect currentScreenRect = QApplication::desktop()->screenGeometry(this->_lastMouseClick);
  int relativeX = this->_lastMouseClick.x() - currentScreenRect.x();
  int relativeY = this->_lastMouseClick.y() - currentScreenRect.y();

  int resultX = relativeX;
  if (relativeX < 0)
    resultX = space;
  else if (relativeX > currentScreenRect.width() - width())
    resultX = currentScreenRect.width() - width() - space;

  int resultY = relativeY;
  if (relativeY < 0)
    resultY = space;
  else if (relativeY > currentScreenRect.height() - height())
    resultY = currentScreenRect.height() - height() - space;

  resultX += currentScreenRect.x();
  resultY += currentScreenRect.y();
  this->move(resultX, resultY);
}

void TrayWindow::menuMouseClicked(int index) 
{
  if (index == QuitMenu) 
    this->_systray->hide();
}

void TrayWindow::mouseClicked(QSystemTrayIcon::ActivationReason reason)   
{
  this->_lastMouseClick = QCursor::pos(); 
  this->moveToTray();

  switch(reason) {
  case QSystemTrayIcon::Trigger:
    emit this->activate();
    if (this->isVisible()) {
      this->hide();
    }
    break;
  case QSystemTrayIcon::Context: 
    if (this->isVisible()){
      this->hide();
    } else {
      this->activateWindow();
    }
    break;
  }
}

void TrayWindow::activateWindow()
{
  this->setFocusPolicy(Qt::StrongFocus);
  this->showMinimized(); 
  this->setWindowState(Qt::WindowActive); 
  this->showNormal();
  this->setFocus();
}
