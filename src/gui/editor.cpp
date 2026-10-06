#include "editor.hpp"
#include "config.hpp"
#include <QPainter>
#include <QFont>

//set font and stuff
Editor::Editor(QWidget* parent) : QWidget(parent) {
    QFont font;
    font.setFamilies({                              // { } builds a list, like Python's [ ]
        QString::fromStdString(FONT_FAMILY),
        QString::fromStdString(FALLBACK_FONT),
    });
    font.setPointSize(FONT_SIZE);
    setFont(font);
}

void Editor::paintEvent(QPaintEvent*) {
    QPainter painter(this);              // a "pen" for drawing on this widget
    int line_height = fontMetrics().height();
    for (size_t i = 0; i < buf.lines.size(); i++) {
        int y = (int(i) + 1) * line_height;
        painter.drawText(20, y, QString::fromStdString(buf.lines[i]));
    }
}

