#include "databasemanager.h"

DatabaseManager::DatabaseManager(QObject *parent) : QObject(parent)
{
}

DatabaseManager::~DatabaseManager()
{
    if (db.isOpen()) {
        db.close();
    }
}

bool DatabaseManager::initDatabase(const QString &dbPath)
{
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(dbPath);

    if (!db.open()) {
        qDebug() << "Помилка відкриття БД:" << db.lastError().text();
        return false;
    }

    qDebug() << "База даних успішно відкрита";
    return createTables();
}

bool DatabaseManager::createTables()
{
    QSqlQuery query(db);

    // Включаем поддержку внешних ключей
    if (!query.exec("PRAGMA foreign_keys = ON")) {
        qDebug() << "Помилка включення foreign keys:" << query.lastError().text();
    }

    // Таблица курсов
    if (!query.exec("CREATE TABLE IF NOT EXISTS courses ("
                    "id INTEGER PRIMARY KEY AUTOINCREMENT,"
                    "name TEXT NOT NULL UNIQUE)")) {
        qDebug() << "Помилка створення таблиці courses:" << query.lastError().text();
        return false;
    }

    // Таблица предметов
    if (!query.exec("CREATE TABLE IF NOT EXISTS subjects ("
                    "id INTEGER PRIMARY KEY AUTOINCREMENT,"
                    "course_id INTEGER NOT NULL,"
                    "name TEXT NOT NULL,"
                    "FOREIGN KEY (course_id) REFERENCES courses(id) ON DELETE CASCADE)")) {
        qDebug() << "Помилка створення таблиці subjects:" << query.lastError().text();
        return false;
    }

    // Проверяем структуру таблицы assignments
    QSqlQuery checkQuery(db);
    checkQuery.exec("PRAGMA table_info(assignments)");

    QStringList columns;
    while (checkQuery.next()) {
        columns << checkQuery.value(1).toString();
    }

    qDebug() << "Існуючі колонки в assignments:" << columns;

    // Если таблица существует, но у неё неправильная структура, пересоздаём её
    if (columns.size() > 0 && (!columns.contains("name") || !columns.contains("completed") || !columns.contains("max_grade"))) {
        qDebug() << "Стара структура таблиці assignments виявлена. Перестворюємо...";

        // Удаляем старую таблицу
        if (!query.exec("DROP TABLE IF EXISTS assignments")) {
            qDebug() << "Помилка видалення старої таблиці assignments:" << query.lastError().text();
        }
    }

    // Таблица заданий с полями name, completed, max_grade
    if (!query.exec("CREATE TABLE IF NOT EXISTS assignments ("
                    "id INTEGER PRIMARY KEY AUTOINCREMENT,"
                    "subject_id INTEGER NOT NULL,"
                    "name TEXT NOT NULL,"
                    "grade TEXT,"
                    "max_grade TEXT,"
                    "date TEXT,"
                    "completed INTEGER DEFAULT 0,"
                    "FOREIGN KEY (subject_id) REFERENCES subjects(id) ON DELETE CASCADE)")) {
        qDebug() << "Помилка створення таблиці assignments:" << query.lastError().text();
        return false;
    }

    qDebug() << "Таблиці успішно створені";
    return true;
}

// КУРСЫ
bool DatabaseManager::addCourse(const QString &courseName)
{
    QSqlQuery query;
    query.prepare("INSERT INTO courses (name) VALUES (?)");
    query.addBindValue(courseName);

    if (!query.exec()) {
        qDebug() << "Помилка додавання курсу:" << query.lastError().text();
        return false;
    }

    qDebug() << "Курс додано:" << courseName;
    return true;
}

QList<QVariantMap> DatabaseManager::getAllCourses()
{
    QList<QVariantMap> courses;
    QSqlQuery query("SELECT id, name FROM courses ORDER BY id");

    while (query.next()) {
        QVariantMap course;
        course["id"] = query.value(0).toInt();
        course["name"] = query.value(1).toString();
        courses.append(course);
    }

    return courses;
}

bool DatabaseManager::deleteCourse(int courseId)
{
    QSqlQuery query;
    query.prepare("DELETE FROM courses WHERE id = ?");
    query.addBindValue(courseId);

    if (!query.exec()) {
        qDebug() << "Помилка видалення курсу:" << query.lastError().text();
        return false;
    }

    qDebug() << "Курс видалено, ID:" << courseId;
    return true;
}

