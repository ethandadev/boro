// undo / redo stuff, rlly rlly complicated lmfao
#include "editor.hpp"
#include "config.hpp"

// swap oldText (which starts at pos) for newText
void Editor::applyChange(Cursor pos, const std::string& oldText, const std::string& newText) {
    eraseRange(buf, pos, endOf(pos, oldText));
    insertText(buf, pos, newText);
    anchor = buf.cursor;
}

// every text change goes through here
void Editor::replace(Cursor start, Cursor end, const std::string& text) {
    Edit edit{start, textBetween(buf, start, end), text};
    if (edit.removed.empty() && edit.inserted.empty()) {
        return; // nothing changed
    }
    applyChange(start, edit.removed, edit.inserted);
    remember(edit);
    redoStack.clear(); // a new edit throws away the redo future
}

void Editor::replaceSelection(const std::string& text) {
    auto [start, end] = selectionRange();
    replace(start, end, text); // no selection = empty range = plain insert
}

void Editor::remember(const Edit& edit) {
    if (canMerge && !undoStack.empty()) {
        Edit& last = undoStack.back();
        bool bothTyping = last.removed.empty() && edit.removed.empty()
                       && edit.inserted.size() == 1 && edit.inserted != "\n"
                       && last.inserted.find('\n') == std::string::npos;
        if (bothTyping && edit.pos == endOf(last.pos, last.inserted)) {
            last.inserted += edit.inserted; // typing right after the last letter: same group
            return;
        }
    }
    undoStack.push_back(edit);
    canMerge = true;
    if (undoStack.size() > MAX_UNDO) {
        undoStack.erase(undoStack.begin());
    }
}

void Editor::undo() {
    if (undoStack.empty()) {
        return;
    }
    Edit edit = undoStack.back();
    undoStack.pop_back();
    applyChange(edit.pos, edit.inserted, edit.removed);
    redoStack.push_back(edit);
    canMerge = false;
    keepCursorVisible();
    resetBlink();
    update();
}

void Editor::redo() {
    if (redoStack.empty()) {
        return;
    }
    Edit edit = redoStack.back();
    redoStack.pop_back();
    applyChange(edit.pos, edit.removed, edit.inserted);
    undoStack.push_back(edit);
    canMerge = false;
    keepCursorVisible();
    resetBlink();
    update();
}
