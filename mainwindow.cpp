#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "codeeditor.h"
#include "finddialog.h"
#include "replacedialog.h"
#include "aboutdialog.h"

#include <QAction>
#include <QApplication>
#include <QClipboard>
#include <QCloseEvent>
#include <QColorDialog>
#include <QFile>
#include <QFileDialog>
#include <QFileInfo>
#include <QFontDialog>
#include <QIcon>
#include <QLabel>
#include <QMessageBox>
#include <QMimeData>
#include <QPolygonF>
#include <QPainter>
#include <QPixmap>
#include <QStatusBar>
#include <QStyle>
#include <QtMath>
#include <QTextCursor>
#include <QTextDocument>
#include <QTextStream>
#include <QTimer>
#include <QToolBar>

namespace {

const QColor kIconPen(0x5A, 0x5A, 0x5A);

QPixmap newIconPixmap()
{
    QPixmap pixmap(32, 32);
    pixmap.fill(Qt::transparent);
    return pixmap;
}

void drawArrowHead(QPainter &painter, const QPointF &tip, qreal angleDeg)
{
    const qreal size = 5.0;
    const qreal rad = qDegreesToRadians(angleDeg);
    QPolygonF head;
    head << tip
         << tip + QPointF(size * qCos(rad + 0.45), size * qSin(rad + 0.45))
         << tip + QPointF(size * qCos(rad - 0.45), size * qSin(rad - 0.45));
    painter.setBrush(QColor(kIconPen));
    painter.drawPolygon(head);
    painter.setBrush(Qt::NoBrush);
}

// 剪刀：剪切
QIcon makeCutIcon()
{
    QPixmap pixmap = newIconPixmap();
    QPainter p(&pixmap);
    p.setRenderHint(QPainter::Antialiasing);
    p.setPen(QPen(kIconPen, 2));
    p.drawLine(QPointF(10, 9), QPointF(26, 25));
    p.drawLine(QPointF(10, 25), QPointF(26, 9));
    p.setBrush(Qt::NoBrush);
    p.drawEllipse(QPointF(7, 7), 3.2, 3.2);
    p.drawEllipse(QPointF(7, 27), 3.2, 3.2);
    p.end();
    return QIcon(pixmap);
}

// 两张叠放的纸：复制
QIcon makeCopyIcon()
{
    QPixmap pixmap = newIconPixmap();
    QPainter p(&pixmap);
    p.setRenderHint(QPainter::Antialiasing);
    p.setPen(QPen(kIconPen, 1.6));
    p.setBrush(Qt::white);
    p.drawRect(QRectF(6, 9, 15, 19));
    p.drawRect(QRectF(11, 4, 15, 19));
    p.end();
    return QIcon(pixmap);
}

// 写字板：粘贴
QIcon makePasteIcon()
{
    QPixmap pixmap = newIconPixmap();
    QPainter p(&pixmap);
    p.setRenderHint(QPainter::Antialiasing);
    p.setPen(QPen(kIconPen, 1.6));
    p.setBrush(Qt::white);
    p.drawRect(QRectF(7, 8, 18, 20));
    p.setBrush(QColor(0xD8, 0xE4, 0xF2));
    p.drawRect(QRectF(12, 4, 8, 6));
    p.setPen(QPen(kIconPen, 1.2));
    p.drawLine(QPointF(10, 16), QPointF(22, 16));
    p.drawLine(QPointF(10, 20), QPointF(22, 20));
    p.drawLine(QPointF(10, 24), QPointF(18, 24));
    p.end();
    return QIcon(pixmap);
}

// 放大镜：查找
QIcon makeFindIcon()
{
    QPixmap pixmap = newIconPixmap();
    QPainter p(&pixmap);
    p.setRenderHint(QPainter::Antialiasing);
    p.setPen(QPen(kIconPen, 2));
    p.setBrush(Qt::white);
    p.drawEllipse(QPointF(13, 13), 7.0, 7.0);
    p.drawLine(QPointF(18, 18), QPointF(26, 26));
    p.end();
    return QIcon(pixmap);
}

// 放大镜 + 切换箭头：替换
QIcon makeReplaceIcon()
{
    QPixmap pixmap = newIconPixmap();
    QPainter p(&pixmap);
    p.setRenderHint(QPainter::Antialiasing);
    p.setPen(QPen(kIconPen, 1.8));
    p.setBrush(Qt::white);
    p.drawEllipse(QPointF(12, 12), 6.0, 6.0);
    p.drawLine(QPointF(16.5, 16.5), QPointF(22, 22));
    p.setPen(QPen(QColor(0x1F, 0x6E, 0xC0), 1.8));
    p.drawLine(QPointF(18, 8), QPointF(27, 8));
    drawArrowHead(p, QPointF(27, 8), 0);
    p.end();
    return QIcon(pixmap);
}

// 折行的箭头：自动换行
QIcon makeWordWrapIcon()
{
    QPixmap pixmap = newIconPixmap();
    QPainter p(&pixmap);
    p.setRenderHint(QPainter::Antialiasing);
    p.setPen(QPen(kIconPen, 1.8));
    p.drawLine(QPointF(5, 8), QPointF(27, 8));
    p.drawLine(QPointF(5, 15), QPointF(20, 15));
    p.drawLine(QPointF(5, 22), QPointF(27, 22));
    p.drawLine(QPointF(20, 15), QPointF(20, 24));
    drawArrowHead(p, QPointF(20, 24), 90);
    p.end();
    return QIcon(pixmap);
}

// 行号：数字提示
QIcon makeLineNumberIcon()
{
    QPixmap pixmap = newIconPixmap();
    QPainter p(&pixmap);
    p.setRenderHint(QPainter::TextAntialiasing);
    p.fillRect(QRect(4, 3, 24, 26), QColor(0xF3, 0xF5, 0xF8));
    p.setPen(QColor(0x66, 0x66, 0x66));
    QFont font(QStringLiteral("Arial"));
    font.setPixelSize(19);
    font.setBold(true);
    p.setFont(font);
    p.drawText(QRect(4, 3, 24, 26), Qt::AlignCenter, QStringLiteral("123"));
    p.end();
    return QIcon(pixmap);
}

// 用文字与色块绘制“字体/颜色”类图标，避免依赖外部图片资源
QIcon makeLetterIcon(const QString &letter, const QColor &letterColor,
                     const QColor &swatch = QColor(), bool fillBehind = false)
{
    QPixmap pixmap = newIconPixmap();

    QPainter painter(&pixmap);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setRenderHint(QPainter::TextAntialiasing);

    if (fillBehind)
        painter.fillRect(QRect(3, 3, 26, 26), swatch);
    if (swatch.isValid() && !fillBehind)
        painter.fillRect(QRect(4, 25, 24, 5), swatch);
    if (fillBehind) {
        painter.setPen(QColor(0x80, 0x80, 0x80));
        painter.drawRect(QRect(3, 3, 26, 26));
    }

    QFont font(QStringLiteral("Times New Roman"));
    font.setPixelSize(22);
    font.setBold(true);
    painter.setFont(font);
    painter.setPen(letterColor);
    painter.drawText(pixmap.rect(), Qt::AlignCenter, letter);
    painter.end();

    return QIcon(pixmap);
}

} // namespace

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    setupIcons();
    setupStatusBar();
    setupConnections();

    // 编辑器默认字体：等宽字体，便于观察行号与缩进
    QFont defaultFont(QStringLiteral("Consolas"));
    defaultFont.setPointSize(11);
    defaultFont.setStyleHint(QFont::Monospace);
    ui->textEdit->document()->setDefaultFont(defaultFont);

    // 自动保存：定时检查文档是否被修改，已命名的文件自动写回
    m_autoSaveTimer = new QTimer(this);
    connect(m_autoSaveTimer, &QTimer::timeout, this, &MainWindow::autoSave);
    m_autoSaveTimer->start(AutoSaveIntervalMs);

    updateWindowTitle();
    updateStatusBar();
    updateEditActionStates();
    updatePasteActionState();

}

