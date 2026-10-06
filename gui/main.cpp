#include <QApplication>   // the class that represents the whole running app (and owns the event loop)
#include <QWidget>        // the base class for everything visible: windows, buttons, text boxes...

int main(int argc, char* argv[]) {          // same main as terminal boro, Qt needs argc/argv too

    // Create the application object. There's exactly one per program.
    // It sets up the connection to macOS (menu bar, dock icon, keyboard, mouse)
    // and reads any Qt-specific command-line options from argc/argv.
    // Must be created BEFORE any widget.
    QApplication app(argc, argv);

    // Create a plain, empty widget. Because it has no parent widget,
    // Qt turns it into a top-level WINDOW.
    QWidget window;

    window.setWindowTitle("Boro");          // text in the window's title bar
    window.resize(900, 600);                // starting size in pixels: width, height

    // Widgets start hidden. show() asks Qt to put it on screen.
    // It doesn't appear instantly — it appears once the event loop starts below.
    window.show();

    // Hand control to Qt's event loop. This is Qt's version of your
    // `while (true)` loop: it waits for events (key presses, clicks, resizes,
    // repaint requests) and calls the right functions for each one.
    // It only returns when the last window closes, giving back an exit code,
    // which we pass on as main's return value (0 = success, like before).
    return app.exec();
}