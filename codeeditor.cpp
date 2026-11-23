#include "codeeditor.h"

#include <QPainter>
#include <QTextBlock>
#include <QPaintEvent>
#include <QScrollBar>

// ---------------------------------------------------------------- 行号栏控件
class LineNumberArea : public QWidget
{
public:
    explicit LineNumberArea(CodeEditor *editor) : QWidget(editor), m_editor(editor) {}

    QSize sizeHint() const override { return QSize(m_editor->lineNumberAreaWidth(), 0); }

protected:
    void paintEvent(QPaintEvent *event) override { m_editor->lineNumberAreaPaintEvent(event); }

private:
    CodeEditor *m_editor;
};

// ------------------------------------------------------------------ CodeEditor
CodeEditor::CodeEditor(QWidget *parent)
    : QPlainTextEdit(parent)
{
    m_lineNumberArea = new LineNumberArea(this);

    connect(this, &QPlainTextEdit::blockCountChanged,
            this, &CodeEditor::updateLineNumberAreaWidth);
    connect(this, &QPlainTextEdit::updateRequest,
            this, &CodeEditor::updateLineNumberArea);
    connect(this, &QPlainTextEdit::cursorPositionChanged,
            this, &CodeEditor::highlightCurrentLine);

    // 制表符宽度按当前字体计算，保证排版稳定
    setTabStopDistance(fontMetrics().horizontalAdvance(QLatin1Char(' ')) * 4);

    updateLineNumberAreaWidth();
    highlightCurrentLine();
}

int CodeEditor::lineNumberAreaWidth() const
{
    if (!m_lineNumbersVisible)
        return 0;

    int digits = QString::number(qMax(1, blockCount())).length();
    return 8 + fontMetrics().horizontalAdvance(QLatin1Char('9')) * qMax(2, digits);
}

void CodeEditor::updateLineNumberAreaWidth()
{
    // 左侧留出行号栏宽度，使文本区不被覆盖
    setViewportMargins(lineNumberAreaWidth(), 0, 0, 0);
}

void CodeEditor::updateLineNumberArea(const QRect &rect, int dy)
{
    if (dy != 0)
        m_lineNumberArea->scroll(0, dy);
    else
        m_lineNumberArea->update(0, rect.y(), m_lineNumberArea->width(), rect.height());

    if (rect.contains(viewport()->rect()))
        updateLineNumberAreaWidth();
}

void CodeEditor::resizeEvent(QResizeEvent *event)
{
    QPlainTextEdit::resizeEvent(event);

    const QRect cr = contentsRect();
    m_lineNumberArea->setGeometry(QRect(cr.left(), cr.top(), lineNumberAreaWidth(), cr.height()));
}

void CodeEditor::setLineNumbersVisible(bool visible)
{
    m_lineNumbersVisible = visible;
    m_lineNumberArea->setVisible(visible);
    updateLineNumberAreaWidth();
    update();
}

void CodeEditor::setEditorColors(const QColor &background)
{
    // 行号栏底色与当前行高亮随编辑器背景色自动调整，避免深色背景上看不清
    m_lineNumberBackColor = background.lightness() > 128 ? background.darker(108)
                                                         : background.lighter(130);
    m_currentLineColor = background.lightness() > 128
                             ? QColor(0xFF, 0xFF, 0xCC, 0xB0)
                             : QColor(0xFF, 0xFF, 0xFF, 0x30);

    QPalette pal = palette();
    pal.setColor(QPalette::Base, background);
    pal.setColor(QPalette::Window, background);
    setPalette(pal);

    highlightCurrentLine();
    m_lineNumberArea->update();
}

void CodeEditor::highlightCurrentLine()
{
    // 当前行整行浅色高亮，方便定位光标
    QTextEdit::ExtraSelection selection;
    selection.format.setBackground(m_currentLineColor);
    selection.format.setProperty(QTextFormat::FullWidthSelection, true);
    selection.cursor = textCursor();
    selection.cursor.clearSelection();

    QList<QTextEdit::ExtraSelection> extraSelections;
    extraSelections.append(selection);
    setExtraSelections(extraSelections);
}

void CodeEditor::lineNumberAreaPaintEvent(QPaintEvent *event)
{
    QPainter painter(m_lineNumberArea);
    painter.fillRect(event->rect(), m_lineNumberBackColor);

    QTextBlock block = firstVisibleBlock();
    int blockNumber = block.blockNumber();
    int top = qRound(blockBoundingGeometry(block).translated(contentOffset()).top());
    int bottom = top + qRound(blockBoundingRect(block).height());

    while (block.isValid() && top <= event->rect().bottom()) {
        if (block.isVisible() && bottom >= event->rect().top()) {
            const QString number = QString::number(blockNumber + 1);
            painter.setPen(QColor(0x80, 0x80, 0x80));
            painter.drawText(0, top, m_lineNumberArea->width() - 6,
                             fontMetrics().height(), Qt::AlignRight, number);
        }

        block = block.next();
        top = bottom;
        bottom = top + qRound(blockBoundingRect(block).height());
        ++blockNumber;
    }
}