MainWindow::~MainWindow()
{
    delete ui;
}

// --------------------------------------------------------------------- 初始化
void MainWindow::setupIcons()
{
    // 统一使用 QStyle 提供的标准图标，程序不依赖额外的图片文件
    ui->actionNew->setIcon(style()->standardIcon(QStyle::SP_FileIcon));
    ui->actionOpen->setIcon(style()->standardIcon(QStyle::SP_DialogOpenButton));
    ui->actionSave->setIcon(style()->standardIcon(QStyle::SP_DialogSaveButton));
    ui->actionSaveAs->setIcon(style()->standardIcon(QStyle::SP_DialogSaveButton));

    ui->actionUndo->setIcon(style()->standardIcon(QStyle::SP_ArrowBack));
    ui->actionRedo->setIcon(style()->standardIcon(QStyle::SP_ArrowForward));
    ui->actionCut->setIcon(makeCutIcon());
    ui->actionCopy->setIcon(makeCopyIcon());
    ui->actionPaste->setIcon(makePasteIcon());

    ui->actionFind->setIcon(makeFindIcon());
    ui->actionReplace->setIcon(makeReplaceIcon());
    ui->actionWordWrap->setIcon(makeWordWrapIcon());
    ui->actionLineNumber->setIcon(makeLineNumberIcon());

    ui->actionFont->setIcon(makeLetterIcon(QStringLiteral("A"), QColor(0x1F, 0x4E, 0x9C)));
    ui->actionFontColor->setIcon(makeLetterIcon(QStringLiteral("A"), QColor(0x1F, 0x4E, 0x9C), QColor(0xE0, 0x20, 0x20)));
    ui->actionFontBackground->setIcon(makeLetterIcon(QStringLiteral("A"), QColor(0x1F, 0x4E, 0x9C), QColor(0xFF, 0xE0, 0x60), true));
    ui->actionEditorBackground->setIcon(makeLetterIcon(QStringLiteral("ab"), QColor(0x33, 0x33, 0x33), QColor(0xCF, 0xE3, 0xF5), true));

    ui->actionToolBar->setIcon(style()->standardIcon(QStyle::SP_ToolBarHorizontalExtensionButton));
    ui->actionStatusBar->setIcon(style()->standardIcon(QStyle::SP_FileDialogInfoView));
    ui->actionAbout->setIcon(style()->standardIcon(QStyle::SP_MessageBoxInformation));
}

