// src/gui/editor.hpp
#pragma once
#include <QWidget>
#include <QTimer>
#include "buffer.hpp"         

class Editor : public QWidget {
public:
    Editor(QWidget* parent = nullptr);
    Buffer buf;                // buffer, same as terminal version
    void openFile(); // NEW
    void save();    // NEW
    void newFile();

private:
    bool cursorVisible = true;
    QTimer blinkTimer;
    int scrollY = 0;                 // how far down the text is scrolled
    void keepCursorVisible();
    void clampScroll(); // make sure scrollY is within valid range
    bool confirmDiscard();
    int textLeft();

protected:
    void paintEvent(QPaintEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override; 
    void blink();
    void wheelEvent(QWheelEvent* event) override; // handle mouse wheel scrolling
    bool focusNextPrevChild(bool next) override;
    void mousePressEvent(QMouseEvent* event) override;
};