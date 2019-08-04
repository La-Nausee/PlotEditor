#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtDataVisualization>
#include <QListWidget>
#include <QList>
#include <QByteArray>

#define CSV_DELIMITER   ','

using namespace QtDataVisualization;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void createActions();
    void createDockWindows();
    bool initGraphContext();
    void importThread();
private slots:
    void import();
    void showAbout();
    void fileListItemSelectionChanged();
    void fileListContextMenu(const QPoint &);
    void signalListItemSelectionChanged();
    void signalListContextMenu(const QPoint &);
private:
    char delimiter;
    bool withHeader;
    qint64 skipHeader;
    QFile dataFile;
    QList<QByteArray> dataList;
    QList<QString> headerList;
    QFuture<void> future;

    QAction *importAct;
    QAction *undoAct;
    QAction *cutAct;
    QAction *copyAct;
    QAction *pasteAct;
    QAction *deleteAct;
    QAction *trimAct;
    QAction *waveAct;

    QListWidget *fileList;
    QListWidget *columnList;

    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
