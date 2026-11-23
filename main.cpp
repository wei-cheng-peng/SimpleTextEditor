#include "mainwindow.h"

#include <QApplication>
#include <QFileInfo>
#include <QTimer>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    MainWindow window;
    window.show();

    // 支持通过命令行参数指定要打开的文件（也便于测试异常路径）
    if (argc > 1) {
        const QString filePath = QString::fromLocal8Bit(argv[1]);
        QTimer::singleShot(0, &window, [&window, filePath] {
            window.loadFromFile(filePath);
        });
    }

    return app.exec();
}
