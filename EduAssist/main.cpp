#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QDebug>
#include "CourseManager.h"

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    // Налаштування для Settings (для плеєра)
    QCoreApplication::setOrganizationName("MyUniversity");
    QCoreApplication::setOrganizationDomain("univ.example");
    QCoreApplication::setApplicationName("EduAssist");

    // Створюємо CourseManager
    CourseManager courseManager;

    // Ініціалізуємо БД
    if (!courseManager.initDatabase("courses.db")) {
        qDebug() << "Не удалось инициализировать базу данных";
        return -1;
    }

    QQmlApplicationEngine engine;

    // Реєструємо courseManager в QML
    engine.rootContext()->setContextProperty("courseManager", &courseManager);

    QObject::connect(
        &engine,
        &QQmlApplicationEngine::objectCreationFailed,
        &app,
        []() { QCoreApplication::exit(-1); },
        Qt::QueuedConnection);

    engine.loadFromModule("EduAssist", "Main");

    return app.exec();
}
