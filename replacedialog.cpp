#include "replacedialog.h"
#include "ui_replacedialog.h"

#include <QMessageBox>

ReplaceDialog::ReplaceDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::ReplaceDialog)
{
    ui->setupUi(this);

    setModal(false);

    connect(ui->findNextButton, &QPushButton::clicked, this, &ReplaceDialog::emitFindRequest);
    connect(ui->replaceButton, &QPushButton::clicked, this, &ReplaceDialog::emitReplaceRequest);
    connect(ui->replaceAllButton, &QPushButton::clicked, this, &ReplaceDialog::emitReplaceAllRequest);
    connect(ui->cancelButton, &QPushButton::clicked, this, &QDialog::close);
    connect(ui->findLineEdit, &QLineEdit::returnPressed, this, &ReplaceDialog::emitFindRequest);

    // 默认的 Tab 顺序会先跳到按钮上，这里调整成“先填完两个输入框再操作按钮”
    setTabOrder(ui->findLineEdit, ui->replaceLineEdit);
    setTabOrder(ui->replaceLineEdit, ui->replaceButton);
    setTabOrder(ui->replaceButton, ui->replaceAllButton);
    setTabOrder(ui->replaceAllButton, ui->findNextButton);
    setTabOrder(ui->findNextButton, ui->caseCheckBox);
    setTabOrder(ui->caseCheckBox, ui->upRadio);
    setTabOrder(ui->upRadio, ui->downRadio);
    setTabOrder(ui->downRadio, ui->cancelButton);
}

ReplaceDialog::~ReplaceDialog()
{
    delete ui;
}

void ReplaceDialog::setFindText(const QString &text)
{
    if (!text.isEmpty())
        ui->findLineEdit->setText(text);
}

bool ReplaceDialog::checkTarget(const QString &title)
{
    if (!ui->findLineEdit->text().isEmpty())
        return true;

    QMessageBox::information(this, title, QStringLiteral("请输入要查找的内容。"));
    ui->findLineEdit->setFocus();
    return false;
}

void ReplaceDialog::emitFindRequest()
{
    if (!checkTarget(QStringLiteral("查找")))
        return;

    emit findRequested(ui->findLineEdit->text(),
                       ui->upRadio->isChecked(),
                       ui->caseCheckBox->isChecked());
}

void ReplaceDialog::emitReplaceRequest()
{
    if (!checkTarget(QStringLiteral("替换")))
        return;

    emit replaceRequested(ui->findLineEdit->text(),
                          ui->replaceLineEdit->text(),
                          ui->caseCheckBox->isChecked());
}

void ReplaceDialog::emitReplaceAllRequest()
{
    if (!checkTarget(QStringLiteral("替换")))
        return;

    emit replaceAllRequested(ui->findLineEdit->text(),
                             ui->replaceLineEdit->text(),
                             ui->caseCheckBox->isChecked());
}

void ReplaceDialog::showEvent(QShowEvent *event)
{
    QDialog::showEvent(event);

    ui->findLineEdit->setFocus();
    ui->findLineEdit->selectAll();
}
