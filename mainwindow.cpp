#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    //Set up the icon menu items
    close = new QAction(tr("&Quit"), this);
    scanForCameras = new QAction("Scan for Cameras",this);
    showCameraStatus = new QAction("Show Camera Status",this);
    connect(close, SIGNAL(triggered()), qApp, SLOT(quit()));
    connect(scanForCameras, SIGNAL(triggered()), this, SLOT(scan()));
    connect(showCameraStatus,SIGNAL(triggered()),this, SLOT(showCameras()));

    //Create the Icon Menu
    trayIconMenu = new QMenu(this);
    trayIconMenu->addAction(showCameraStatus);
    trayIconMenu->addAction(scanForCameras);
    trayIconMenu->addSeparator();
    trayIconMenu->addAction(close);

    //Create the Icon
    m_sys_tray_icon = new QSystemTrayIcon(this);
    m_icon = QIcon("C:/Users/User/Documents/QTProjects/CameraUseDetector/camera-alt-128.png");
    m_sys_tray_icon->setIcon(m_icon);
    m_sys_tray_icon->setContextMenu(trayIconMenu);

    //Show the icon in the system tray
    m_sys_tray_icon->show();

    //Perfom an initial scan of connected cameras then start a timer
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
    //Loop through all discovered cameras
    for (QCameraInfo &ci : all_cameras)
    {
        QCamera *camera = new QCamera(ci, this);
        camera->load();
        camera->start();//If the camera is already in use by another application then this will throw an error signal
        if(camera->status() != QCamera::ActiveStatus) { //This camera is in use by another application
            if (!all_camera_status[ci.deviceName()]) //This is the first cycle camera use has been detected
            {
                m_sys_tray_icon->showMessage("Warning",ci.description() + " In Use",QSystemTrayIcon::Warning);
            }
            all_camera_status[ci.deviceName()] = true;
        }
        else {// We were able to connect thereby proving no other applications are using the camera
            if(all_camera_status[ci.deviceName()]) m_sys_tray_icon->showMessage("Update", ci.description() + " no longer in use.");
            all_camera_status[ci.deviceName()] = false;
        }
        camera->stop();//disconnect from the camera
        delete camera;
    }
}

void MainWindow::scan()
{
    all_cameras = QCameraInfo::availableCameras();
    for(QCameraInfo camera : all_cameras)
    {
        all_camera_status.insert(camera.deviceName(),false);
    }
}

void MainWindow::showCameras()
{
    QString message = "";
    for(QCameraInfo camera : all_cameras)
    {
        message += camera.description() + ": ";
        if(all_camera_status[camera.deviceName()])
        {
            message += "In Use\n";
        }
        else
        {
            message += "Not in Use\n";
        }
    }
    m_sys_tray_icon->showMessage("Status of Available Cameras",message);
}
