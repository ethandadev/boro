#include "editor.hpp"
#include "config.hpp"
#include <QKeyEvent>
#include <QMouseEvent>
#include <QApplication>
#include <QClipboard>
#include <QWheelEvent>
#include <algorithm>


//nice things
void Editor::mouseMoveEvent(QMouseEvent* event) {
    buf.cursor = positionAt(int(event->position().x()), int(event->position().y())); // update cursor while dragging mouse
    keepCursorVisible();
    resetBlink();
    update();
}

// move cursor to the right place
void Editor::mousePressEvent(QMouseEvent* event) {
    Cursor pos = positionAt(int(event->position().x()), int(event->position().y()));
    buf.cursor = pos;
    anchor = pos; // update text cursor position on click

    resetBlink();

    update();
}

void Editor::keepCursorVisible() {
    // keeps the cursor visible :D
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

//scrolling
void Editor::wheelEvent(QWheelEvent* event) {
    int deltaY;
    if (!event->pixelDelta().isNull()) {
        deltaY = event->pixelDelta().y(); // smooth trackpad scrolling
    } else {
        deltaY = event->angleDelta().y() * 3 * fontMetrics().height() / 120; // 120 is the default angleDelta for one notch
    }

    scrollY -= deltaY; // subtract because scrolling up should decrease scrollY
    clampScroll();       //  make sure scrollY is within valid range
    update();
}

//some weird ahh helper function
void Editor::clampScroll() {
    int lineHeight = fontMetrics().height();
    int contentHeight = int(buf.lines.size()) * lineHeight;
    int extra = int(height() * SCROLL_PAST_END);
    int maxScroll = contentHeight + extra - height();

    scrollY = std::min(scrollY, maxScroll);
    scrollY = std::max(scrollY, 0); // Ensure scrollY is not negative
}


//something
void Editor::keyPressEvent(QKeyEvent* event) {
    int key = event->key();
    QString text = event->text();
    bool shift = event->modifiers() & Qt::ShiftModifier;
    bool isArrow = (key == Qt::Key_Up || key == Qt::Key_Down || key == Qt::Key_Left || key == Qt::Key_Right);
    bool cmdOrCtrl = event->modifiers() & Qt::ControlModifier;   // define this first
    bool isTyping = key == Qt::Key_Backspace || key == Qt::Key_Return || key == Qt::Key_Enter || key == Qt::Key_Tab || (!cmdOrCtrl && !text.isEmpty() && text[0].unicode() >= 32 && text[0].unicode() <= 126); 

    if (cmdOrCtrl) {
        return;
    }
    // map incoming keyboard keys to buffer editing actions

    // holy if else if else if chain
    if (key == Qt::Key_Backspace) {
        if (!hasSelection()) {
            moveLeft(buf);
        }
        replaceSelection("");
    } else if (key == Qt::Key_Tab) {
        replaceSelection(std::string(" ", TAB_WIDTH));
    } else if (isArrow) {
        if (key == Qt::Key_Up) {
            moveUp(buf);
        } else if (key == Qt::Key_Left) {
            moveLeft(buf);
        } else if (key == Qt::Key_Right) {
            moveRight(buf);
        } else if (key == Qt::Key_Down) {
            moveDown(buf);
        }
        if (!shift) {
            anchor = buf.cursor;
        }
        canMerge = false;
    } else if (key == Qt::Key_Enter || key == Qt::Key_Return) {
        replaceSelection("\n");
    } else if (isTyping) {
        replaceSelection(text[0]);
    } 

    keepCursorVisible(); // scroll if needed
    resetBlink();
    update(); // ask Qt to call paintEvent again, so the change shows up
}

void Editor::selectAll() {
    anchor = Cursor(0,0);
    size_t last = buf.lines.size()-1;
    buf.cursor = Cursor(last, buf.lines[last].size());
    update();
}


//the cool and actually useful stuff below :D
void Editor::copy() {
    if (!hasSelection()) {
        return;
    }
    QApplication::clipboard()->setText(QString::fromStdString(selectedText()) );
}

void Editor::cut() {
    if (!hasSelection()){
        return;
    }
    copy();
    deleteSelection();

    //this stuff goes below almost everything
    keepCursorVisible();
    resetBlink();
    update();

}

void Editor::paste() {
    std::string text = cleanText(QApplication::clipboard()->text().toStdString());
    if (text.empty()) {
        return;
    }
    if (hasSelection()) {
        deleteSelection();
    }
    buf.cursor = insertText(buf, buf.cursor, text);
    anchor = buf.cursor;

    //this stuff goes below almost everything
    keepCursorVisible();
    resetBlink();
    update();
}