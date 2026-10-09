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
    blinkTimer.start(500); // tick every 500 ms

    buf.lines.push_back(""); // the editor always has at least one line

    setCursor(Qt::IBeamCursor); // show the text-editing mouse pointer over the editor
}

void Editor::paintEvent(QPaintEvent*) {
    QPainter painter(this);              // a "pen" for drawing on this widget
    painter.fillRect(rect(), QColor(255, 255, 255)); // fill the background with white
    painter.setPen(QColor(0, 0, 0)); // set the pen color to black

    int lineHeight = fontMetrics().height();
    int charWidth = fontMetrics().horizontalAdvance('0');

    int gutterWidth = textLeft();

    int dividerX = gutterWidth - charWidth;

    painter.fillRect(0, 0, dividerX, height(), QColor(250, 250, 250));

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
    } else if (!text.isEmpty()) {
        int code = text[0].unicode();
        if (code >= 32 && code <= 126) {
            insertChar(buf, char(code));
        }
    } 

    keepCursorVisible(); // scroll if needed
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

void Editor::openFile() {
    if (!confirmDiscard()) {
            return; // user chose not to discard changes
    }
    QString path = QFileDialog::getOpenFileName(this, "Open File");   // empty if you pressed cancel
    if (!path.isEmpty()) {                                   // only if a file was actually picked
        buf = Buffer();                   // throw away the old file, start fresh
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
    return answer == QMessageBox::Yes;
}

void Editor::save() {
    if (buf.fileName.empty()) {
        QString path = QFileDialog::getSaveFileName(this, "Save File");
        if (path.isEmpty()) {
            return;
        }
        buf.fileName = path.toStdString();
        window() -> setWindowTitle("Boro - " + path);
    }
    saveFile(buf, buf.fileName);
    update();
}

void Editor::newFile() {
    if (!confirmDiscard()) {
        return;
    }
    buf = Buffer();
    buf.lines.push_back("");
    scrollY = 0;
    window() -> setWindowTitle("Boro - Untitled");
    update();
}

int Editor::textLeft() {
    int charWidth = fontMetrics().horizontalAdvance('0');
    int digits = std::to_string(buf.lines.size()).size();
    return (digits + 3) * charWidth;
}

void Editor::mousePressEvent(QMouseEvent* event) {
    int lineHeight = fontMetrics().height();
    int y = int(event->position().y());
    int row = (y + scrollY) / lineHeight;
    row = std::min(row, int(buf.lines.size())-1);
    buf.cursor.row = row;
    int x = int(event->position().x()) - textLeft();
    QString line = QString::fromStdString(buf.lines[row]);

    int col = 0;
    while (col < line.length()) {
        int left = fontMetrics().horizontalAdvance(line.left(col));
        int right = fontMetrics().horizontalAdvance(line.left(col+1));
        if (x < (left+right) / 2) {
            break;
        }
        col++;
    }

    buf.cursor.col = col;

    cursorVisible = true;
    blinkTimer.start(500);
    
    update();
}