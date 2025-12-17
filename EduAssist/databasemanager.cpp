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
        qDebug() << "Ошибка открытия БД:" << db.lastError().text();
        return false;
    }

    qDebug() << "База данных успешно открыта";
    return createTables();
}

bool DatabaseManager::createTables()
{
    QSqlQuery query(db);

    // Включаем поддержку внешних ключей
    if (!query.exec("PRAGMA foreign_keys = ON")) {
        qDebug() << "Ошибка включения foreign keys:" << query.lastError().text();
    }

    // Таблица курсов
    if (!query.exec("CREATE TABLE IF NOT EXISTS courses ("
                    "id INTEGER PRIMARY KEY AUTOINCREMENT,"
                    "name TEXT NOT NULL UNIQUE)")) {
        qDebug() << "Ошибка создания таблицы courses:" << query.lastError().text();
        return false;
    }

    // Таблица предметов
    if (!query.exec("CREATE TABLE IF NOT EXISTS subjects ("
                    "id INTEGER PRIMARY KEY AUTOINCREMENT,"
                    "course_id INTEGER NOT NULL,"
                    "name TEXT NOT NULL,"
                    "FOREIGN KEY (course_id) REFERENCES courses(id) ON DELETE CASCADE)")) {
        qDebug() << "Ошибка создания таблицы subjects:" << query.lastError().text();
        return false;
    }

    // Проверяем структуру таблицы assignments
    QSqlQuery checkQuery(db);
    checkQuery.exec("PRAGMA table_info(assignments)");

    QStringList columns;
    while (checkQuery.next()) {
        columns << checkQuery.value(1).toString();
    }

    qDebug() << "Существующие колонки в assignments:" << columns;

    // Если таблица существует, но у неё неправильная структура, пересоздаём её
    if (columns.size() > 0 && !columns.contains("name")) {
        qDebug() << "Старая структура таблицы assignments обнаружена. Пересоздаём...";

        // Удаляем старую таблицу
        if (!query.exec("DROP TABLE IF EXISTS assignments")) {
            qDebug() << "Ошибка удаления старой таблицы assignments:" << query.lastError().text();
        }
    }

    // Таблица заданий с полем name
    if (!query.exec("CREATE TABLE IF NOT EXISTS assignments ("
                    "id INTEGER PRIMARY KEY AUTOINCREMENT,"
                    "subject_id INTEGER NOT NULL,"
                    "name TEXT NOT NULL,"
                    "grade TEXT,"
                    "date TEXT,"
                    "FOREIGN KEY (subject_id) REFERENCES subjects(id) ON DELETE CASCADE)")) {
        qDebug() << "Ошибка создания таблицы assignments:" << query.lastError().text();
        return false;
    }

    qDebug() << "Таблицы успешно созданы";
    return true;
}

// КУРСЫ
bool DatabaseManager::addCourse(const QString &courseName)
{
    QSqlQuery query;
    query.prepare("INSERT INTO courses (name) VALUES (?)");
    query.addBindValue(courseName);

    if (!query.exec()) {
        qDebug() << "Ошибка добавления курса:" << query.lastError().text();
        return false;
    }

    qDebug() << "Курс добавлен:" << courseName;
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
        qDebug() << "Ошибка удаления курса:" << query.lastError().text();
        return false;
    }

    qDebug() << "Курс удален, ID:" << courseId;
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
        qDebug() << "Ошибка добавления предмета:" << query.lastError().text();
        return false;
    }

    qDebug() << "Предмет добавлен:" << subjectName;
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
        qDebug() << "Ошибка удаления предмета:" << query.lastError().text();
        return false;
    }

    qDebug() << "Предмет удален, ID:" << subjectId;
    return true;
}

// ЗАДАНИЯ
bool DatabaseManager::addAssignment(int subjectId, const QString &name, const QString &grade, const QString &date)
{
    QSqlQuery query(db);

    // Пробуем альтернативный способ - через exec с форматированием
    QString queryStr = QString("INSERT INTO assignments (subject_id, name, grade, date) VALUES (%1, '%2', '%3', '%4')")
                           .arg(subjectId)
                           .arg(name)
                           .arg(grade)
                           .arg(date);

    qDebug() << "Выполняем запрос:" << queryStr;

    if (!query.exec(queryStr)) {
        qDebug() << "Ошибка добавления задания:" << query.lastError().text();
        qDebug() << "Код ошибки:" << query.lastError().nativeErrorCode();
        qDebug() << "Subject ID:" << subjectId << "Name:" << name << "Grade:" << grade << "Date:" << date;
        return false;
    }

    qDebug() << "Задание добавлено:" << name;
    return true;
}

QList<QVariantMap> DatabaseManager::getAssignmentsBySubject(int subjectId)
{
    QList<QVariantMap> assignments;
    QSqlQuery query;
    query.prepare("SELECT id, name, grade, date FROM assignments WHERE subject_id = ? ORDER BY id");
    query.addBindValue(subjectId);

    if (query.exec()) {
        while (query.next()) {
            QVariantMap assignment;
            assignment["id"] = query.value(0).toInt();
            assignment["name"] = query.value(1).toString();
            assignment["grade"] = query.value(2).toString();
            assignment["date"] = query.value(3).toString();
            assignments.append(assignment);
        }
    }

    return assignments;
}

bool DatabaseManager::updateAssignment(int assignmentId, const QString &name, const QString &grade, const QString &date)
{
    QSqlQuery query;
    query.prepare("UPDATE assignments SET name = ?, grade = ?, date = ? WHERE id = ?");
    query.addBindValue(name);
    query.addBindValue(grade);
    query.addBindValue(date);
    query.addBindValue(assignmentId);

    if (!query.exec()) {
        qDebug() << "Ошибка обновления задания:" << query.lastError().text();
        return false;
    }

    qDebug() << "Задание обновлено, ID:" << assignmentId;
    return true;
}

bool DatabaseManager::deleteAssignment(int assignmentId)
{
    QSqlQuery query;
    query.prepare("DELETE FROM assignments WHERE id = ?");
    query.addBindValue(assignmentId);

    if (!query.exec()) {
        qDebug() << "Ошибка удаления задания:" << query.lastError().text();
        return false;
    }

    qDebug() << "Задание удалено, ID:" << assignmentId;
    return true;
}
