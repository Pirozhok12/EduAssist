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
    QSqlQuery query;

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

    // Таблица заданий
    if (!query.exec("CREATE TABLE IF NOT EXISTS assignments ("
                    "id INTEGER PRIMARY KEY AUTOINCREMENT,"
                    "subject_id INTEGER NOT NULL,"
                    "grade INTEGER NOT NULL,"
                    "date TEXT NOT NULL,"
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
    query.prepare("INSERT INTO courses (name) VALUES (:name)");
    query.bindValue(":name", courseName);

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
    QSqlQuery query("SELECT id, name FROM courses");

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
    query.prepare("DELETE FROM courses WHERE id = :id");
    query.bindValue(":id", courseId);

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
    query.prepare("INSERT INTO subjects (course_id, name) VALUES (:course_id, :name)");
    query.bindValue(":course_id", courseId);
    query.bindValue(":name", subjectName);

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
    query.prepare("SELECT id, name FROM subjects WHERE course_id = :course_id");
    query.bindValue(":course_id", courseId);

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
    query.prepare("DELETE FROM subjects WHERE id = :id");
    query.bindValue(":id", subjectId);

    if (!query.exec()) {
        qDebug() << "Ошибка удаления предмета:" << query.lastError().text();
        return false;
    }

    qDebug() << "Предмет удален, ID:" << subjectId;
    return true;
}

// ЗАДАНИЯ
bool DatabaseManager::addAssignment(int subjectId, int grade, const QString &date)
{
    QSqlQuery query;
    query.prepare("INSERT INTO assignments (subject_id, grade, date) VALUES (:subject_id, :grade, :date)");
    query.bindValue(":subject_id", subjectId);
    query.bindValue(":grade", grade);
    query.bindValue(":date", date);

    if (!query.exec()) {
        qDebug() << "Ошибка добавления задания:" << query.lastError().text();
        return false;
    }

    qDebug() << "Задание добавлено. Оценка:" << grade << "Дата:" << date;
    return true;
}

QList<QVariantMap> DatabaseManager::getAssignmentsBySubject(int subjectId)
{
    QList<QVariantMap> assignments;
    QSqlQuery query;
    query.prepare("SELECT id, grade, date FROM assignments WHERE subject_id = :subject_id ORDER BY date DESC");
    query.bindValue(":subject_id", subjectId);

    if (query.exec()) {
        while (query.next()) {
            QVariantMap assignment;
            assignment["id"] = query.value(0).toInt();
            assignment["grade"] = query.value(1).toInt();
            assignment["date"] = query.value(2).toString();
            assignments.append(assignment);
        }
    }

    return assignments;
}

bool DatabaseManager::updateAssignment(int assignmentId, int grade, const QString &date)
{
    QSqlQuery query;
    query.prepare("UPDATE assignments SET grade = :grade, date = :date WHERE id = :id");
    query.bindValue(":grade", grade);
    query.bindValue(":date", date);
    query.bindValue(":id", assignmentId);

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
    query.prepare("DELETE FROM assignments WHERE id = :id");
    query.bindValue(":id", assignmentId);

    if (!query.exec()) {
        qDebug() << "Ошибка удаления задания:" << query.lastError().text();
        return false;
    }

    qDebug() << "Задание удалено, ID:" << assignmentId;
    return true;
}

// ПОЛУЧЕНИЕ ПОЛНОЙ СТРУКТУРЫ
QList<QVariantMap> DatabaseManager::getFullStructure()
{
    QList<QVariantMap> structure;

    QSqlQuery query("SELECT c.id as course_id, c.name as course_name, "
                    "s.id as subject_id, s.name as subject_name, "
                    "a.id as assignment_id, a.grade, a.date "
                    "FROM courses c "
                    "LEFT JOIN subjects s ON c.id = s.course_id "
                    "LEFT JOIN assignments a ON s.id = a.subject_id "
                    "ORDER BY c.name, s.name, a.date DESC");

    while (query.next()) {
        QVariantMap row;
        row["course_id"] = query.value(0);
        row["course_name"] = query.value(1);
        row["subject_id"] = query.value(2);
        row["subject_name"] = query.value(3);
        row["assignment_id"] = query.value(4);
        row["grade"] = query.value(5);
        row["date"] = query.value(6);
        structure.append(row);
    }

    return structure;
}

// ============================================
// ПРИМЕР ИСПОЛЬЗОВАНИЯ (main.cpp)
// ============================================

#include <QCoreApplication>
#include "databasemanager.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    DatabaseManager dbManager;

    if (!dbManager.initDatabase()) {
        return -1;
    }

    // Добавление курса
    dbManager.addCourse("Программирование");
    dbManager.addCourse("Математика");

    // Получение всех курсов
    QList<QVariantMap> courses = dbManager.getAllCourses();
    qDebug() << "\n=== КУРСЫ ===";
    for (const auto &course : courses) {
        qDebug() << "ID:" << course["id"] << "Название:" << course["name"];
    }

    // Добавление предметов
    int courseId = courses.first()["id"].toInt();
    dbManager.addSubject(courseId, "C++");
    dbManager.addSubject(courseId, "Python");

    // Получение предметов
    QList<QVariantMap> subjects = dbManager.getSubjectsByCourse(courseId);
    qDebug() << "\n=== ПРЕДМЕТЫ ===";
    for (const auto &subject : subjects) {
        qDebug() << "ID:" << subject["id"] << "Название:" << subject["name"];
    }

    // Добавление заданий
    int subjectId = subjects.first()["id"].toInt();
    dbManager.addAssignment(subjectId, 5, "2024-12-01");
    dbManager.addAssignment(subjectId, 4, "2024-12-08");
    dbManager.addAssignment(subjectId, 5, "2024-12-13");

    // Получение заданий
    QList<QVariantMap> assignments = dbManager.getAssignmentsBySubject(subjectId);
    qDebug() << "\n=== ЗАДАНИЯ ===";
    for (const auto &assignment : assignments) {
        qDebug() << "ID:" << assignment["id"]
                 << "Оценка:" << assignment["grade"]
                 << "Дата:" << assignment["date"];
    }

    // Получение полной структуры
    QList<QVariantMap> fullStructure = dbManager.getFullStructure();
    qDebug() << "\n=== ПОЛНАЯ СТРУКТУРА ===";
    for (const auto &row : fullStructure) {
        if (!row["course_name"].isNull()) {
            qDebug() << "Курс:" << row["course_name"]
                     << "| Предмет:" << row["subject_name"]
                     << "| Оценка:" << row["grade"]
                     << "| Дата:" << row["date"];
        }
    }

    return 0;
}
