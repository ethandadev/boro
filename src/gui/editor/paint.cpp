//src/gui/editor/paint.cpp
// all the drawing stuff
#include "editor.hpp"
#include <QPainter>
#include <QColor>


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

    int gutterWidth = textLeft(); // space reserved for line numbers on the left

    if (cursorVisible) {
        const std::string& line = buf.lines[buf.cursor.row];
        QString beforeCursor = QString::fromStdString(line.substr(0, buf.cursor.col)); // like line[:col]
        int cursorX = gutterWidth + fontMetrics().horizontalAdvance(beforeCursor);
        int cursorTop = int(buf.cursor.row) * lineHeight() - scrollY;
        painter.fillRect(cursorX, cursorTop, 2, lineHeight(), QColor(0, 0, 0)); // draw blinking cursor bar
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
    int gutterWidth = textLeft(); // space reserved for line numbers on the left

    auto [start, end] = selectionRange();       // like:  start, end = self.selection_range()


    // one blue box per line
    size_t first = firstVisibleRow();
    size_t last = lastVisibleRow();

    for (size_t row = std::max(start.row, first); row <= std::min(end.row, last); row++) {
        QString line = QString::fromStdString(buf.lines[row].substr(0, maxCols()));
        size_t fromCol = (row == start.row) ? start.col : 0;
        size_t toCol = (row == end.row) ? end.col : line.size();

        int x1 = gutterWidth + fontMetrics().horizontalAdvance(line.left(int(fromCol)));
        int x2 = gutterWidth + fontMetrics().horizontalAdvance(line.left(int(toCol)));
        if (row != end.row) {
            x2 += charWidth(); // show the newline is selected
        }

        int top = int(row) * lineHeight() - scrollY;
        painter.fillRect(x1, top, x2 - x1, lineHeight(), QColor(173, 214, 255)); // light blue
    }
}


void Editor::drawLines(QPainter& painter) {
    int gutterWidth = textLeft(); // space reserved for line numbers on the left

    int dividerX = gutterWidth - charWidth();
    // loop through all lines and draw both line numbers and text contents
    for (int i = firstVisibleRow(); i <= lastVisibleRow(); i++) {
        int y = int(i) * lineHeight() - scrollY + fontMetrics().ascent();
        QString num = QString::number(int(i) + 1);
        int numX = dividerX - charWidth() - int(num.length()) * charWidth();
        painter.drawText(numX, y, num);
        painter.drawText(gutterWidth, y, QString::fromStdString(buf.lines[i].substr(0, maxCols())));
    }
}
