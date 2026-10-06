#include "editor.hpp"
#include "config.hpp"
#include <QPainter>
#include <QFont>
#include <QColor>
#include <string>

//set font and stuff
Editor::Editor(QWidget* parent) : QWidget(parent) {
    QFont font;
    font.setFamilies({                              // { } builds a list, like Python's [ ]
        QString::fromStdString(FONT_FAMILY),
        QString::fromStdString(FALLBACK_FONT),
    });
    font.setPointSize(FONT_SIZE); // set fontsize
    setFont(font); //set font
}

void Editor::paintEvent(QPaintEvent*) {
    QPainter painter(this);              // a "pen" for drawing on this widget

    int lineHeight = fontMetrics().height();
    int charWidth = fontMetrics().horizontalAdvance('0');

    size_t lineCount = buf.lines.size(); // size_t is an unsigned integer type used for sizes

    size_t gutterDigits = std::to_string(lineCount).size(); // variable to store the highest digit in the line numbers

    size_t gutterWidth = (int(gutterDigits) + 2) * charWidth;

    for (size_t i = 0; i < buf.lines.size(); i++) {
        int y = (int(i) + 1) * lineHeight;
        painter.drawText(10, y, QString::number(int(i) + 1));
        painter.drawText(gutterWidth, y, QString::fromStdString(buf.lines[i]));
    }

    //draw divider
    int dividerX = gutterWidth - charWidth;
    painter.setPen(QColor(80, 80, 80)); //setcolor
    painter.drawLine(dividerX, 0, dividerX, height()); // draw line takes x1, y1, x2, y2

    //draw cursor
    painter.setPen(QColor(0,0,0));
    painter.drawLine((buf.cursor.col*charWidth)+gutterWidth, (buf.cursor.row*lineHeight), (buf.cursor.col*charWidth)+gutterWidth, (buf.cursor.row*lineHeight)+lineHeight);
}

