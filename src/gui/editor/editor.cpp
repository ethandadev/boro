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
    setFocusPolicy(Qt::StrongFocus); // NEW: let this widget receive key presses

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
    Cursor start = anchor;
    Cursor end = buf.cursor;
    if (end.row < start.row || (end.row == start.row && end.col < start.col)) {
        std::swap(start, end);
    }

    std::string before = buf.lines[start.row].substr(0, start.col);
    std::string after = buf.lines[end.row].substr(end.col);
    buf.lines[start.row] = before + after;

    buf.lines.erase(buf.lines.begin() + start.row + 1, buf.lines.begin() + end.row + 1);

    buf.cursor = start;
    anchor = start;
    buf.dirty = true;
}

std::string Editor::selectedText() {
    Cursor start = anchor;
    Cursor end = buf.cursor;
    if (end.row < start.row || (end.row == start.row && end.col < start.col)) {
        std::swap(start, end);
    }

    if (start.row == end.row) {
        return buf.lines[start.row].substr(start.col, end.col-start.col);
    }
    
    std::string text = buf.lines[start.row].substr(start.col);
    for (size_t row = start.row + 1; row < end.row; row++) {
        text += "\n" + buf.lines[row];
    }
    text += "\n" + buf.lines[end.row].substr(0, end.col);

    return text;
}