// ПРЕДМЕТЫ
bool DatabaseManager::addSubject(int courseId, const QString &subjectName)
{
    QSqlQuery query;
    query.prepare("INSERT INTO subjects (course_id, name) VALUES (?, ?)");
    query.addBindValue(courseId);
    query.addBindValue(subjectName);

    if (!query.exec()) {
        qDebug() << "Помилка додавання предмету:" << query.lastError().text();
        return false;
    }

    qDebug() << "Предмет додано:" << subjectName;
    return true;
}

QList<QVariantMap> DatabaseManager::getSubjectsByCourse(int courseId)
{
    QList<QVariantMap> subjects;
    QSqlQuery query;
    query.prepare("SELECT id, name FROM subjects WHERE course_id = ? ORDER BY id");
    query.addBindValue(courseId);

    if (query.exec()) {
        while (query.next()) {
            QVariantMap subject;
            subject["id"] = query.value(0).toInt();
            subject["name"] = query.value(1).toString();
            subjects.append(subject);
        }
    }

    return subjects;
}

bool DatabaseManager::deleteSubject(int subjectId)
{
    QSqlQuery query;
    query.prepare("DELETE FROM subjects WHERE id = ?");
    query.addBindValue(subjectId);

    if (!query.exec()) {
        qDebug() << "Помилка видалення предмету:" << query.lastError().text();
        return false;
    }

    qDebug() << "Предмет видалено, ID:" << subjectId;
    return true;
}

// ЗАДАНИЯ
bool DatabaseManager::addAssignment(int subjectId, const QString &name, const QString &grade, const QString &maxGrade, const QString &date, bool completed)
{
    QSqlQuery query(db);

    // Используем прямой exec с форматированием
    QString queryStr = QString("INSERT INTO assignments (subject_id, name, grade, max_grade, date, completed) VALUES (%1, '%2', '%3', '%4', '%5', %6)")
                           .arg(subjectId)
                           .arg(name)
                           .arg(grade)
                           .arg(maxGrade)
                           .arg(date)
                           .arg(completed ? 1 : 0);

    qDebug() << "Виконуємо запит:" << queryStr;

    if (!query.exec(queryStr)) {
        qDebug() << "Помилка додавання завдання:" << query.lastError().text();
        qDebug() << "Код помилки:" << query.lastError().nativeErrorCode();
        return false;
    }

    qDebug() << "Завдання додано:" << name;
    return true;
}

QList<QVariantMap> DatabaseManager::getAssignmentsBySubject(int subjectId)
{
    QList<QVariantMap> assignments;
    QSqlQuery query;
    query.prepare("SELECT id, name, grade, max_grade, date, completed FROM assignments WHERE subject_id = ? ORDER BY id");
    query.addBindValue(subjectId);

    if (query.exec()) {
        while (query.next()) {
            QVariantMap assignment;
            assignment["id"] = query.value(0).toInt();
            assignment["name"] = query.value(1).toString();
            assignment["grade"] = query.value(2).toString();
            assignment["max_grade"] = query.value(3).toString();
            assignment["date"] = query.value(4).toString();
            assignment["completed"] = query.value(5).toInt() == 1;
            assignments.append(assignment);
        }
    }

    return assignments;
}

bool DatabaseManager::updateAssignment(int assignmentId, const QString &name, const QString &grade, const QString &maxGrade, const QString &date, bool completed)
{
    QSqlQuery query;
    query.prepare("UPDATE assignments SET name = ?, grade = ?, max_grade = ?, date = ?, completed = ? WHERE id = ?");
    query.addBindValue(name);
    query.addBindValue(grade);
    query.addBindValue(maxGrade);
    query.addBindValue(date);
    query.addBindValue(completed ? 1 : 0);
    query.addBindValue(assignmentId);

    if (!query.exec()) {
        qDebug() << "Помилка оновлення завдання:" << query.lastError().text();
        return false;
    }

    qDebug() << "Завдання оновлено, ID:" << assignmentId;
    return true;
}

bool DatabaseManager::deleteAssignment(int assignmentId)
{
    QSqlQuery query;
    query.prepare("DELETE FROM assignments WHERE id = ?");
    query.addBindValue(assignmentId);

    if (!query.exec()) {
        qDebug() << "Помилка видалення завдання:" << query.lastError().text();
        return false;
    }

    qDebug() << "Завдання видалено, ID:" << assignmentId;
    return true;
}
