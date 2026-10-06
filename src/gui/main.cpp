#include <QApplication>
#include "editor.hpp"

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);   // the app itself (one per program)

    Editor window;                  // our editor, shown as the window
    window.setWindowTitle("Boro");
    window.resize(900, 600);
    window.show();

    return app.exec();              // hand control to Qt's event loop
}