#include "CourseManager.h"

CourseManager::CourseManager(QObject *parent)
    : QObject(parent)
{
}

QVariantList CourseManager::courses() const
{
    return m_courses;
}

void CourseManager::setCourses(const QVariantList &courses)
{
    if (m_courses != courses) {
        m_courses = courses;
        emit coursesChanged();
    }
}

void CourseManager::addCourse(const QString &name)
{
    QVariantMap course;
    course["name"] = name;
    course["subjects"] = QVariantList();

    m_courses.append(course);
    emit coursesChanged();
}

void CourseManager::addSubject(int courseIndex, const QString &name)
{
    if (courseIndex < 0 || courseIndex >= m_courses.size())
        return;

    QVariantMap course = m_courses[courseIndex].toMap();
    QVariantList subjects = course["subjects"].toList();

    QVariantMap subject;
    subject["name"] = name;
    subject["tasks"] = QVariantList();

    subjects.append(subject);
    course["subjects"] = subjects;
    m_courses[courseIndex] = course;

    emit coursesChanged();
}

void CourseManager::addTask(int courseIndex, int subjectIndex, const QString &name)
{
    if (courseIndex < 0 || courseIndex >= m_courses.size())
        return;

    QVariantMap course = m_courses[courseIndex].toMap();
    QVariantList subjects = course["subjects"].toList();

    if (subjectIndex < 0 || subjectIndex >= subjects.size())
        return;

    QVariantMap subject = subjects[subjectIndex].toMap();
    QVariantList tasks = subject["tasks"].toList();

    QVariantMap task;
    task["name"] = name;
    task["grade"] = "";
    task["date"] = "";

    tasks.append(task);
    subject["tasks"] = tasks;
    subjects[subjectIndex] = subject;
    course["subjects"] = subjects;
    m_courses[courseIndex] = course;

    emit coursesChanged();
}

void CourseManager::updateTaskGrade(int courseIndex, int subjectIndex, int taskIndex, const QString &grade)
{
    if (courseIndex < 0 || courseIndex >= m_courses.size())
        return;

    QVariantMap course = m_courses[courseIndex].toMap();
    QVariantList subjects = course["subjects"].toList();

    if (subjectIndex < 0 || subjectIndex >= subjects.size())
        return;

    QVariantMap subject = subjects[subjectIndex].toMap();
    QVariantList tasks = subject["tasks"].toList();

    if (taskIndex < 0 || taskIndex >= tasks.size())
        return;

    QVariantMap task = tasks[taskIndex].toMap();
    task["grade"] = grade;
    tasks[taskIndex] = task;
    subject["tasks"] = tasks;
    subjects[subjectIndex] = subject;
    course["subjects"] = subjects;
    m_courses[courseIndex] = course;

    emit coursesChanged();
}

void CourseManager::updateTaskDate(int courseIndex, int subjectIndex, int taskIndex, const QString &date)
{
    if (courseIndex < 0 || courseIndex >= m_courses.size())
        return;

    QVariantMap course = m_courses[courseIndex].toMap();
    QVariantList subjects = course["subjects"].toList();

    if (subjectIndex < 0 || subjectIndex >= subjects.size())
        return;

    QVariantMap subject = subjects[subjectIndex].toMap();
    QVariantList tasks = subject["tasks"].toList();

    if (taskIndex < 0 || taskIndex >= tasks.size())
        return;

    QVariantMap task = tasks[taskIndex].toMap();
    task["date"] = date;
    tasks[taskIndex] = task;
    subject["tasks"] = tasks;
    subjects[subjectIndex] = subject;
    course["subjects"] = subjects;
    m_courses[courseIndex] = course;

    emit coursesChanged();
}

void CourseManager::removeCourse(int courseIndex)
{
    if (courseIndex < 0 || courseIndex >= m_courses.size())
        return;

    m_courses.removeAt(courseIndex);
    emit coursesChanged();
}

void CourseManager::removeSubject(int courseIndex, int subjectIndex)
{
    if (courseIndex < 0 || courseIndex >= m_courses.size())
        return;

    QVariantMap course = m_courses[courseIndex].toMap();
    QVariantList subjects = course["subjects"].toList();

    if (subjectIndex < 0 || subjectIndex >= subjects.size())
        return;

    subjects.removeAt(subjectIndex);
    course["subjects"] = subjects;
    m_courses[courseIndex] = course;

    emit coursesChanged();
}

void CourseManager::removeTask(int courseIndex, int subjectIndex, int taskIndex)
{
    if (courseIndex < 0 || courseIndex >= m_courses.size())
        return;

    QVariantMap course = m_courses[courseIndex].toMap();
    QVariantList subjects = course["subjects"].toList();

    if (subjectIndex < 0 || subjectIndex >= subjects.size())
        return;

    QVariantMap subject = subjects[subjectIndex].toMap();
    QVariantList tasks = subject["tasks"].toList();

    if (taskIndex < 0 || taskIndex >= tasks.size())
        return;

    tasks.removeAt(taskIndex);
    subject["tasks"] = tasks;
    subjects[subjectIndex] = subject;
    course["subjects"] = subjects;
    m_courses[courseIndex] = course;

    emit coursesChanged();
}
