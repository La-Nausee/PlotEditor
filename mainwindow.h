#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtDataVisualization>
#include <QList>
#include <QListWidget>
#include <QListWidgetItem>
#include <QByteArray>
#include <QTemporaryFile>

#define CSV_DELIMITER_TAB               '\t'
#define CSV_DELIMITER_COMMA             ','
#define CSV_DELIMITER_SEMICOLON         ';'

using namespace QtDataVisualization;

namespace Ui {
class MainWindow;
}

enum EVENT{
    EVENT_GENERAL_UNDO,
    EVENT_GENERAL_REDO,
    EVENT_FILE_IMPORT,
    EVENT_FILE_EXPORT,
    EVENT_FILE_COPY,
    EVENT_FILE_PASTE,
    EVENT_FILE_MERGE,
    EVENT_FILE_DELETE,
    EVENT_COLUMN_CREATE,
    EVENT_COLUMN_EXPORT,
    EVENT_COLUMN_COPY,
    EVENT_COLUMN_PASTE,
    EVENT_COLUMN_DELETE,
    EVENT_PLOT_CUT,
    EVENT_PLOT_COPY,
    EVENT_PLOT_PASTE,
    EVENT_PLOT_MERGE,
    EVENT_PLOT_DELETE,
    EVENT_PLOT_TRIM,
    EVENT_PLOT_UPDATE
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void createActions();
    void createDockWindows();
    bool initGraphContext();
    void eventProcessor();
private slots:
    void import();
    void showAbout();
    void fileListItemDoubleClicked(QListWidgetItem *item);
    void fileListContextMenu(const QPoint &);
    void columnListItemDoubleClicked(QListWidgetItem *item);
    void columnListContextMenu(const QPoint &);
private:
    bool withHeader;

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
    QAction *mergeAct;
    QAction *deleteAct;
    QAction *trimAct;
    QAction *aboutAct;

    int currentRow;
    QListWidgetItem* currentIterm;
    QList<QListWidgetItem *>  selectedIterms;
    QList<QListWidgetItem *>  fileClipboard;
    QList<QListWidgetItem *>  columnClipboard;
    QListWidget *fileList;
    QListWidget *columnList;

    enum EVENT event;
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