void MainWindow::setupStatusBar()
{
    m_infoLabel = new QLabel(this);
    m_authorLabel = new QLabel(QStringLiteral("彭炜程   2023414290124"), this);

    statusBar()->addPermanentWidget(m_infoLabel);
    statusBar()->addPermanentWidget(m_authorLabel);
}

void MainWindow::setupConnections()
{
    // -------- 文件 --------
    connect(ui->actionNew,    &QAction::triggered, this, &MainWindow::newFile);
    connect(ui->actionOpen,   &QAction::triggered, this, &MainWindow::openFile);
    connect(ui->actionSave,   &QAction::triggered, this, [this] { saveFile(); });
    connect(ui->actionSaveAs, &QAction::triggered, this, [this] { saveFileAs(); });
    connect(ui->actionExit,   &QAction::triggered, this, &MainWindow::close);

    // ------------- 编辑 --------
    connect(ui->actionUndo, &QAction::triggered, ui->textEdit, &QPlainTextEdit::undo);
    connect(ui->actionRedo, &QAction::triggered, ui->textEdit, &QPlainTextEdit::redo);
    connect(ui->actionCut,  &QAction::triggered, ui->textEdit, &QPlainTextEdit::cut);
    connect(ui->actionCopy, &QAction::triggered, ui->textEdit, &QPlainTextEdit::copy);
    connect(ui->actionPaste, &QAction::triggered, ui->textEdit, &QPlainTextEdit::paste);
    connect(ui->actionSelectAll, &QAction::triggered, ui->textEdit, &QPlainTextEdit::selectAll);

    // 撤销/恢复是否可用由文档状态决定
    connect(ui->textEdit->document(), &QTextDocument::undoAvailable,
            ui->actionUndo, &QAction::setEnabled);
    connect(ui->textEdit->document(), &QTextDocument::redoAvailable,
            ui->actionRedo, &QAction::setEnabled);

    // 剪切/复制只有存在选中内容时才可用
    connect(ui->textEdit, &QPlainTextEdit::copyAvailable,
            this, &MainWindow::updateEditActionStates);

    // 剪贴板内容变化时刷新“粘贴”的使能状态
    connect(QGuiApplication::clipboard(), &QClipboard::dataChanged,
            this, &MainWindow::updatePasteActionState);

    ui->actionUndo->setEnabled(false);
    ui->actionRedo->setEnabled(false);

    // -------- 查找与替换 -------------
    connect(ui->actionFind,    &QAction::triggered, this, &MainWindow::showFindDialog);
    connect(ui->actionReplace, &QAction::triggered, this, &MainWindow::showReplaceDialog);

    // -------- 格式 --------
    connect(ui->actionFont,           &QAction::triggered, this, &MainWindow::changeFont);
    connect(ui->actionFontColor,      &QAction::triggered, this, &MainWindow::changeFontColor);
    connect(ui->actionFontBackground, &QAction::triggered, this, &MainWindow::changeFontBackground);
    connect(ui->actionEditorBackground, &QAction::triggered, this, &MainWindow::changeEditorBackground);

    connect(ui->actionWordWrap, &QAction::toggled, this, [this](bool checked) {
        ui->textEdit->setLineWrapMode(checked ? QPlainTextEdit::WidgetWidth
                                              : QPlainTextEdit::NoWrap);
        statusBar()->showMessage(checked ? QStringLiteral("已开启自动换行")
                                        : QStringLiteral("已关闭自动换行"), 2000);
    });

    connect(ui->actionLineNumber, &QAction::toggled, this, [this](bool checked) {
        ui->textEdit->setLineNumbersVisible(checked);
        statusBar()->showMessage(checked ? QStringLiteral("已显示行号")
                                        : QStringLiteral("已隐藏行号"), 2000);
    });

    // -------- 查看 --------
    connect(ui->actionToolBar, &QAction::toggled, ui->toolBar, &QWidget::setVisible);
    connect(ui->toolBar, &QToolBar::visibilityChanged,
            ui->actionToolBar, &QAction::setChecked);
    connect(ui->actionStatusBar, &QAction::toggled, statusBar(), &QWidget::setVisible);

    // -------- 帮助 --------
    connect(ui->actionAbout, &QAction::triggered, this, [this] {
        AboutDialog dialog(this);
        dialog.exec();
    });

    // -------- 状态维护 -------------
    connect(ui->textEdit, &QPlainTextEdit::textChanged, this, &MainWindow::updateStatusBar);
    connect(ui->textEdit, &QPlainTextEdit::cursorPositionChanged, this, &MainWindow::updateStatusBar);
    connect(ui->textEdit->document(), &QTextDocument::modificationChanged,
            this, &MainWindow::updateWindowTitle);
}

