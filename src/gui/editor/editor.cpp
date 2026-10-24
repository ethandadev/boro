//src/gui/editor.cpp
#include "editor.hpp"
#include "config.hpp"
#include <QFont>
#include <string>
#include <algorithm> // for std::max and std::min

//set font and stuff
Editor::Editor(QWidget* parent) : QWidget(parent) {
    QFont font;
    font.setFamilies({                              // { } builds a list, like Python's [ ]
        QString::fromStdString(FONT_FAMILY),
        QString::fromStdString(FALLBACK_FONT),
    });
    font.setPointSize(FONT_SIZE); // set fontsize
    setFont(font); //set font
    setFocusPolicy(Qt::StrongFocus); // let this widget receive key presses

    // every time blinkTimer ticks, call this editor's blink()
    connect(&blinkTimer, &QTimer::timeout, this, &Editor::blink);
    resetBlink();

    buf.lines.push_back(""); // the editor always has at least one line

    setCursor(Qt::IBeamCursor); // show the text-editing mouse pointer over the editor
}


void Editor::blink() {
    cursorVisible = !cursorVisible; // flip: true → false, false → true
    update();                       // redraw
}

bool Editor::focusNextPrevChild(bool) {
    return false; // capture tab key presses instead of letting focus jump away
}


int Editor::textLeft() {
    int charWidth = fontMetrics().horizontalAdvance('0');
    int digits = std::to_string(buf.lines.size()).size();
    return (digits + 3) * charWidth; // compute dynamic gutter width based on line count
}

Cursor Editor::positionAt(int x, int y) {
    int lineHeight = fontMetrics().height();
    int row = (y + scrollY) / lineHeight;
    row = std::min(row, int(buf.lines.size())-1);
    row = std::max(row, 0);
    QString line = QString::fromStdString(buf.lines[row]);

    int col = 0;

    int textX = x - textLeft();   // pixels from where the text starts
    // figure out which character column was clicked by checking character widths
    while (col < line.length()) {
        int left = fontMetrics().horizontalAdvance(line.left(col));
        int right = fontMetrics().horizontalAdvance(line.left(col+1));
        if (textX < (left+right) / 2) {
            break;
        }
        col++;
    }

    return Cursor(row, col);
}


void Editor::resetBlink() {
    cursorVisible = true;
    blinkTimer.start(500);
}

bool Editor::hasSelection(){
    return (anchor.col != buf.cursor.col || anchor.row != buf.cursor.row);
}

void Editor::deleteSelection(){
    auto [start, end] = selectionRange();       // like:  start, end = self.selection_range()
    eraseRange(buf, start, end);
    anchor = start;
}

std::string Editor::selectedText() {
    auto [start, end] = selectionRange();       // like:  start, end = self.selection_range()
    return textBetween(buf, start, end);
}

std::pair<Cursor, Cursor> Editor::selectionRange() {
    if (buf.cursor.row < anchor.row || (buf.cursor.row == anchor.row && buf.cursor.col < anchor.col)) {
        return {buf.cursor, anchor};
    }

    return {anchor, buf.cursor};
}