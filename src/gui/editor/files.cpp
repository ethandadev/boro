#include "editor.hpp"
#include <QFileDialog>
#include <QMessageBox>

void Editor::openFile() {
    if (!confirmDiscard()) {
        return; // user chose not to discard changes
    }
    QString path = QFileDialog::getOpenFileName(this, "Open File");   // empty if you pressed cancel
    if (!path.isEmpty()) {                                   // only if a file was actually picked
        resetState();
        loadFile(buf, path.toStdString());
        updateTitle();
        update();    // redraw the editor with the new file
    }

}

bool Editor::confirmDiscard() {
    if (!buf.dirty) {
        return true; // no unsaved changes, safe to discard
    }
    auto answer = QMessageBox::question(this, "Unsaved changes", "Discard your changes to this file?");
    return answer == QMessageBox::Yes; // prompt user if closing with dirty buffer
}

void Editor::save() {
    if (buf.fileName.empty()) {
        QString path = QFileDialog::getSaveFileName(this, "Save File");
        if (path.isEmpty()) {
            return; // cancelled save dialog
        }
        buf.fileName = path.toStdString();
        updateTitle();
    }
    saveFile(buf, buf.fileName);
    update();
}

void Editor::newFile() {
    if (!confirmDiscard()) {
        return; // user cancelled new file action
    }
    resetState();
    buf.lines.push_back("");
    updateTitle();
    update();
}

void Editor::updateTitle() {
    QString name = QString::fromStdString((buf.fileName == "") ? "Untitled" : buf.fileName);
    window()->setWindowTitle("Boro - " + name);
}

void Editor::resetState() {
    buf = Buffer();
    anchor = buf.cursor;
    scrollY = 0;
    undoStack.clear();
    redoStack.clear();
    canMerge = false;
}