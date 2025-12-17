#include "CourseManager.h"
#include <QDebug>

CourseManager::CourseManager(QObject *parent)
    : QObject(parent)
    , m_dbManager(new DatabaseManager(this))
{
}

QVariantList CourseManager::courses() const
{
    return m_courses;
}

bool CourseManager::initDatabase(const QString &dbPath)
{
    if (!m_dbManager->initDatabase(dbPath)) {
        qDebug() << "Помилка ініціалізації БД";
        return false;
    }

    loadDataFromDatabase();
    return true;
}

void CourseManager::loadDataFromDatabase()
{
    m_courses.clear();

    // Получаем все курсы из БД
    QList<QVariantMap> dbCourses = m_dbManager->getAllCourses();

    for (const QVariantMap &dbCourse : dbCourses) {
        QVariantMap course;
        course["id"] = dbCourse["id"];
        course["name"] = dbCourse["name"];

        QVariantList subjects;
        int courseId = dbCourse["id"].toInt();

        // Получаем предметы для курса
        QList<QVariantMap> dbSubjects = m_dbManager->getSubjectsByCourse(courseId);

        for (const QVariantMap &dbSubject : dbSubjects) {
            QVariantMap subject;
            subject["id"] = dbSubject["id"];
            subject["name"] = dbSubject["name"];

            QVariantList tasks;
            int subjectId = dbSubject["id"].toInt();

            // Получаем задания для предмета
            QList<QVariantMap> dbAssignments = m_dbManager->getAssignmentsBySubject(subjectId);

            for (const QVariantMap &dbAssignment : dbAssignments) {
                QVariantMap task;
                task["id"] = dbAssignment["id"];
                task["name"] = dbAssignment["name"].toString();
                task["grade"] = dbAssignment["grade"].toString();
                task["max_grade"] = dbAssignment["max_grade"].toString();
                task["date"] = dbAssignment["date"].toString();
                task["completed"] = dbAssignment["completed"].toBool();

                tasks.append(task);
            }

            subject["tasks"] = tasks;
            subjects.append(subject);
        }

        course["subjects"] = subjects;
        m_courses.append(course);
    }

    emit coursesChanged();
}

void CourseManager::rebuildCoursesFromDatabase()
{
    loadDataFromDatabase();
}

int CourseManager::getCourseIdByIndex(int courseIndex) const
{
    if (courseIndex < 0 || courseIndex >= m_courses.size())
        return -1;

    return m_courses[courseIndex].toMap()["id"].toInt();
}

int CourseManager::getSubjectIdByIndex(int courseIndex, int subjectIndex) const
{
    if (courseIndex < 0 || courseIndex >= m_courses.size())
        return -1;

    QVariantMap course = m_courses[courseIndex].toMap();
    QVariantList subjects = course["subjects"].toList();

    if (subjectIndex < 0 || subjectIndex >= subjects.size())
        return -1;

    return subjects[subjectIndex].toMap()["id"].toInt();
}

int CourseManager::getTaskIdByIndex(int courseIndex, int subjectIndex, int taskIndex) const
{
    if (courseIndex < 0 || courseIndex >= m_courses.size())
        return -1;

    QVariantMap course = m_courses[courseIndex].toMap();
    QVariantList subjects = course["subjects"].toList();

    if (subjectIndex < 0 || subjectIndex >= subjects.size())
        return -1;

    QVariantMap subject = subjects[subjectIndex].toMap();
    QVariantList tasks = subject["tasks"].toList();

    if (taskIndex < 0 || taskIndex >= tasks.size())
        return -1;

    return tasks[taskIndex].toMap()["id"].toInt();
}

void CourseManager::addCourse(const QString &name)
{
    if (m_dbManager->addCourse(name)) {
        rebuildCoursesFromDatabase();
    }
}

void CourseManager::addSubject(int courseIndex, const QString &name)
{
    int courseId = getCourseIdByIndex(courseIndex);
    if (courseId == -1) {
        qDebug() << "Невірний індекс курсу:" << courseIndex;
        return;
    }

    if (m_dbManager->addSubject(courseId, name)) {
        rebuildCoursesFromDatabase();
    }
}

void CourseManager::addTask(int courseIndex, int subjectIndex, const QString &name)
{
    int subjectId = getSubjectIdByIndex(courseIndex, subjectIndex);
    if (subjectId == -1) {
        qDebug() << "Невірний індекс предмету. Курс:" << courseIndex << "Предмет:" << subjectIndex;
        return;
    }

    qDebug() << "Спроба додати завдання:" << name << "до предмету з ID:" << subjectId;

    // Додаємо завдання з порожніми значеннями та completed=false
    if (m_dbManager->addAssignment(subjectId, name, "", "", "", false)) {
        qDebug() << "Завдання успішно додано, перезавантажуємо дані";
        rebuildCoursesFromDatabase();
    } else {
        qDebug() << "Не вдалося додати завдання";
    }
}

void CourseManager::updateTaskGrade(int courseIndex, int subjectIndex, int taskIndex, const QString &grade)
{
    int taskId = getTaskIdByIndex(courseIndex, subjectIndex, taskIndex);
    if (taskId == -1) {
        qDebug() << "Невірний індекс завдання";
        return;
    }

    // Получаем текущие данные задания
    QVariantMap course = m_courses[courseIndex].toMap();
    QVariantList subjects = course["subjects"].toList();
    QVariantMap subject = subjects[subjectIndex].toMap();
    QVariantList tasks = subject["tasks"].toList();
    QVariantMap task = tasks[taskIndex].toMap();

    QString currentName = task["name"].toString();
    QString currentMaxGrade = task["max_grade"].toString();
    QString currentDate = task["date"].toString();
    bool currentCompleted = task["completed"].toBool();

    if (m_dbManager->updateAssignment(taskId, currentName, grade, currentMaxGrade, currentDate, currentCompleted)) {
        // Обновляем локально без полной перезагрузки
        task["grade"] = grade;
        tasks[taskIndex] = task;
        subject["tasks"] = tasks;
        subjects[subjectIndex] = subject;
        course["subjects"] = subjects;
        m_courses[courseIndex] = course;
        emit coursesChanged();
    }
}

