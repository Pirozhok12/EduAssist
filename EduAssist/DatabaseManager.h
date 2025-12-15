#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include <QObject>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include <QVariantMap>
#include <QList>

class DatabaseManager : public QObject
{
    Q_OBJECT

public:
    explicit DatabaseManager(QObject *parent = nullptr);
    ~DatabaseManager();

    bool initDatabase(const QString &dbPath = "courses.db");

    // Курсы
    bool addCourse(const QString &courseName);
    QList<QVariantMap> getAllCourses();
    bool deleteCourse(int courseId);

    // Предметы
    bool addSubject(int courseId, const QString &subjectName);
    QList<QVariantMap> getSubjectsByCourse(int courseId);
    bool deleteSubject(int subjectId);

    // Задания
    bool addAssignment(int subjectId, int grade, const QString &date);
    QList<QVariantMap> getAssignmentsBySubject(int subjectId);
    bool updateAssignment(int assignmentId, int grade, const QString &date);
    bool deleteAssignment(int assignmentId);

    // Получение полной структуры
    QList<QVariantMap> getFullStructure();

private:
    QSqlDatabase db;
    bool createTables();
};

#endif // DATABASEMANAGER_H
