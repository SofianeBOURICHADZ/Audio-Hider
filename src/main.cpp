#include <QtWidgets/QApplication>
#include "MainWindow.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    MainWindow window;
    
    // Prompt for audio file immediately on launch
    if (!window.selectInitialFile()) {
        return 0; // Exit if user cancels file selection
    }

    window.show();
    return app.exec();
}