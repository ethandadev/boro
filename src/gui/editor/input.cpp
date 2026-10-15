#include "editor.hpp"
#include "config.hpp"
#include <QKeyEvent>
#include <QMouseEvent>
#include <QWheelEvent>
#include <algorithm>

void Editor::mouseMoveEvent(QMouseEvent* event) {
    buf.cursor = positionAt(int(event->position().x()), int(event->position().y())); // update cursor while dragging mouse
    keepCursorVisible();
    resetBlink();
    update();
}

void Editor::mousePressEvent(QMouseEvent* event) {
    Cursor pos = positionAt(int(event->position().x()), int(event->position().y()));
    buf.cursor = pos;
    anchor = pos; // update text cursor position on click

    resetBlink();

    update();
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