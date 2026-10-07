// src/gui/editor.hpp
#pragma once
#include <QWidget>
#include <QTimer>
#include "buffer.hpp"         

class Editor : public QWidget {
public:
    Editor(QWidget* parent = nullptr);
    Buffer buf;                // buffer, same as terminal version

private:
    bool cursorVisible = true;
    QTimer blinkTimer;
    int scrollY = 0;                 // NEW: how far down the text is scrolled
    void keepCursorVisible();
    void clampScroll(); // NEW: make sure scrollY is within valid range

protected:
    void paintEvent(QPaintEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override; 
    void blink();
    void wheelEvent(QWheelEvent* event) override; // NEW: handle mouse wheel scrolling
    bool focusNextPrevChild(bool next) override;
};