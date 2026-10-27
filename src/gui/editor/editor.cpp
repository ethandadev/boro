//src/gui/editor.cpp

// main editor file thingy 
#include "editor.hpp"
#include "config.hpp"
#include <QFont>
#include <string>
#include <algorithm> // for std::max and std::min

//set font and stuff
Editor::Editor(QWidget* parent) : QWidget(parent) {
    //sets the jetbrains mono font or else it looks so bad.
    QFont font;
    font.setFamilies({                              // { } builds a list, like Python's [ ]
        QString::fromStdString(FONT_FAMILY),
        QString::fromStdString(FALLBACK_FONT),
    });
    font.setPointSize(FONT_SIZE); // set fontsize
    setFont(font); //set font
    setFocusPolicy(Qt::StrongFocus); // let this widget receive key presses

    //every like 0.5s it blinks the cursor
    // every time blinkTimer ticks, call this editor's blink()
    connect(&blinkTimer, &QTimer::timeout, this, &Editor::blink);
    resetBlink();

    buf.lines.push_back(""); // the editor always has at least one line

    setCursor(Qt::IBeamCursor); // show the text-editing mouse pointer over the editor
}


void Editor::blink() {
    //blinks the cursor by flipping the cursorVisible flag.
    cursorVisible = !cursorVisible; // flip: true → false, false → true
    update();                       // redraw
}

bool Editor::focusNextPrevChild(bool) {
    //Qt calls this when tab is pressed (i think) and usually it jumps  
    //the focus to a different widget, this stops it.
    return false; // capture tab key presses instead of letting focus jump away
}


int Editor::textLeft() {
    // helper function to return the gutter's width
    // returns an integer of the gutter's width in pixels.
    int digits = std::to_string(buf.lines.size()).size();
    return (digits + 3) * charWidth(); // compute dynamic gutter width based on line count
}

Cursor Editor::positionAt(int x, int y) {
    // helper function to see where the cursor will be
    // after a click at a x and y

    // returns a new Cursor at the correct col and row.
    int row = (y + scrollY) / lineHeight();
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
    // function to reset the blink while you are typing
    // as you don't want the cursor to blink while
    // you are typing something

    cursorVisible = true; // cursor is visible
    blinkTimer.start(500); // reset the blink timer.
}

bool Editor::hasSelection(){
    // helper function to see if theres a selection

    //returns true if there is selection, returns false if there isn't
    return (anchor.col != buf.cursor.col || anchor.row != buf.cursor.row);
}

void Editor::deleteSelection(){
    // deletes selection
    // not used anymore (i think)
    auto [start, end] = selectionRange();       // like:  start, end = self.selection_range()
    eraseRange(buf, start, end);
    anchor = start;
}

std::string Editor::selectedText() {
    // returns the selected text.
    // not used anymore (i think)
    auto [start, end] = selectionRange();       // like:  start, end = self.selection_range()
    return textBetween(buf, start, end);
}

std::pair<Cursor, Cursor> Editor::selectionRange() {
    // returns the selection range.

    if (buf.cursor.row < anchor.row || (buf.cursor.row == anchor.row && buf.cursor.col < anchor.col)) {
        return {buf.cursor, anchor}; // if cursor is after the anchor then returns the opposite
    }

    return {anchor, buf.cursor}; // if the anchor is before the cursor the same ykyk.
    
    // sorry my explantion is really bad
}


// below is a bunch helper functions that im too lazy to explain

int Editor::lineHeight() {
    return fontMetrics().height();
}

int Editor::charWidth() {
    return fontMetrics().horizontalAdvance('0');
}

int Editor::firstVisibleRow() {
    return std::max(0, scrollY / lineHeight());
}

int Editor::lastVisibleRow() {
    int row = (scrollY + height()) / lineHeight();
    return std::min(row, int(buf.lines.size()) - 1);
}

int Editor::maxCols() {
    return width() / charWidth() + 2;
}