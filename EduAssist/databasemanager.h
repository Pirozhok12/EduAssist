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

    // Курси
    bool addCourse(const QString &courseName);
    QList<QVariantMap> getAllCourses();
    bool deleteCourse(int courseId);

    // Предмети
    bool addSubject(int courseId, const QString &subjectName);
    QList<QVariantMap> getSubjectsByCourse(int courseId);
    bool deleteSubject(int subjectId);

    // Завдання
    bool addAssignment(int subjectId, const QString &name, const QString &grade, const QString &maxGrade, const QString &date, bool completed);
    QList<QVariantMap> getAssignmentsBySubject(int subjectId);
    bool updateAssignment(int assignmentId, const QString &name, const QString &grade, const QString &maxGrade, const QString &date, bool completed);
    bool deleteAssignment(int assignmentId);

private:
    QSqlDatabase db;
    bool createTables();
};

#endif // DATABASEMANAGER_H
