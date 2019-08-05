#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtDataVisualization>
#include <QListWidget>
#include <QList>
#include <QByteArray>
#include <QTemporaryFile>

#define CSV_DELIMITER_TAB               '\t'
#define CSV_DELIMITER_COMMA             ','
#define CSV_DELIMITER_SEMICOLON         ';'

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
    bool withHeader;
    qint64 m_index;
    QFile dataFile;
    QList<QByteArray> dataList;
    QList<QStringList> headerList;
    QFuture<void> future;

    QAction *importAct;
    QAction *undoAct;
    QAction *redoAct;
    QAction *cutAct;
    QAction *copyAct;
    QAction *pasteAct;
    QAction *deleteAct;
    QAction *trimAct;
    QAction *aboutAct;

    QListWidget *fileList;
    QListWidget *columnList;

    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
