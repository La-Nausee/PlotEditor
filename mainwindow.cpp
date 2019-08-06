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
    undoAct->setShortcut(QKeySequence::Undo);
    //connect(undoAct, &QAction::triggered, this, &MainWindow::importData);
    mainToolBar->addAction(undoAct);
    
    const QIcon redoIcon = QIcon::fromTheme("document-new", QIcon(":/assets/redo.png"));
    redoAct = new QAction(redoIcon, tr("&Redo"), this);
    redoAct->setShortcut(QKeySequence::Redo);
    mainToolBar->addAction(redoAct);
    //connect(undoAct, &QAction::triggered, this, &MainWindow::importData);

    const QIcon cutIcon = QIcon::fromTheme("document-new", QIcon(":/assets/cut.png"));
    cutAct = new QAction(cutIcon, tr("&Cut"), this);
    cutAct->setShortcut(QKeySequence::Cut);
    //connect(undoAct, &QAction::triggered, this, &MainWindow::importData);
    mainToolBar->addAction(cutAct);

    const QIcon copyIcon = QIcon::fromTheme("document-new", QIcon(":/assets/copy.png"));
    copyAct = new QAction(copyIcon, tr("&Copy"), this);
    copyAct->setShortcut(QKeySequence::Copy);
    //connect(undoAct, &QAction::triggered, this, &MainWindow::importData);
    mainToolBar->addAction(copyAct);

    const QIcon pasteIcon = QIcon::fromTheme("document-new", QIcon(":/assets/paste.png"));
    pasteAct = new QAction(pasteIcon, tr("&Paste"), this);
    pasteAct->setShortcut(QKeySequence::Paste);
    //connect(undoAct, &QAction::triggered, this, &MainWindow::importData);
    mainToolBar->addAction(pasteAct);

    const QIcon mergeIcon = QIcon::fromTheme("document-new", QIcon(":/assets/merge.png"));
    mergeAct = new QAction(mergeIcon, tr("&Merge"), this);
//    pasteAct->setShortcut(QKeySequence::);
    //connect(undoAct, &QAction::triggered, this, &MainWindow::importData);
    mainToolBar->addAction(mergeAct);
    
    const QIcon deleteIcon = QIcon::fromTheme("document-new", QIcon(":/assets/delete.png"));
    deleteAct = new QAction(deleteIcon, tr("&Delete"), this);
    deleteAct->setShortcut(QKeySequence::Delete);
    //connect(undoAct, &QAction::triggered, this, &MainWindow::importData);
    mainToolBar->addAction(deleteAct);

    const QIcon trimIcon = QIcon::fromTheme("document-new", QIcon(":/assets/trim.png"));
    trimAct = new QAction(trimIcon, tr("&Trim"), this);
    //connect(undoAct, &QAction::triggered, this, &MainWindow::importData);
    mainToolBar->addAction(trimAct);

    mainToolBar->addSeparator();

    const QIcon aboutIcon = QIcon::fromTheme("document-new", QIcon(":/assets/about.png"));
    aboutAct = new QAction(aboutIcon, tr("&About"), this);
    //connect(undoAct, &QAction::triggered, this, &MainWindow::importData);
    mainToolBar->addAction(aboutAct);
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
    connect(fileList, SIGNAL(itemDoubleClicked(QListWidgetItem *)),
            this, SLOT(fileListItemDoubleClicked(QListWidgetItem *)));
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
    connect(columnList, SIGNAL(itemDoubleClicked(QListWidgetItem *)),
            this, SLOT(columnListItemDoubleClicked(QListWidgetItem *)));
    connect(columnList, SIGNAL(customContextMenuRequested(const QPoint &)),
            this, SLOT(columnListContextMenu(const QPoint &)));
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
    dialog.setWindowTitle(QString(tr("Data import")));
    if(dialog.exec() == QDialog::Accepted)
        withHeader = true;
    else
        withHeader = false;
    
    QFileInfo info(dataFile);
    qDebug()<<info.fileName();
    fileList->addItem(info.fileName());

    statusBar()->showMessage(QString(tr("Start importing ...")));
    event = EVENT_FILE_IMPORT;
    future = QtConcurrent::run(this,&MainWindow::eventProcessor);

    future.waitForFinished();
    statusBar()->showMessage(QString(tr("Ready")));

    if(dataFile.isOpen())
        dataFile.close();
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

void MainWindow::fileListItemDoubleClicked(QListWidgetItem *item)
{
    columnList->clear();
    columnList->addItems(headerList.at(fileList->row(item)));
}
        
void MainWindow::fileListContextMenu(const QPoint &pos)
{
    currentIterm   = fileList->currentItem();
    currentRow = fileList->currentRow();
    selectedIterms = fileList->selectedItems();
    
    QPoint item = fileList->mapToGlobal(pos);
    QMenu submenu;
    submenu.addAction(tr("Import data from CSV file"));
    submenu.addAction(tr("Export data to CSV file"));
    submenu.addAction(tr("Copy"));
    if(!fileClipboard.isEmpty())
        submenu.addAction(tr("Paste"));
    if(selectedIterms.length() > 1)
        submenu.addAction(tr("Merge"));    
    submenu.addAction(tr("Delete"));
    
    QAction* rightClickItem = submenu.exec(item);
    if (!rightClickItem)
        return;
    if (rightClickItem->text().contains(tr("Import data from CSV file")) )
    {
        import();
    }
    else if (rightClickItem->text().contains(tr("Export data to CSV file")) )
    {
        event = EVENT_FILE_EXPORT;
    }
    else if (rightClickItem->text().contains(tr("Copy")) )
    {
        event = EVENT_FILE_COPY; 
    }
    else if (rightClickItem->text().contains(tr("Paste")) )
    {
        event = EVENT_FILE_PASTE; 
    }
    else if (rightClickItem->text().contains(tr("Delete")) )
    {
        event = EVENT_FILE_DELETE;
    }
    
    future = QtConcurrent::run(this,&MainWindow::eventProcessor);

    future.waitForFinished();
}

