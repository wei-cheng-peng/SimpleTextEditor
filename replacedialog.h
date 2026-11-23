#ifndef REPLACEDIALOG_H
#define REPLACEDIALOG_H

#include <QDialog>

namespace Ui {
class ReplaceDialog;
}

// 非模态替换对话框：查找、替换、全部替换三种操作分别发信号
class ReplaceDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ReplaceDialog(QWidget *parent = nullptr);
    ~ReplaceDialog() override;

    void setFindText(const QString &text);

signals:
    void findRequested(const QString &text, bool backward, bool matchCase);
    void replaceRequested(const QString &text, const QString &replacement, bool matchCase);
    void replaceAllRequested(const QString &text, const QString &replacement, bool matchCase);

protected:
    void showEvent(QShowEvent *event) override;

private slots:
    void emitFindRequest();
    void emitReplaceRequest();
    void emitReplaceAllRequest();
    // 没有输入查找内容时给出提示，返回 false 表示不应继续
    bool checkTarget(const QString &title);

private:
    Ui::ReplaceDialog *ui;
};

#endif // REPLACEDIALOG_H
