#ifndef FINDDIALOG_H
#define FINDDIALOG_H

#include <QDialog>

namespace Ui {
class FindDialog;
}

// 非模态查找对话框，查找条件通过信号交给主窗口统一处理
class FindDialog : public QDialog
{
    Q_OBJECT

public:
    explicit FindDialog(QWidget *parent = nullptr);
    ~FindDialog() override;

    void setFindText(const QString &text);

signals:
    void findRequested(const QString &text, bool backward, bool matchCase);

protected:
    void showEvent(QShowEvent *event) override;

private slots:
    void emitFindRequest();

private:
    Ui::FindDialog *ui;
};

#endif // FINDDIALOG_H