void MainWindow::columnListItemDoubleClicked(QListWidgetItem *item)
{
    qDebug()<<item->text();            
}

void MainWindow::columnListContextMenu(const QPoint &pos)
{
    currentIterm   = columnList->currentItem();
    currentRow = columnList->currentRow();
    selectedIterms = columnList->selectedItems();

    QPoint item = columnList->mapToGlobal(pos);
    QMenu submenu;
    submenu.addAction(tr("Create a new file"));
    submenu.addAction(tr("Export data to CSV file"));
    submenu.addAction(tr("Copy"));
    if(!columnClipboard.isEmpty())
        submenu.addAction(tr("Paste"));
    submenu.addAction(tr("Delete"));
    
    QAction* rightClickItem = submenu.exec(item);
    if (!rightClickItem)
        return;
    if (rightClickItem->text().contains(tr("Create a new file")) )
    {
        event = EVENT_COLUMN_CREATE;
    }
    else if (rightClickItem->text().contains(tr("Export data to CSV file")) )
    {
        event = EVENT_COLUMN_EXPORT;
    }
    else if (rightClickItem->text().contains(tr("Copy")) )
    {
        event = EVENT_COLUMN_COPY;
    }
    else if (rightClickItem->text().contains(tr("Paste")) )
    {
        event = EVENT_COLUMN_PASTE;
    }
    else if (rightClickItem->text().contains(tr("Delete")) )
    {
        event = EVENT_COLUMN_DELETE;
    }
    
    future = QtConcurrent::run(this,&MainWindow::eventProcessor);

    future.waitForFinished();
}

void MainWindow::eventProcessor()
{
    int index = 0;
    QString line;
    QStringList header, tmp;
    
    switch(event)
    {
    case EVENT_GENERAL_UNDO:
        break;
    case EVENT_GENERAL_REDO:
        break;
    case EVENT_FILE_IMPORT:
        dataList.append(dataFile.readAll());
        dataFile.seek(0);
        line.clear();
        line.append(dataFile.readLine());
        
        header<<line.simplified();
        
        if(line.contains(CSV_DELIMITER_TAB))
        {
            tmp = line.split(CSV_DELIMITER_TAB);
            header.clear();
            foreach(const QString &s,tmp)
            {
                header << s.simplified();
            }
        }
        
        if(line.contains(CSV_DELIMITER_COMMA))
        {
            tmp = header;
            tmp.detach();
            header.clear();
            foreach(const QString &s,tmp)
            {
                header << s.split(CSV_DELIMITER_COMMA);
            }
        }
    
        if(line.contains(CSV_DELIMITER_SEMICOLON))
        {
            tmp = header;
            tmp.detach();
            header.clear();
            foreach(const QString &s,tmp)
            {
                header << s.split(CSV_DELIMITER_SEMICOLON);
            }
        }
    
        tmp = header;
        tmp.detach();
        header.clear();
        index = 1;
        foreach(const QString &s,tmp)
        {
            if(s.isEmpty())
                header<<QString(tr("Row"))+QString::number(index);
            else
                header<<s;
            index++;
        }
        
        if(!withHeader)
        {
            tmp = header;
            tmp.detach();
            header.clear();
            index = 1;
            foreach(const QString &s,tmp)
            {
                Q_UNUSED(s)
                header<<QString(tr("Row"))+QString::number(index++);
            }
        }
        
        columnList->clear();
        columnList->addItems(header);
        headerList.append(header);
        break;
    case EVENT_FILE_EXPORT:
        break;
    case EVENT_FILE_COPY:
        fileClipboard.clear();
        fileClipboard = selectedIterms;
        break;
    case EVENT_FILE_PASTE:
        foreach(QListWidgetItem* item, fileClipboard)
        {
            index = fileList->row(item);
            QByteArray data = dataList.at(index);
            data.detach();
            dataList.append(data);
            QStringList header = headerList.at(index);
            headerList.append(header);
            fileList->addItem(QString(item->text()));
            currentRow++;
        }
        break;
    case EVENT_FILE_MERGE:
        break;
    case EVENT_FILE_DELETE:
        foreach(QListWidgetItem* item, selectedIterms)
        {
            index = fileList->row(item);
            dataList.removeAt(index);
            headerList.removeAt(index);
            fileList->removeItemWidget(item);
            delete item; // Qt documentation warnings you to destroy item to effectively remove it from QListWidget.
        }
        break;
    case EVENT_COLUMN_CREATE:
        break;
    case EVENT_COLUMN_EXPORT:
        break;
    case EVENT_COLUMN_COPY:
        columnClipboard.clear();
        columnClipboard = selectedIterms;
        break;
    case EVENT_COLUMN_PASTE:
        break;
    case EVENT_COLUMN_DELETE:
        foreach(QListWidgetItem* item, selectedIterms)
        {
//            index = fileList->row(item);
//            dataList.removeAt(index);
//            headerList.removeAt(index);
            columnList->removeItemWidget(item);
            delete item; // Qt documentation warnings you to destroy item to effectively remove it from QListWidget.
        }
        break;
    case EVENT_PLOT_CUT:
        break;
    case EVENT_PLOT_COPY:
        break;
    case EVENT_PLOT_PASTE:
        break;
    case EVENT_PLOT_MERGE:
        break;
    case EVENT_PLOT_DELETE:
        break;
    case EVENT_PLOT_TRIM:
        break;
    case EVENT_PLOT_UPDATE:
        break;
    }
}
