#include "finddialog.h"
#include "ui_finddialog.h"

FindDialog::FindDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::FindDialog)
{
    ui->setupUi(this);

    // 查找对话框不阻塞主窗口，用户可以边看结果边修改文档
    setModal(false);

    connect(ui->findNextButton, &QPushButton::clicked, this, &FindDialog::emitFindRequest);
    connect(ui->findLineEdit, &QLineEdit::returnPressed, this, &FindDialog::emitFindRequest);
    connect(ui->cancelButton, &QPushButton::clicked, this, &QDialog::close);

    setTabOrder(ui->findLineEdit, ui->findNextButton);
    setTabOrder(ui->findNextButton, ui->caseCheckBox);
    setTabOrder(ui->caseCheckBox, ui->upRadio);
    setTabOrder(ui->upRadio, ui->downRadio);
    setTabOrder(ui->downRadio, ui->cancelButton);
}

FindDialog::~FindDialog()
{
    delete ui;
}

void FindDialog::setFindText(const QString &text)
{
    if (!text.isEmpty())
        ui->findLineEdit->setText(text);
}

void FindDialog::emitFindRequest()
{
    emit findRequested(ui->findLineEdit->text(),
                       ui->upRadio->isChecked(),
                       ui->caseCheckBox->isChecked());
}

void FindDialog::showEvent(QShowEvent *event)
{
    QDialog::showEvent(event);

    // 弹出后自动聚焦到输入框并选中已有内容，方便直接改写
    ui->findLineEdit->setFocus();
    ui->findLineEdit->selectAll();
}
