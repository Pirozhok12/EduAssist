#ifndef COURSEMANAGER_H
#define COURSEMANAGER_H

#include <QObject>
#include <QVariantList>
#include <QVariantMap>
#include "databasemanager.h"

class CourseManager : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QVariantList courses READ courses NOTIFY coursesChanged)

public:
    explicit CourseManager(QObject *parent = nullptr);

    // Получить все курсы
    QVariantList courses() const;

    // Инициализация БД
    Q_INVOKABLE bool initDatabase(const QString &dbPath = "courses.db");

    // Добавление
    Q_INVOKABLE void addCourse(const QString &name);
    Q_INVOKABLE void addSubject(int courseIndex, const QString &name);
    Q_INVOKABLE void addTask(int courseIndex, int subjectIndex, const QString &name);

    // Изменение
    Q_INVOKABLE void updateTaskGrade(int courseIndex, int subjectIndex, int taskIndex, const QString &grade);
    Q_INVOKABLE void updateTaskDate(int courseIndex, int subjectIndex, int taskIndex, const QString &date);

    // Удаление
    Q_INVOKABLE void removeCourse(int courseIndex);
    Q_INVOKABLE void removeSubject(int courseIndex, int subjectIndex);
    Q_INVOKABLE void removeTask(int courseIndex, int subjectIndex, int taskIndex);

signals:
    void coursesChanged();

private:
    QVariantList m_courses;
    DatabaseManager *m_dbManager;

    // Вспомогательные методы
    void loadDataFromDatabase();
    void rebuildCoursesFromDatabase();

    // Поиск ID в БД по индексам
    int getCourseIdByIndex(int courseIndex) const;
    int getSubjectIdByIndex(int courseIndex, int subjectIndex) const;
    int getTaskIdByIndex(int courseIndex, int subjectIndex, int taskIndex) const;
};

#endif // COURSEMANAGER_H
