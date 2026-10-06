#pragma once
#include <QWidget>

// Editor = the area of the window that shows the text.
// It's a QWidget, plus our own drawing.
class Editor : public QWidget {
protected:
    // Qt calls this whenever the editor needs to be drawn.
    // Declaration only — the body is in editor.cpp.
    void paintEvent(QPaintEvent* event) override;
};