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

    // Отримати всі курси
    QVariantList courses() const;

    // Ініціалізація БД
    Q_INVOKABLE bool initDatabase(const QString &dbPath = "courses.db");

    // Додавання
    Q_INVOKABLE void addCourse(const QString &name);
    Q_INVOKABLE void addSubject(int courseIndex, const QString &name);
    Q_INVOKABLE void addTask(int courseIndex, int subjectIndex, const QString &name);

    // Зміна
    Q_INVOKABLE void updateTaskGrade(int courseIndex, int subjectIndex, int taskIndex, const QString &grade);
    Q_INVOKABLE void updateTaskMaxGrade(int courseIndex, int subjectIndex, int taskIndex, const QString &maxGrade);
    Q_INVOKABLE void updateTaskDate(int courseIndex, int subjectIndex, int taskIndex, const QString &date);
    Q_INVOKABLE void updateTaskCompleted(int courseIndex, int subjectIndex, int taskIndex, bool completed);

    // Видалення
    Q_INVOKABLE void removeCourse(int courseIndex);
    Q_INVOKABLE void removeSubject(int courseIndex, int subjectIndex);
    Q_INVOKABLE void removeTask(int courseIndex, int subjectIndex, int taskIndex);

signals:
    void coursesChanged();

private:
    QVariantList m_courses;
    DatabaseManager *m_dbManager;

    // Допоміжні методи
    void loadDataFromDatabase();
    void rebuildCoursesFromDatabase();

    // Пошук ID в БД за індексами
    int getCourseIdByIndex(int courseIndex) const;
    int getSubjectIdByIndex(int courseIndex, int subjectIndex) const;
    int getTaskIdByIndex(int courseIndex, int subjectIndex, int taskIndex) const;
};

#endif // COURSEMANAGER_H
