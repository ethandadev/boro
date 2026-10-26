// src/gui/files.cpp

/*
The main file for managing all file related stuff in the GUI.
*/

#include "editor.hpp"
#include <QFileDialog>
#include <QMessageBox>

void Editor::openFile() {
    // shows the macOS file picker dialog

    // updates the buffer automatically, returns nothing.
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
    // if the editor has unsaved changes and a new file was opened or created then a dialog will
    // show to ask the user if it would like to discard changes. 

    // returns true/false depending on user choice
    if (!buf.dirty) {
        return true; // no unsaved changes, safe to discard
    }
    auto answer = QMessageBox::question(this, "Unsaved changes", "Discard your changes to this file?");
    return answer == QMessageBox::Yes; // prompt user if closing with dirty buffer
}

void Editor::save() {
    // saves the open file

    // returns nothing, directly updates the file through the core.
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
    // creates a new file with no location or name with no contents.

    // first calls confirmDiscard() to see if the user as unsaved changes.
    // if the user chooses to not discard changes the newFile() gets cancelled.
    // otherwise it doesnt save and discards the changes.

    if (!confirmDiscard()) {
        return; // user cancelled new file action
    }
    resetState();
    buf.lines.push_back("");
    updateTitle();
    update();
}

void Editor::updateTitle() {
    // automatically updates the title on the boro window.

    // returns nothing directly changes the menu bar title to "Boro - " + fileName or "Unititled"
    QString name = QString::fromStdString((buf.fileName == "") ? "Untitled" : buf.fileName);
    window()->setWindowTitle("Boro - " + name);
}

void Editor::resetState() {
    // helper function to help reset the state when a new file is opened or created.

    // returns nothing directly changes the buffer and editor.
    buf = Buffer();
    anchor = buf.cursor;
    scrollY = 0;
    undoStack.clear();
    redoStack.clear();
    canMerge = false;
}


// it really is a pain in the ass to write all these