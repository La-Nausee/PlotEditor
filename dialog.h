#ifndef DIALOG_H
#define DIALOG_H

#include <QDialog>
#include <QPlainTextEdit>

namespace Ui {
class Dialog;
}

class Dialog : public QDialog
{
    Q_OBJECT

public:
    explicit Dialog(QWidget *parent = nullptr);
    ~Dialog();

    void setMsg(const QString &text);

private:
    Ui::Dialog *ui;

    QPlainTextEdit *textEdit;
};

#endif // DIALOG_H
