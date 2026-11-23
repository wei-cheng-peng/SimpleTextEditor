#include "aboutdialog.h"
#include "ui_aboutdialog.h"

#include <QDialogButtonBox>
#include <QStyle>

AboutDialog::AboutDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::AboutDialog)
{
    ui->setupUi(this);

    // 程序图标直接用系统标准图标，避免额外图片资源
    ui->logoLabel->setPixmap(style()->standardIcon(QStyle::SP_FileIcon).pixmap(96, 96));

    connect(ui->buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
}

AboutDialog::~AboutDialog()
{
    delete ui;
}
