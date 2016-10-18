#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSystemTrayIcon>
#include <QMenu>
#include <QTimer>
#include <QCameraInfo>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    QIcon m_icon;
    QTimer *m_timer = nullptr;
    QSystemTrayIcon *m_sys_tray_icon = nullptr;
    QMenu *trayIconMenu;
    QAction *close;
    QAction *scanForCameras;
    QAction *showCameraStatus;

    QList<QCameraInfo> all_cameras;
    QMap<QString,bool> all_camera_status;

private slots:
    void onTimeout();
    void scan();
    void showCameras();
};

#endif // MAINWINDOW_H