// -------------------------------------------------------------------- 文件操作
void MainWindow::newFile()
{
    if (!maybeSave())
        return;

    ui->textEdit->clear();
    setCurrentFile(QString());
    ui->textEdit->document()->setModified(false);
    statusBar()->showMessage(QStringLiteral("已新建空白文档"), 2000);
}

void MainWindow::openFile()
{
    if (!maybeSave())
        return;

    const QString path = QFileDialog::getOpenFileName(
        this, QStringLiteral("打开文件"), QString(),
        QStringLiteral("文本文件 (*.txt);;所有文件 (*.*)"));
    if (path.isEmpty())
        return;

    loadFromFile(path);
}

bool MainWindow::loadFromFile(const QString &filePath)
{
    if (!QFile::exists(filePath)) {
        QMessageBox::warning(this, QStringLiteral("打开失败"),
                             QStringLiteral("文件不存在：\n%1").arg(filePath));
        statusBar()->showMessage(QStringLiteral("打开失败：文件不存在"), 3000);
        return false;
    }

    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        QMessageBox::warning(this, QStringLiteral("打开失败"),
                             QStringLiteral("无法读取文件：\n%1\n\n%2")
                                 .arg(filePath, file.errorString()));
        return false;
    }

    QTextStream in(&file);
    in.setEncoding(QStringConverter::Utf8);   // 统一按 UTF-8 读取
    const QString content = in.readAll();
    file.close();

    ui->textEdit->setPlainText(content);
    ui->textEdit->document()->setModified(false);
    setCurrentFile(filePath);
    statusBar()->showMessage(QStringLiteral("已打开：%1")
                                 .arg(QFileInfo(filePath).fileName()), 3000);
    return true;
}

