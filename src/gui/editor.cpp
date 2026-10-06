#include "editor.hpp"
#include <QPainter>

void Editor::paintEvent(QPaintEvent*) {
    QPainter painter(this);              // a "pen" for drawing on this widget
    painter.drawText(20, 30, "hello");   // x = 20px from left, y = 30px from top
}