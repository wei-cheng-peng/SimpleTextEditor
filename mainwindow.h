#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTextCharFormat>

class QLabel;
class QTimer;
class FindDialog;
class ReplaceDialog;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

    // 打开指定路径的文件（供命令行参数使用）
    bool loadFromFile(const QString &filePath);

protected:
    void closeEvent(QCloseEvent *event) override;

private:
    // ---------- 文件操作 ----------
    void newFile();
    void openFile();
    bool saveFile();
    bool saveFileAs();
    bool writeToFile(const QString &filePath);
    bool maybeSave();
    void setCurrentFile(const QString &filePath);

    // ---------- 格式设置 ----------
    void changeFont();
    void changeFontColor();
    void changeFontBackground();
    void changeEditorBackground();
    void applyCharFormat(const QTextCharFormat &format);

    // ---------- 查找与替换 ----------
    void showFindDialog();
    void showReplaceDialog();
    void findNext(const QString &text, bool backward, bool matchCase);
    void replaceOnce(const QString &text, const QString &replacement, bool matchCase);
    void replaceAll(const QString &text, const QString &replacement, bool matchCase);
    bool selectionEquals(const QString &text, bool matchCase) const;

    // ---------- 界面 ------------------
    void setupIcons();
    void setupStatusBar();
    void setupConnections();
    void updateWindowTitle();
    void updateStatusBar();
    void updateEditActionStates();
    void updatePasteActionState();
    void autoSave();

    Ui::MainWindow *ui;

    FindDialog    *m_findDialog    = nullptr;
    ReplaceDialog *m_replaceDialog = nullptr;
    QLabel *m_infoLabel   = nullptr;
    QLabel *m_authorLabel = nullptr;
    QTimer *m_autoSaveTimer = nullptr;

    QString m_currentFile;              // 当前文件名，未保存过则为空
    static const int AutoSaveIntervalMs = 60000;   // 自动保存检查周期
};

#endif // MAINWINDOW_H
