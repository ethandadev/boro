//src/gui/main.cpp

//main entrypoint for boro
#include <QApplication>
#include <QFontDatabase>
#include <QStyleHints>
#include <QMainWindow>
#include <QMenuBar>
#include <QMenu>
#include <QAction>
#include <QKeySequence>
#include "editor/editor.hpp"

// comments added on Oct 28, 2026 by ethandadev :D

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);   // the app itself (one per program)

    // this line forces the app to be light mode
    // dark mode is a TODO
    app.styleHints()->setColorScheme(Qt::ColorScheme::Light);   // needs #include <QStyleHints>

    QFontDatabase::addApplicationFont(":/resources/fonts/JetBrainsMono-Regular.ttf");
    
    QMainWindow window;                      // the real app window
    Editor* editor = new Editor(&window);    // the editor, living inside the window
    window.setCentralWidget(editor);         // the editor fills the window

    //set menu bar shi
    // its very complicated and alot I know

    // file menu stuff
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

    // edit menu stuff
    QMenu* editMenu = window.menuBar()->addMenu("Edit");
    QAction* undoAction = editMenu->addAction("Undo");
    QAction* redoAction = editMenu->addAction("Redo");
    editMenu->addSeparator(); // adds a seperator line
    QAction* cutAction = editMenu->addAction("Cut");
    QAction* copyAction = editMenu->addAction("Copy");
    QAction* pasteAction = editMenu->addAction("Paste");
    editMenu->addSeparator(); // adds a seperator line
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

    // tbh this looks very un organized and very confusing


    // other random stuff
    window.resize(900, 600); // window size
    window.setWindowTitle("Boro — Untitled"); // default title
    window.show();                      // show the window
    editor->setFocus();                      // keys go to the editor straight away
    editor->openFile();                     // open a file to start

    return app.exec();              // hand control to Qt's event loop
}




// :D