void CourseManager::updateTaskMaxGrade(int courseIndex, int subjectIndex, int taskIndex, const QString &maxGrade)
{
    int taskId = getTaskIdByIndex(courseIndex, subjectIndex, taskIndex);
    if (taskId == -1) {
        qDebug() << "Невірний індекс завдання";
        return;
    }

    // Получаем текущие данные задания
    QVariantMap course = m_courses[courseIndex].toMap();
    QVariantList subjects = course["subjects"].toList();
    QVariantMap subject = subjects[subjectIndex].toMap();
    QVariantList tasks = subject["tasks"].toList();
    QVariantMap task = tasks[taskIndex].toMap();

    QString currentName = task["name"].toString();
    QString currentGrade = task["grade"].toString();
    QString currentDate = task["date"].toString();
    bool currentCompleted = task["completed"].toBool();

    if (m_dbManager->updateAssignment(taskId, currentName, currentGrade, maxGrade, currentDate, currentCompleted)) {
        // Обновляем локально без полной перезагрузки
        task["max_grade"] = maxGrade;
        tasks[taskIndex] = task;
        subject["tasks"] = tasks;
        subjects[subjectIndex] = subject;
        course["subjects"] = subjects;
        m_courses[courseIndex] = course;
        emit coursesChanged();
    }
}

void CourseManager::updateTaskDate(int courseIndex, int subjectIndex, int taskIndex, const QString &date)
{
    int taskId = getTaskIdByIndex(courseIndex, subjectIndex, taskIndex);
    if (taskId == -1) {
        qDebug() << "Невірний індекс завдання";
        return;
    }

    // Получаем текущие данные задания
    QVariantMap course = m_courses[courseIndex].toMap();
    QVariantList subjects = course["subjects"].toList();
    QVariantMap subject = subjects[subjectIndex].toMap();
    QVariantList tasks = subject["tasks"].toList();
    QVariantMap task = tasks[taskIndex].toMap();

    QString currentName = task["name"].toString();
    QString currentGrade = task["grade"].toString();
    QString currentMaxGrade = task["max_grade"].toString();
    bool currentCompleted = task["completed"].toBool();

    if (m_dbManager->updateAssignment(taskId, currentName, currentGrade, currentMaxGrade, date, currentCompleted)) {
        // Обновляем локально без полной перезагрузки
        task["date"] = date;
        tasks[taskIndex] = task;
        subject["tasks"] = tasks;
        subjects[subjectIndex] = subject;
        course["subjects"] = subjects;
        m_courses[courseIndex] = course;
        emit coursesChanged();
    }
}

void CourseManager::updateTaskCompleted(int courseIndex, int subjectIndex, int taskIndex, bool completed)
{
    int taskId = getTaskIdByIndex(courseIndex, subjectIndex, taskIndex);
    if (taskId == -1) {
        qDebug() << "Невірний індекс завдання";
        return;
    }

    // Получаем текущие данные задания
    QVariantMap course = m_courses[courseIndex].toMap();
    QVariantList subjects = course["subjects"].toList();
    QVariantMap subject = subjects[subjectIndex].toMap();
    QVariantList tasks = subject["tasks"].toList();
    QVariantMap task = tasks[taskIndex].toMap();

    QString currentName = task["name"].toString();
    QString currentGrade = task["grade"].toString();
    QString currentMaxGrade = task["max_grade"].toString();
    QString currentDate = task["date"].toString();

    if (m_dbManager->updateAssignment(taskId, currentName, currentGrade, currentMaxGrade, currentDate, completed)) {
        // Обновляем локально без полной перезагрузки
        task["completed"] = completed;
        tasks[taskIndex] = task;
        subject["tasks"] = tasks;
        subjects[subjectIndex] = subject;
        course["subjects"] = subjects;
        m_courses[courseIndex] = course;
        emit coursesChanged();
    }
}

void CourseManager::removeCourse(int courseIndex)
{
    int courseId = getCourseIdByIndex(courseIndex);
    if (courseId == -1) {
        qDebug() << "Невірний індекс курсу:" << courseIndex;
        return;
    }

    if (m_dbManager->deleteCourse(courseId)) {
        rebuildCoursesFromDatabase();
    }
}

void CourseManager::removeSubject(int courseIndex, int subjectIndex)
{
    int subjectId = getSubjectIdByIndex(courseIndex, subjectIndex);
    if (subjectId == -1) {
        qDebug() << "Невірний індекс предмету";
        return;
    }

    if (m_dbManager->deleteSubject(subjectId)) {
        rebuildCoursesFromDatabase();
    }
}

void CourseManager::removeTask(int courseIndex, int subjectIndex, int taskIndex)
{
    int taskId = getTaskIdByIndex(courseIndex, subjectIndex, taskIndex);
    if (taskId == -1) {
        qDebug() << "Невірний індекс завдання";
        return;
    }

    if (m_dbManager->deleteAssignment(taskId)) {
        rebuildCoursesFromDatabase();
    }
}
