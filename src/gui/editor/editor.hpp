// src/gui/editor.hpp
#pragma once

#include <QWidget>
#include <QTimer>
#include <QPainter>

#include "buffer.hpp" // text buffer and cursor logic

// Qt-based text editor widget
class Editor : public QWidget {
public:
    Editor(QWidget* parent = nullptr);

    Buffer buf;       // text buffer, same as terminal version

    void openFile();  // open an existing file
    void save();      // save current buffer to file
    void newFile();   // clear buffer and start a new file

    void selectAll();

private:
    bool cursorVisible = true; // used for blinking cursor
    QTimer blinkTimer;         // controls cursor blink timing

    int scrollY = 0; // how far down the text is scrolled

    void keepCursorVisible(); // scroll so the cursor stays on screen
    void clampScroll();       // make sure scrollY is within valid range

    bool confirmDiscard(); // ask before losing unsaved changes

    int textLeft(); // x position where text starts (after gutter)

    Cursor positionAt(int x, int y); // convert mouse position to cursor position
    Cursor anchor;                   // selection start position

    void resetBlink();  // restart cursor blinking after input
    void updateTitle(); // update window title with file name / modified state

    void drawGutter(QPainter& painter);    // draw line numbers
    void drawSelection(QPainter& painter); // draw selected text background
    void drawLines(QPainter& painter);     // draw text lines
    void drawCursor(QPainter& painter);    // draw blinking cursor

    bool hasSelection(); // check whether text is selected

    void deleteSelection();

protected:
    void paintEvent(QPaintEvent* event) override;       // redraw editor
    void keyPressEvent(QKeyEvent* event) override;      // handle keyboard input
    void blink();                                       // toggle cursor visibility
    void wheelEvent(QWheelEvent* event) override;       // handle mouse wheel scrolling
    bool focusNextPrevChild(bool next) override;        // stop Tab from leaving editor
    void mousePressEvent(QMouseEvent* event) override;  // handle mouse click / selection start
    void mouseMoveEvent(QMouseEvent* event) override;   // handle dragging to select text
};