#ifndef TRAYWINDOW_H
#define TRAYWINDOW_H

#include <QMainWindow>
#include <QDialog>
#include <QMenu>
#include <QAction>
#include <QPoint>
#include <QDeclarativeView>
#include <QSystemTrayIcon>
#include <QKeyEvent>

class TrayWindow : public QMainWindow
{
  Q_OBJECT
  Q_ENUMS(MenuLabel)
  Q_ENUMS(ContextMenuType)
  Q_PROPERTY(int menuType READ menuType NOTIFY menuTypeChanged) 

public:
  enum MenuLabel {
    ProfileMenu = 0, 
    MoneyMenu = 1,
    SettingsMenu = 2, 
    QuitMenu = 3  
  };

  enum ContextMenuType{
    OnlyQuitMenu = 0,
    FullMenu = 1 
  };
  
    TrayWindow(QWidget *parent = 0);
    ~TrayWindow();

    int menuType() const;
    void setMenuType(ContextMenuType menuType);  

public slots:
  void resizeWindow(int w, int h);
  void moveToTray();

signals:
    void activate();
    void menuClick(int index);
    void menuTypeChanged();
    void keyPressed(int key);
    void hideMenu();

private slots:
    void mouseClicked(QSystemTrayIcon::ActivationReason reason); 
    void menuMouseClicked(int index);
    void activateWindow();

protected:
    void keyPressEvent(QKeyEvent* event);
    void hideEvent(QHideEvent* event);

private:
      QDeclarativeView *_trayMenuContainer; 
      QSystemTrayIcon* _systray;
      ContextMenuType _menuType;
      QPoint _lastMouseClick;
};

#endif // TRAYWINDOW_H
