#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QApplication>
#include <QtConcurrentRun>
#include <QtWidgets/QMessageBox>
#include <QHBoxLayout>
#include <QToolBar>
#include <QFileDialog>
#include <QUrl>
#include <QDir>
#include <QtDataVisualization/Q3DTheme>
#include <QtDataVisualization/QCustom3DItem>
#include <QtDataVisualization/QCustom3DLabel>
#include <QtGui/QImage>
#include <QDebug>
#include <QLabel>
#include <QDockWidget>

#include <dialog.h>
#include <importdialog.h>
#include <math.h>

using namespace QtDataVisualization;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    createActions();

    createDockWindows();

    statusBar()->showMessage(QString(tr("Ready")));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::createActions()
{
    QToolBar *mainToolBar = addToolBar(tr("ToolBar"));;

    const QIcon importIcon = QIcon::fromTheme("document-new", QIcon(":/assets/import.png"));
    importAct = new QAction(importIcon, tr("&Import data"), this);
    connect(importAct, &QAction::triggered, this, &MainWindow::import);
    mainToolBar->addAction(importAct);

    mainToolBar->addSeparator();

    const QIcon undoIcon = QIcon::fromTheme("document-new", QIcon(":/assets/undo.png"));
    undoAct = new QAction(undoIcon, tr("&Undo"), this);
    //connect(undoAct, &QAction::triggered, this, &MainWindow::importData);
    mainToolBar->addAction(undoAct);

    const QIcon cutIcon = QIcon::fromTheme("document-new", QIcon(":/assets/cut.png"));
    cutAct = new QAction(cutIcon, tr("&Cut"), this);
    //connect(undoAct, &QAction::triggered, this, &MainWindow::importData);
    mainToolBar->addAction(cutAct);

    const QIcon copyIcon = QIcon::fromTheme("document-new", QIcon(":/assets/copy.png"));
    copyAct = new QAction(copyIcon, tr("&Copy"), this);
    //connect(undoAct, &QAction::triggered, this, &MainWindow::importData);
    mainToolBar->addAction(copyAct);

    const QIcon pasteIcon = QIcon::fromTheme("document-new", QIcon(":/assets/paste.png"));
    pasteAct = new QAction(pasteIcon, tr("&Paste"), this);
    //connect(undoAct, &QAction::triggered, this, &MainWindow::importData);
    mainToolBar->addAction(pasteAct);

    const QIcon deleteIcon = QIcon::fromTheme("document-new", QIcon(":/assets/delete.png"));
    deleteAct = new QAction(deleteIcon, tr("&Delete"), this);
    //connect(undoAct, &QAction::triggered, this, &MainWindow::importData);
    mainToolBar->addAction(deleteAct);

    const QIcon trimIcon = QIcon::fromTheme("document-new", QIcon(":/assets/trim.png"));
    trimAct = new QAction(trimIcon, tr("&Trim"), this);
    //connect(undoAct, &QAction::triggered, this, &MainWindow::importData);
    mainToolBar->addAction(trimAct);

    mainToolBar->addSeparator();

    const QIcon waveIcon = QIcon::fromTheme("document-new", QIcon(":/assets/waveform.png"));
    waveAct = new QAction(waveIcon, tr("&Waveform"), this);
    //connect(undoAct, &QAction::triggered, this, &MainWindow::importData);
    mainToolBar->addAction(waveAct);

}

void MainWindow::createDockWindows()
{
    QDockWidget *dock = new QDockWidget(tr("File"), this);
    dock->setAllowedAreas(Qt::LeftDockWidgetArea);
    dock->setFeatures(QDockWidget::DockWidgetFloatable);
    fileList = new QListWidget(dock);
    dock->setWidget(fileList);
    addDockWidget(Qt::LeftDockWidgetArea, dock);
    fileList->setSelectionMode(QAbstractItemView::ExtendedSelection);
    fileList->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(fileList, SIGNAL(itemSelectionChanged()),
            this, SLOT(fileListItemSelectionChanged()));
    connect(fileList, SIGNAL(customContextMenuRequested(const QPoint &)),
            this, SLOT(fileListContextMenu(const QPoint &)));


    dock = new QDockWidget(tr("Data"), this);
    dock->setAllowedAreas(Qt::LeftDockWidgetArea);
    dock->setFeatures(QDockWidget::DockWidgetFloatable);
    columnList = new QListWidget(dock);
    dock->setWidget(columnList);
    addDockWidget(Qt::LeftDockWidgetArea, dock);
    columnList->setSelectionMode(QAbstractItemView::ExtendedSelection);
    columnList->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(columnList, SIGNAL(itemSelectionChanged()),
            this, SLOT(signalListItemSelectionChanged()));
    connect(columnList, SIGNAL(customContextMenuRequested(const QPoint &)),
            this, SLOT(signalListContextMenu(const QPoint &)));
}

