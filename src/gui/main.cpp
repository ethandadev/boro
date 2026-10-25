//src/gui/main.cpp
#include <QApplication>
#include <QFontDatabase>
#include <QStyleHints>
#include <QMainWindow>
#include <QMenuBar>
#include <QMenu>
#include <QAction>
#include <QKeySequence>
#include "editor/editor.hpp"

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);   // the app itself (one per program)

    app.styleHints()->setColorScheme(Qt::ColorScheme::Light);   // needs #include <QStyleHints>

    QFontDatabase::addApplicationFont(":/resources/fonts/JetBrainsMono-Regular.ttf");
    
    QMainWindow window;                      // the real app window
    Editor* editor = new Editor(&window);    // the editor, living inside the window
    window.setCentralWidget(editor);         // the editor fills the window

    //set menu bar shi
    QMenu* fileMenu = window.menuBar()->addMenu("File");      // the File menu
    QAction* newAction = fileMenu->addAction("New");     // an item in it
    QAction* openAction = fileMenu->addAction("Open...");
    QAction* saveAction = fileMenu->addAction("Save");
    newAction->setShortcut(QKeySequence::New);                // Cmd+N on a Mac
    openAction->setShortcut(QKeySequence::Open); 
    saveAction->setShortcut(QKeySequence::Save); 
    QObject::connect(newAction, &QAction::triggered, editor, &Editor::newFile);
    QObject::connect(openAction, &QAction::triggered, editor, &Editor::openFile);
    QObject::connect(saveAction, &QAction::triggered, editor, &Editor::save);

    QMenu* editMenu = window.menuBar()->addMenu("Edit");
    QAction* undoAction = editMenu->addAction("Undo");
    QAction* redoAction = editMenu->addAction("Redo");
    editMenu->addSeparator();
    QAction* cutAction = editMenu->addAction("Cut");
    QAction* copyAction = editMenu->addAction("Copy");
    QAction* pasteAction = editMenu->addAction("Paste");
    editMenu->addSeparator();
    QAction* selectAllAction = editMenu->addAction("Select All");
    undoAction->setShortcut(QKeySequence::Undo);
    redoAction->setShortcut(QKeySequence::Redo);
    copyAction->setShortcut(QKeySequence::Copy);
    cutAction->setShortcut(QKeySequence::Cut);
    pasteAction->setShortcut(QKeySequence::Paste);
    selectAllAction->setShortcut(QKeySequence::SelectAll);
    QObject::connect(undoAction, &QAction::triggered, editor, &Editor::undo);
    QObject::connect(redoAction, &QAction::triggered, editor, &Editor::redo);
    QObject::connect(cutAction, &QAction::triggered, editor, &Editor::cut);
    QObject::connect(copyAction, &QAction::triggered, editor, &Editor::copy);
    QObject::connect(pasteAction, &QAction::triggered, editor, &Editor::paste);
    QObject::connect(selectAllAction, &QAction::triggered, editor, &Editor::selectAll);

    // other random stuff
    window.resize(900, 600);
    window.setWindowTitle("Boro — Untitled");
    window.show();
    editor->setFocus();                      // keys go to the editor straight away
    editor->openFile();

    return app.exec();              // hand control to Qt's event loop
}