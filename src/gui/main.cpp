#include <QApplication>
#include <QFontDatabase>
#include <QStyleHints>
#include "editor.hpp"
#include "buffer.hpp"

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);   // the app itself (one per program)

    app.styleHints()->setColorScheme(Qt::ColorScheme::Light);   // needs #include <QStyleHints>

    QFontDatabase::addApplicationFont(":/resources/fonts/JetBrainsMono-Regular.ttf");
    
    Editor window;                  // our editor, shown as the window
    window.setWindowTitle("Boro");
    window.resize(900, 600);


    loadFile(window.buf, "empty.txt");

    window.show();

    return app.exec();              // hand control to Qt's event loop
}