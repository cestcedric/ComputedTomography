#include "MainWindow.h"

int main(int argc, char* argv[]) {

    QApplication a(argc, argv);

        MainWindow mw;
        mw.show();
                if (argc > 1) {
                    mw.cmdLoad(argv[1]);
                }

        return a.exec();
}