bool MainWindow::initGraphContext()
{
    QWidget *widget = new QWidget;

    setCentralWidget(widget);

    return true;
}

void MainWindow::import()
{
    QString filename = QFileDialog::getOpenFileName(this,tr("Open CSV"), QDir::currentPath(),tr("Text Files (*.txt *.csv)"));
    qDebug()<<filename;

    if(dataFile.isOpen())
        dataFile.close();
    dataFile.setFileName(filename);

    if (filename.isEmpty() || !dataFile.open(QFile::ReadOnly))
    {
        statusBar()->showMessage(QString(tr("Open file canceled or failed")));
        return;
    }

    ImportDialog dialog;
    dialog.setWindowTitle(QString(tr("Import")));
    if(dialog.exec() != QDialog::Accepted)
        return;

    QFileInfo info(dataFile);
    qDebug()<<info.fileName();
    fileList->addItem(info.fileName());

    statusBar()->showMessage(QString(tr("Start importing ...")));
    future = QtConcurrent::run(this,&MainWindow::importThread);

    future.waitForFinished();
    statusBar()->showMessage(QString(tr("Data imported")));

    if(dataFile.isOpen())
        dataFile.close();
}

void MainWindow::importThread()
{
    dataList.append(dataFile.readAll());
    dataFile.seek(0);
    QByteArray line = dataFile.readLine();
    QString header(line);


    columnList->clear();
    columnList->addItems(header.simplified().split(','));
}

void MainWindow::showAbout()
{
    Dialog  m_dialog;
    QString msg;
    QTextStream in(&msg);
    in<<QApplication::applicationName()<<endl;
    in<<"Author: "<<"Zeyang Dai"<<endl;
    in<<"Date: 2019-02-24"<<endl;
    in<<endl;
    in<<"Operation Flows:"<<endl;
    in<<"\t1. Import data from Text or CSV file, each line of which is in this format: x, y, z;"<<endl;
    in<<"\t2. Click the remove button to delete the abnormal points;"<<endl;
    in<<"\t3. [Opetional] Click the export button to save datas in the left graph;"<<endl;
    in<<"\t4. Run the calibration and a dialog will show the results;"<<endl;
    in<<"\t5. Save the expected parameters by pressing the save button."<<endl;
    in<<endl;
    in<<"Citation:"<<endl;
    in<<"\t1. https://github.com/paynterf/MagCalTool"<<endl;
    in<<"\t2. https://www.mathworks.com/matlabcentral/fileexchange/23398-magnetometers-calibration"<<endl;
    in<<"\t3. http://eigen.tuxfamily.org/index.php?title=Main_Page"<<endl;

    m_dialog.setWindowTitle(QString("About"));
    m_dialog.setMsg(msg);
    m_dialog.exec();
}

void MainWindow::fileListItemSelectionChanged()
{
    qDebug()<<"file list selection changed";
}

void MainWindow::fileListContextMenu(const QPoint &pos)
{
    QPoint item = fileList->mapToGlobal(pos);
    QMenu submenu;
    submenu.addAction("ADD");
    submenu.addAction("Delete");
    QAction* rightClickItem = submenu.exec(item);
    if (rightClickItem && rightClickItem->text().contains("Delete") )
    {
        //ui->listFiles->takeItem(ui->listFiles->indexAt(pos).row());
    }
}

void MainWindow::signalListItemSelectionChanged()
{
    qDebug()<<"signal list selection changed";
}

void MainWindow::signalListContextMenu(const QPoint &pos)
{
    QPoint item = columnList->mapToGlobal(pos);
    QMenu submenu;
    submenu.addAction("ADD");
    submenu.addAction("Delete");
    QAction* rightClickItem = submenu.exec(item);
    if (rightClickItem && rightClickItem->text().contains("Delete") )
    {
        //ui->listFiles->takeItem(ui->listFiles->indexAt(pos).row());
    }
}
