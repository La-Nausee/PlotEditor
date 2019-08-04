#include "dialog.h"
#include "ui_dialog.h"
#include <QHBoxLayout>

Dialog::Dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog)
{
    ui->setupUi(this);  

    textEdit = new QPlainTextEdit;
    textEdit->setReadOnly(true);

    QHBoxLayout *mainLayout = new QHBoxLayout;
    mainLayout->addWidget(textEdit);

    this->setLayout(mainLayout);
}

Dialog::~Dialog()
{
    delete ui;
}

void Dialog::setMsg(const QString &text)
{
    textEdit->setPlainText(text);
}