bool MainWindow::saveFile()
{
    if (m_currentFile.isEmpty())
        return saveFileAs();
    return writeToFile(m_currentFile);
}

bool MainWindow::saveFileAs()
{
    QString path = QFileDialog::getSaveFileName(
        this, QStringLiteral("另存为"), m_currentFile,
        QStringLiteral("文本文件 (*.txt);;所有文件 (*.*)"));
    if (path.isEmpty())
        return false;

    if (!writeToFile(path))
        return false;

    setCurrentFile(path);
    return true;
}

bool MainWindow::writeToFile(const QString &filePath)
{
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        QMessageBox::warning(this, QStringLiteral("保存失败"),
                             QStringLiteral("无法写入文件：\n%1\n\n%2")
                                 .arg(filePath, file.errorString()));
        return false;
    }

    QTextStream out(&file);
    out.setEncoding(QStringConverter::Utf8);   // 统一按 UTF-8 保存
    out << ui->textEdit->toPlainText();
    file.close();

    ui->textEdit->document()->setModified(false);
    statusBar()->showMessage(QStringLiteral("已保存：%1")
                                 .arg(QFileInfo(filePath).fileName()), 3000);
    return true;
}

bool MainWindow::maybeSave()
{
    if (!ui->textEdit->document()->isModified())
        return true;

    const QString name = m_currentFile.isEmpty() ? QStringLiteral("新建文本文档")
                                                 : QFileInfo(m_currentFile).fileName();
    const QMessageBox::StandardButton ret = QMessageBox::warning(
        this, QStringLiteral("文本编辑器"),
        QStringLiteral("文档“%1”已被修改。\n是否保存修改？").arg(name),
        QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel,
        QMessageBox::Save);

    if (ret == QMessageBox::Save)
        return saveFile();
    if (ret == QMessageBox::Cancel)
        return false;
    return true;   // 选择“不保存”，直接放弃修改
}

void MainWindow::setCurrentFile(const QString &filePath)
{
    m_currentFile = filePath;
    updateWindowTitle();
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    if (maybeSave())
        event->accept();
    else
        event->ignore();
}

// ------------------------------------------------------------------ 格式设置
void MainWindow::applyCharFormat(const QTextCharFormat &format)
{
    QTextCursor original = ui->textEdit->textCursor();

    QTextCursor target = original;
    if (!target.hasSelection())
        target.select(QTextCursor::Document);   // 未选中则作用于全文
    target.mergeCharFormat(format);

    ui->textEdit->setTextCursor(original);      // 还原原来的光标位置
    ui->textEdit->mergeCurrentCharFormat(format);   // 之后输入的文字沿用该格式
}

void MainWindow::changeFont()
{
    bool ok = false;
    const QFont font = QFontDialog::getFont(&ok, ui->textEdit->document()->defaultFont(),
                                            this, QStringLiteral("字体"));
    if (!ok)
        return;

    ui->textEdit->document()->setDefaultFont(font);
    QTextCharFormat format;
    format.setFont(font);
    applyCharFormat(format);

    ui->textEdit->updateLineNumberAreaWidth();
    statusBar()->showMessage(QStringLiteral("字体：%1  %2pt")
                                 .arg(font.family())
                                 .arg(font.pointSize()), 3000);
}

