#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    close = new QAction(tr("&Quit"), this);
    scanForCameras = new QAction("Scan for Cameras",this);
    showCameraStatus = new QAction("Show Camera Status",this);
    connect(close, SIGNAL(triggered()), qApp, SLOT(quit()));
    connect(scanForCameras, SIGNAL(triggered()), this, SLOT(scan()));
    connect(showCameraStatus,SIGNAL(triggered()),this, SLOT(showCameras()));

    trayIconMenu = new QMenu(this);
    trayIconMenu->addAction(showCameraStatus);
    trayIconMenu->addAction(scanForCameras);
    trayIconMenu->addSeparator();
    trayIconMenu->addAction(close);

    m_sys_tray_icon = new QSystemTrayIcon(this);
    m_icon = QIcon("C:/Users/User/Documents/QTProjects/CameraUseDetector/camera-alt-128.png");
    m_sys_tray_icon->setIcon(m_icon);
    m_sys_tray_icon->setContextMenu(trayIconMenu);

   // connect(m_sys_tray_icon, SIGNAL(activated(QSystemTrayIcon::ActivationReason reason)), this, SLOT(showCameras()));

    m_sys_tray_icon->show();

    m_cameraInUse = false;
    scan();
    if(m_timer)
    {
        delete m_timer;
        m_timer = nullptr;
    }
    m_timer = new QTimer(this);
    connect(m_timer, SIGNAL(timeout()), this, SLOT(onTimeout()));
    m_timer->start(1000);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::onTimeout()
{
    //QList<QCameraInfo> cameras = QCameraInfo::availableCameras();
    //const QCameraInfo &ci = cameras.at(0);
    for (QCameraInfo &ci : all_cameras)
    {
        QCamera *camera = new QCamera(ci, this);
        camera->load();
        camera->start();
        //qDebug() << camera->error();
        //qDebug() << camera->status();
        //qDebug() << camera->availability();
        if(camera->status() != QCamera::ActiveStatus) {
            if (!all_camera_status[ci.deviceName()])
            {
                m_sys_tray_icon->showMessage("Warning",ci.description() + " In Use",QSystemTrayIcon::Warning);
            }
            all_camera_status[ci.deviceName()] = true;
        }
        else {
            if(all_camera_status[ci.deviceName()]) m_sys_tray_icon->showMessage("Update", ci.description() + " no longer in use.");
            all_camera_status[ci.deviceName()] = false;
        }
        camera->stop();
        delete camera;
    }
}

void MainWindow::scan()
{
    qDebug() << "Scanning...";
    all_cameras = QCameraInfo::availableCameras();
    for(QCameraInfo camera : all_cameras)
    {
        qDebug() << camera.description();
        all_camera_status.insert(camera.deviceName(),false);
    }
}

void MainWindow::showCameras()
{
    qDebug() << "showing Cameras";
    QString message = "";
    for(QCameraInfo camera : all_cameras)
    {
        message += camera.description() + ": ";
        if(all_camera_status[camera.deviceName()])
        {
            message += "In Use";
        }
        else
        {
            message += "Not in Use";
        }
        message += "\n";
    }
    m_sys_tray_icon->showMessage("Status of Available Cameras",message);
}
