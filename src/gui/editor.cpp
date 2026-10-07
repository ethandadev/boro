#include "editor.hpp"
#include "config.hpp"
#include <QPainter>
#include <QFont>
#include <QColor>
#include <QKeyEvent>
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
    setFocusPolicy(Qt::StrongFocus); // NEW: let this widget receive key presses
}

void Editor::paintEvent(QPaintEvent*) {
    QPainter painter(this);              // a "pen" for drawing on this widget
    painter.fillRect(rect(), QColor(255, 255, 255)); // fill the background with white
    painter.setPen(QColor(0, 0, 0)); // set the pen color to black

    int lineHeight = fontMetrics().height();
    int charWidth = fontMetrics().horizontalAdvance('0');

    size_t lineCount = buf.lines.size(); // size_t is an unsigned integer type used for sizes

    size_t gutterDigits = std::to_string(lineCount).size(); // variable to store the highest digit in the line numbers

    size_t gutterWidth = (int(gutterDigits) + 3) * charWidth;

    int dividerX = gutterWidth - charWidth;

    painter.fillRect(0, 0, gutterWidth - charWidth, height(), QColor(250, 250, 250));

    for (size_t i = 0; i < buf.lines.size(); i++) {
        int y = int(i) * lineHeight + fontMetrics().ascent(); 
        QString num = QString::number(int(i) + 1);
        int numX = dividerX - charWidth - int(num.length()) * charWidth;
        painter.drawText(numX, y, num);
        painter.drawText(gutterWidth, y, QString::fromStdString(buf.lines[i]));
    }

    //draw divider
    painter.setPen(QColor(80, 80, 80)); //setcolor
    painter.drawLine(dividerX, 0, dividerX, height()); // draw line takes x1, y1, x2, y2

    //draw cursor
    std::string line = buf.lines[buf.cursor.row];
    QString beforeCursor = QString::fromStdString(line.substr(0, buf.cursor.col)); // like line[:col]
    int cursorX = int(gutterWidth) + fontMetrics().horizontalAdvance(beforeCursor);
    int cursorTop = int(buf.cursor.row) * lineHeight;

    painter.setPen(QColor(0, 0, 0));
    painter.fillRect(cursorX, cursorTop, 2, lineHeight, QColor(0, 0, 0));
}

void Editor::keyPressEvent(QKeyEvent* event) {
    int key = event->key();
    QString text = event->text();
    
    if (key == Qt::Key_Backspace) {
        deleteChar(buf);
    } else if (!text.isEmpty()) {
        int code = text[0].unicode();
        if (code >= 32 && code <= 126) {
            insertChar(buf, char(code));
        }
    }

    update(); // ask Qt to call paintEvent again, so the change shows up
}

