//src/gui/editor.cpp
#include "editor.hpp"
#include "config.hpp"
#include <QPainter>
#include <QFont>
#include <QColor>
#include <QKeyEvent>
#include <QFileDialog>
#include <QMessageBox>
#include <QMouseEvent>
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
    resetBlink();

    buf.lines.push_back(""); // the editor always has at least one line

    setCursor(Qt::IBeamCursor); // show the text-editing mouse pointer over the editor
}

void Editor::paintEvent(QPaintEvent*) {
    QPainter painter(this);              // a "pen" for drawing on this widget
    painter.fillRect(rect(), QColor(255, 255, 255)); // fill the background with white
    painter.setPen(QColor(0, 0, 0)); // set the pen color to black

    int lineHeight = fontMetrics().height();
    int charWidth = fontMetrics().horizontalAdvance('0');

    int gutterWidth = textLeft(); // space reserved for line numbers on the left

    int dividerX = gutterWidth - charWidth;

    painter.fillRect(0, 0, dividerX, height(), QColor(250, 250, 250)); // gutter background

    Cursor start = anchor;
    Cursor end = buf.cursor;
    if (end.row < start.row || (end.row == start.row && end.col < start.col)) {
        std::swap(start, end);
    }

    for (size_t row = start.row; row <= end.row; row++) {
        QString line = QString::fromStdString(buf.lines[row]);
        size_t fromCol = (row == start.row) ? start.col : 0;
        size_t toCol = (row == end.row) ? end.col : line.size();

        int x1 = gutterWidth + fontMetrics().horizontalAdvance(line.left(int(fromCol)));
        int x2 = gutterWidth + fontMetrics().horizontalAdvance(line.left(int(toCol)));
        if (row != end.row) {
            x2 += charWidth;
        }

        int top = int(row) * lineHeight - scrollY;
        painter.fillRect(x1, top, x2 - x1, lineHeight, QColor(173, 214, 255));
    }

    // loop through all lines and draw both line numbers and text contents
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
        int cursorX = gutterWidth + fontMetrics().horizontalAdvance(beforeCursor);
        int cursorTop = int(buf.cursor.row) * lineHeight - scrollY;
        painter.fillRect(cursorX, cursorTop, 2, lineHeight, QColor(0, 0, 0)); // draw blinking cursor bar
    }
}

void Editor::keyPressEvent(QKeyEvent* event) {
    int key = event->key();
    QString text = event->text();
    bool shift = event->modifiers() & Qt::ShiftModifier;
    bool isArrow = (key == Qt::Key_Up || key == Qt::Key_Down || key == Qt::Key_Left || key == Qt::Key_Right);
    
    
    // map incoming keyboard keys to buffer editing actions
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
    } else if (!text.isEmpty()) {
        int code = text[0].unicode();
        if (code >= 32 && code <= 126) {
            insertChar(buf, char(code)); // regular printable characters
        }
    } 

    if (!(shift && isArrow)) {
        anchor = buf.cursor;
    }

    keepCursorVisible(); // scroll if needed
    resetBlink();
    update(); // ask Qt to call paintEvent again, so the change shows up
}

void Editor::blink() {
    cursorVisible = !cursorVisible; // flip: true → false, false → true
    update();                       // redraw
}

bool Editor::focusNextPrevChild(bool) {
    return false; // capture tab key presses instead of letting focus jump away
}

void Editor::keepCursorVisible() {
    int lineHeight = fontMetrics().height();
    int cursorTop = int(buf.cursor.row) * lineHeight;
    int cursorBottom = cursorTop + lineHeight;

    // auto-scroll up or down if the cursor goes out of view bounds
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
        deltaY = event->pixelDelta().y(); // smooth trackpad scrolling
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

void Editor::openFile() {
    if (!confirmDiscard()) {
            return; // user chose not to discard changes
    }
    QString path = QFileDialog::getOpenFileName(this, "Open File");   // empty if you pressed cancel
    if (!path.isEmpty()) {                                   // only if a file was actually picked
        buf = Buffer();                   // throw away the old file, start fresh
        anchor = buf.cursor;
        loadFile(buf, path.toStdString());
        window()->setWindowTitle("Boro — " + path);
        scrollY = 0; // reset scroll position
        update();    // redraw the editor with the new file
    }

}

bool Editor::confirmDiscard() {
    if (!buf.dirty) {
        return true; // no unsaved changes, safe to discard
    }
    auto answer = QMessageBox::question(this, "Unsaved changes", "Discard your changes to this file?");
    return answer == QMessageBox::Yes; // prompt user if closing with dirty buffer
}

void Editor::save() {
    if (buf.fileName.empty()) {
        QString path = QFileDialog::getSaveFileName(this, "Save File");
        if (path.isEmpty()) {
            return; // cancelled save dialog
        }
        buf.fileName = path.toStdString();
        window() -> setWindowTitle("Boro - " + path);
    }
    saveFile(buf, buf.fileName);
    update();
}

void Editor::newFile() {
    if (!confirmDiscard()) {
        return; // user cancelled new file action
    }
    buf = Buffer();
    anchor = buf.cursor;
    buf.lines.push_back("");
    scrollY = 0;
    window() -> setWindowTitle("Boro - Untitled");
    update();
}

int Editor::textLeft() {
    int charWidth = fontMetrics().horizontalAdvance('0');
    int digits = std::to_string(buf.lines.size()).size();
    return (digits + 3) * charWidth; // compute dynamic gutter width based on line count
}

void Editor::mousePressEvent(QMouseEvent* event) {
    Cursor pos = positionAt(int(event->position().x()), int(event->position().y()));
    buf.cursor = pos;
    anchor = pos; // update text cursor position on click

    resetBlink();

    update();
}

Cursor Editor::positionAt(int x, int y) {
    int lineHeight = fontMetrics().height();
    int row = (y + scrollY) / lineHeight;
    row = std::min(row, int(buf.lines.size())-1);
    row = std::max(row, 0);
    QString line = QString::fromStdString(buf.lines[row]);

    int col = 0;

    int textX = x - textLeft();   // pixels from where the text starts
    // figure out which character column was clicked by checking character widths
    while (col < line.length()) {
        int left = fontMetrics().horizontalAdvance(line.left(col));
        int right = fontMetrics().horizontalAdvance(line.left(col+1));
        if (textX < (left+right) / 2) {
            break;
        }
        col++;
    }

    return Cursor(row, col);
}

void Editor::mouseMoveEvent(QMouseEvent* event) {
    buf.cursor = positionAt(int(event->position().x()), int(event->position().y())); // update cursor while dragging mouse
    keepCursorVisible();
    resetBlink();
    update();
}

void Editor::resetBlink() {
    cursorVisible = true;
    blinkTimer.start(500);
}