void MainWindow::changeFontColor()
{
    const QColor color = QColorDialog::getColor(QColor(Qt::black), this,
                                                QStringLiteral("字体颜色"));
    if (!color.isValid())
        return;

    QTextCharFormat format;
    format.setForeground(color);
    applyCharFormat(format);
    statusBar()->showMessage(QStringLiteral("字体颜色：%1").arg(color.name()), 3000);
}

void MainWindow::changeFontBackground()
{
    const QColor color = QColorDialog::getColor(Qt::yellow, this,
                                                QStringLiteral("字体背景色"));
    if (!color.isValid())
        return;

    QTextCharFormat format;
    format.setBackground(color);
    applyCharFormat(format);
}

void MainWindow::changeEditorBackground()
{
    const QColor color = QColorDialog::getColor(Qt::white, this,
                                                QStringLiteral("编辑器背景色"));
    if (!color.isValid())
        return;

    ui->textEdit->setEditorColors(color);
    statusBar()->showMessage(QStringLiteral("编辑器背景色：%1").arg(color.name()), 3000);
}

// ---------------------------------------------------------------- 查找与替换
void MainWindow::showFindDialog()
{
    if (!m_findDialog) {
        m_findDialog = new FindDialog(this);
        connect(m_findDialog, &FindDialog::findRequested, this, &MainWindow::findNext);
    }

    const QTextCursor cursor = ui->textEdit->textCursor();
    if (cursor.hasSelection())
        m_findDialog->setFindText(cursor.selectedText());   // 把选中内容带入查找框

    m_findDialog->show();
    m_findDialog->raise();
    m_findDialog->activateWindow();
}

void MainWindow::showReplaceDialog()
{
    if (!m_replaceDialog) {
        m_replaceDialog = new ReplaceDialog(this);
        connect(m_replaceDialog, &ReplaceDialog::findRequested, this, &MainWindow::findNext);
        connect(m_replaceDialog, &ReplaceDialog::replaceRequested, this, &MainWindow::replaceOnce);
        connect(m_replaceDialog, &ReplaceDialog::replaceAllRequested, this, &MainWindow::replaceAll);
    }

    const QTextCursor cursor = ui->textEdit->textCursor();
    if (cursor.hasSelection())
        m_replaceDialog->setFindText(cursor.selectedText());

    m_replaceDialog->show();
    m_replaceDialog->raise();
    m_replaceDialog->activateWindow();
}

void MainWindow::findNext(const QString &text, bool backward, bool matchCase)
{
    if (text.isEmpty()) {
        QMessageBox::information(this, QStringLiteral("查找"),
                                 QStringLiteral("请输入要查找的内容。"));
        return;
    }

    QTextDocument::FindFlags flags;
    if (backward)
        flags |= QTextDocument::FindBackward;
    if (matchCase)
        flags |= QTextDocument::FindCaseSensitively;

    QTextCursor found = ui->textEdit->document()->find(text, ui->textEdit->textCursor(), flags);

    bool wrapped = false;
    if (found.isNull()) {
        // 到达文档边界后，从另一端继续查找，实现连续查找
        QTextCursor edge = ui->textEdit->textCursor();
        edge.movePosition(backward ? QTextCursor::End : QTextCursor::Start);
        found = ui->textEdit->document()->find(text, edge, flags);
        wrapped = !found.isNull();
    }

    if (found.isNull()) {
        statusBar()->showMessage(QStringLiteral("未找到“%1”").arg(text), 3000);
        QMessageBox::information(this, QStringLiteral("查找"),
                                 QStringLiteral("未找到“%1”。").arg(text));
        return;
    }

    ui->textEdit->setTextCursor(found);
    ui->textEdit->centerCursor();

    if (wrapped)
        statusBar()->showMessage(QStringLiteral("已到文档边界，从另一端继续查找"), 3000);
}

