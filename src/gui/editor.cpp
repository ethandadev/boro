//src/gui/editor.cpp
#include "editor.hpp"
#include "config.hpp"
#include <QPainter>
#include <QFont>
#include <QColor>
#include <QKeyEvent>
#include <QFileDialog>
#include <QMessageBox>
#include <string>
#include <algorithm> // for std::max and std::min

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

    // every time blinkTimer ticks, call this editor's blink()
    connect(&blinkTimer, &QTimer::timeout, this, &Editor::blink);
    blinkTimer.start(500); // tick every 500 ms
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
        int y = int(i) * lineHeight - scrollY + fontMetrics().ascent();
        QString num = QString::number(int(i) + 1);
        int numX = dividerX - charWidth - int(num.length()) * charWidth;
        painter.drawText(numX, y, num);
        painter.drawText(gutterWidth, y, QString::fromStdString(buf.lines[i]));
    }

    //draw divider
    painter.setPen(QColor(80, 80, 80)); //setcolor
    painter.drawLine(dividerX, 0, dividerX, height()); // draw line takes x1, y1, x2, y2

    //draw cursor
    painter.setPen(QColor(0, 0, 0));
    if (cursorVisible) {
        std::string line = buf.lines[buf.cursor.row];
        QString beforeCursor = QString::fromStdString(line.substr(0, buf.cursor.col)); // like line[:col]
        int cursorX = int(gutterWidth) + fontMetrics().horizontalAdvance(beforeCursor);
        int cursorTop = int(buf.cursor.row) * lineHeight - scrollY;
        painter.fillRect(cursorX, cursorTop, 2, lineHeight, QColor(0, 0, 0));
    }
}

void Editor::keyPressEvent(QKeyEvent* event) {
    int key = event->key();
    QString text = event->text();
    
    if (key == Qt::Key_Backspace) {
        deleteChar(buf);
    } else if (key == Qt::Key_Tab) {
        insertTab(buf);
    } else if (key == Qt::Key_Up) {
        moveUp(buf);
    } else if (key == Qt::Key_Left) {
        moveLeft(buf);
    } else if (key == Qt::Key_Right) {
        moveRight(buf);
    } else if (key == Qt::Key_Down) {
        moveDown(buf);
    } else if (key == Qt::Key_Enter || key == Qt::Key_Return) {
        insertNewLine(buf);
    } else if (key == Qt::Key_S && (event -> modifiers() & Qt::ControlModifier)) {
        saveFile(buf, buf.fileName);
    } else if (key == Qt::Key_O && (event -> modifiers() & Qt::ControlModifier)) {
        if (buf.dirty) {
            auto answer = QMessageBox::question(this, "Unsaved changes", "Discard your changes to this file?");
            if (answer != QMessageBox::Yes) {
                return;                         // stop: don't open anything
            }
        }
        QString path = QFileDialog::getOpenFileName(this, "Open File");   // empty if you pressed cancel
        if (!path.isEmpty()) {                                   // only if a file was actually picked
            buf = Buffer();                   // throw away the old file, start fresh
            loadFile(buf, path.toStdString());
            window()->setWindowTitle("Boro — " + path);
        }
    } else if (!text.isEmpty()) {
        int code = text[0].unicode();
        if (code >= 32 && code <= 126) {
            insertChar(buf, char(code));
        }
    }

    keepCursorVisible(); // NEW: scroll if needed
    cursorVisible = true;
    blinkTimer.start(500);
    update(); // ask Qt to call paintEvent again, so the change shows up
}

void Editor::blink() {
    cursorVisible = !cursorVisible; // flip: true → false, false → true
    update();                       // redraw
}

bool Editor::focusNextPrevChild(bool) {
    return false;
}

void Editor::keepCursorVisible() {
    int lineHeight = fontMetrics().height();
    int cursorTop = int(buf.cursor.row) * lineHeight;
    int cursorBottom = cursorTop + lineHeight;

    if (cursorTop < scrollY) {
        scrollY = cursorTop;
    } else if (cursorBottom > scrollY + height()) {
        scrollY = cursorBottom - height();
    }

    clampScroll();       // NEW: make sure scrollY is within valid range
}

void Editor::wheelEvent(QWheelEvent* event) {
    int deltaY;
    if (!event->pixelDelta().isNull()) {
        deltaY = event->pixelDelta().y();
    } else {
        deltaY = event->angleDelta().y() * 3 * fontMetrics().height() / 120; // 120 is the default angleDelta for one notch
    }

    scrollY -= deltaY; // subtract because scrolling up should decrease scrollY
    clampScroll();       // NEW: make sure scrollY is within valid range
    update();
}

void Editor::clampScroll() {
    int lineHeight = fontMetrics().height();
    int contentHeight = int(buf.lines.size()) * lineHeight;
    int extra = int(height() * SCROLL_PAST_END);
    int maxScroll = contentHeight + extra - height();

    scrollY = std::min(scrollY, maxScroll);
    scrollY = std::max(scrollY, 0); // Ensure scrollY is not negative
}