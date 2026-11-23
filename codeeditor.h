#ifndef CODEEDITOR_H
#define CODEEDITOR_H

#include <QPlainTextEdit>
#include <QColor>

class LineNumberArea;

/**
 * 从 QPlainTextEdit 派生的编辑器控件，额外提供行号栏。
 * 在 Qt Designer 中通过"提升（promote）"把 QPlainTextEdit 替换成本类。
 */
class CodeEditor : public QPlainTextEdit
{
    Q_OBJECT

public:
    explicit CodeEditor(QWidget *parent = nullptr);

    // 行号栏绘制，由 LineNumberArea 的 paintEvent 调用
    void lineNumberAreaPaintEvent(QPaintEvent *event);
    // 行号栏所需的宽度（随总行数的位数变化）
    int lineNumberAreaWidth() const;

    void setLineNumbersVisible(bool visible);
    bool isLineNumbersVisible() const { return m_lineNumbersVisible; }

    // 编辑器背景色变化时同步行号栏与当前行高亮颜色
    void setEditorColors(const QColor &background);

public slots:
    // 字体改变后行号栏宽度需要重新计算
    void updateLineNumberAreaWidth();

protected:
    void resizeEvent(QResizeEvent *event) override;

private slots:
    void updateLineNumberArea(const QRect &rect, int dy);
    void highlightCurrentLine();

private:
    LineNumberArea *m_lineNumberArea = nullptr;
    bool     m_lineNumbersVisible   = true;
    QColor   m_lineNumberBackColor  = QColor(0xEF, 0xEF, 0xEF);
    QColor   m_currentLineColor     = QColor(0xFF, 0xFF, 0xCC, 0xB0);
};

#endif // CODEEDITOR_H