bool MainWindow::selectionEquals(const QString &text, bool matchCase) const
{
    const QString selected = ui->textEdit->textCursor().selectedText();
    if (selected.isEmpty())
        return false;
    return matchCase ? (selected == text) : (selected.compare(text, Qt::CaseInsensitive) == 0);
}

void MainWindow::replaceOnce(const QString &text, const QString &replacement, bool matchCase)
{
    if (text.isEmpty()) {
        QMessageBox::information(this, QStringLiteral("替换"),
                                 QStringLiteral("请输入要查找的内容。"));
        return;
    }

    // 当前选中内容与查找目标一致时才替换，否则先定位到下一个匹配项
    if (selectionEquals(text, matchCase)) {
        QTextCursor cursor = ui->textEdit->textCursor();
        cursor.insertText(replacement);
        ui->textEdit->setTextCursor(cursor);
    }

    findNext(text, false, matchCase);
}

void MainWindow::replaceAll(const QString &text, const QString &replacement, bool matchCase)
{
    if (text.isEmpty()) {
        QMessageBox::information(this, QStringLiteral("替换"),
                                 QStringLiteral("请输入要查找的内容。"));
        return;
    }

    QTextDocument::FindFlags flags;
    if (matchCase)
        flags |= QTextDocument::FindCaseSensitively;

    QTextDocument *document = ui->textEdit->document();
    QTextCursor allCursor(document);
    allCursor.beginEditBlock();     // 合并为一次可撤销的操作

    int count = 0;
    QTextCursor found = document->find(text, 0, flags);
    while (!found.isNull()) {
        // 记录插入点之后的偏移，避免替换文本中再次包含目标时死循环
        const int after = found.selectionStart() + replacement.length();
        found.insertText(replacement);
        ++count;
        found = document->find(text, after, flags);
    }

    allCursor.endEditBlock();

    if (count == 0) {
        statusBar()->showMessage(QStringLiteral("未找到“%1”").arg(text), 3000);
        QMessageBox::information(this, QStringLiteral("替换"),
                                 QStringLiteral("未找到“%1”，没有内容被替换。").arg(text));
    } else {
        statusBar()->showMessage(QStringLiteral("共替换 %1 处").arg(count), 3000);
        QMessageBox::information(this, QStringLiteral("替换"),
                                 QStringLiteral("共替换 %1 处。").arg(count));
    }
}

// ---------------------------------------------------------------- 界面状态维护
void MainWindow::updateWindowTitle()
{
    const QString name = m_currentFile.isEmpty()
                             ? QStringLiteral("新建文本文档")
                             : QFileInfo(m_currentFile).fileName();
    const QString marker = ui->textEdit->document()->isModified()
                               ? QStringLiteral("* ")
                               : QString();
    setWindowTitle(QStringLiteral("%1%2 - 编辑器").arg(marker, name));
}

void MainWindow::updateStatusBar()
{
    const QTextCursor cursor = ui->textEdit->textCursor();
    const int length = ui->textEdit->toPlainText().length();
    const int lines = ui->textEdit->blockCount();
    const int line = cursor.blockNumber() + 1;
    const int column = cursor.positionInBlock() + 1;

    m_infoLabel->setText(QStringLiteral("length: %1    lines: %2    Ln: %3    Col: %4")
                             .arg(length).arg(lines).arg(line).arg(column));
}

void MainWindow::updateEditActionStates()
{
    const bool hasSelection = ui->textEdit->textCursor().hasSelection();
    ui->actionCut->setEnabled(hasSelection);
    ui->actionCopy->setEnabled(hasSelection);
}

void MainWindow::updatePasteActionState()
{
    const QMimeData *mime = QGuiApplication::clipboard()->mimeData();
    ui->actionPaste->setEnabled(mime != nullptr && mime->hasText());
}

void MainWindow::autoSave()
{
    if (!ui->textEdit->document()->isModified() || m_currentFile.isEmpty())
        return;   // 没有修改或还没命名，无法自动保存

    if (writeToFile(m_currentFile))
        statusBar()->showMessage(QStringLiteral("已自动保存：%1")
                                     .arg(QFileInfo(m_currentFile).fileName()), 3000);
}
