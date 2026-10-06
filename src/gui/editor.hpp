// src/gui/editor.hpp
#pragma once
#include <QWidget>
#include "buffer.hpp"          // NEW

class Editor : public QWidget {
public:
    Editor(QWidget* parent = nullptr);
    Buffer buf;                // NEW: the text this editor shows

protected:
    void paintEvent(QPaintEvent* event) override;
};