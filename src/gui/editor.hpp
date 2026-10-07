// src/gui/editor.hpp
#pragma once
#include <QWidget>
#include <QTimer>
#include "buffer.hpp"          // NEW

class Editor : public QWidget {
public:
    Editor(QWidget* parent = nullptr);
    Buffer buf;                // NEW: the text this editor shows

private:
    bool cursorVisible = true;
    QTimer blinkTimer;


protected:
    void paintEvent(QPaintEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;   // NEW
    void blink();
    bool focusNextPrevChild(bool next) override;
};