#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>

#include "fileopendialog.h"
#include "imageprovider.h"
#include "messageuicontrol.h"
#include "personaluicontrol.h"
#include "publishpagecontrol.h"
#include "pushpagecontrol.h"
#include "videopagecontrol.h"

ImageProvider *imageProvider = nullptr;
int main(int argc, char *argv[])
{
    qmlRegisterType<PersonalUIControl>("UIControl", 1, 0, "PersonalUIControl");
    qmlRegisterType<PushPageControl>("UIControl", 1, 0, "PushPageControl");
    qmlRegisterType<MessageUIControl>("UIControl", 1, 0, "MessageUIControl");
    qmlRegisterType<VideoPageControl>("UIControl", 1, 0, "VideoPageControl");
    qmlRegisterType<FileOpenDialog>("UIControl", 1, 0, "FileOpenDialog");
    qmlRegisterType<PublishPageControl>("UIControl", 1, 0, "PublishPageControl");

    QGuiApplication app(argc, argv);

    QQmlApplicationEngine engine;
    const QUrl url(QStringLiteral("qrc:/ClientTestQmlRedBook/Main.qml"));
    QObject::connect(
        &engine,
        &QQmlApplicationEngine::objectCreationFailed,
        &app,
        []() { QCoreApplication::exit(-1); },
        Qt::QueuedConnection);

    imageProvider = new ImageProvider();

    engine.addImageProvider("photos", imageProvider);

    engine.load(url);

    return app.exec();
}
