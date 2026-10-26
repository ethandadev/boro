//src/gui/editor/paint.cpp
// all the drawing stuff
#include "editor.hpp"
#include <QPainter>
#include <QColor>
#include <utility> // for std::swap

void Editor::paintEvent(QPaintEvent*) {
    QPainter painter(this);              // a "pen" for drawing on this widget
    painter.fillRect(rect(), QColor(255, 255, 255)); // fill the background with white
    painter.setPen(QColor(0, 0, 0)); // set the pen color to black

    // order matters, later stuff gets drawn on top
    drawGutter(painter);
    drawSelection(painter);
    drawLines(painter);
    drawCursor(painter);
}

void Editor::drawCursor(QPainter& painter) { 
    painter.setPen(QColor(0, 0, 0));
    int lineHeight = fontMetrics().height();

    int gutterWidth = textLeft(); // space reserved for line numbers on the left

    if (cursorVisible) {
        std::string line = buf.lines[buf.cursor.row];
        QString beforeCursor = QString::fromStdString(line.substr(0, buf.cursor.col)); // like line[:col]
        int cursorX = gutterWidth + fontMetrics().horizontalAdvance(beforeCursor);
        int cursorTop = int(buf.cursor.row) * lineHeight - scrollY;
        painter.fillRect(cursorX, cursorTop, 2, lineHeight, QColor(0, 0, 0)); // draw blinking cursor bar
    }
}

void Editor::drawGutter(QPainter& painter) {
    int charWidth = fontMetrics().horizontalAdvance('0');

    int gutterWidth = textLeft(); // space reserved for line numbers on the left

    int dividerX = gutterWidth - charWidth;

    painter.fillRect(0, 0, dividerX, height(), QColor(250, 250, 250)); // gutter background
    
    painter.setPen(QColor(80, 80, 80)); //setcolor
    painter.drawLine(dividerX, 0, dividerX, height()); // draw line takes x1, y1, x2, y2
}

void Editor::drawSelection(QPainter& painter) {
    int lineHeight = fontMetrics().height();
    int charWidth = fontMetrics().horizontalAdvance('0');

    int gutterWidth = textLeft(); // space reserved for line numbers on the left

    // flip them if you selected backwards
    Cursor start = anchor;
    Cursor end = buf.cursor;
    if (end.row < start.row || (end.row == start.row && end.col < start.col)) {
        std::swap(start, end);
    }

    // one blue box per line
    for (size_t row = start.row; row <= end.row; row++) {
        QString line = QString::fromStdString(buf.lines[row]);
        size_t fromCol = (row == start.row) ? start.col : 0;
        size_t toCol = (row == end.row) ? end.col : line.size();

        int x1 = gutterWidth + fontMetrics().horizontalAdvance(line.left(int(fromCol)));
        int x2 = gutterWidth + fontMetrics().horizontalAdvance(line.left(int(toCol)));
        if (row != end.row) {
            x2 += charWidth; // show the newline is selected
        }

        int top = int(row) * lineHeight - scrollY;
        painter.fillRect(x1, top, x2 - x1, lineHeight, QColor(173, 214, 255)); // light blue
    }
}


void Editor::drawLines(QPainter& painter) {
    int lineHeight = fontMetrics().height();
    int charWidth = fontMetrics().horizontalAdvance('0');

    int gutterWidth = textLeft(); // space reserved for line numbers on the left

    int dividerX = gutterWidth - charWidth;
    // loop through all lines and draw both line numbers and text contents
    for (size_t i = 0; i < buf.lines.size(); i++) {
        int y = int(i) * lineHeight - scrollY + fontMetrics().ascent();
        QString num = QString::number(int(i) + 1);
        int numX = dividerX - charWidth - int(num.length()) * charWidth;
        painter.drawText(numX, y, num);
        painter.drawText(gutterWidth, y, QString::fromStdString(buf.lines[i]));
    }
}
