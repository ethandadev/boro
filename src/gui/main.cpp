//src/gui/main.cpp
#include <QApplication>
#include <QFontDatabase>
#include <QStyleHints>
#include <QMainWindow>
#include <QMenuBar>
#include <QMenu>
#include <QAction>
#include <QKeySequence>
#include "editor.hpp"
#include "buffer.hpp"

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

    // other random stuff
    window.resize(900, 600);
    window.setWindowTitle("Boro — Untitled");
    window.show();
    editor->setFocus();                      // keys go to the editor straight away
    editor->openFile();

    return app.exec();              // hand control to Qt's event loop
}