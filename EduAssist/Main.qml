import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

ApplicationWindow {
    id: root
    visible: true
    width: 850
    height: 600
    title: "Управління завданнями"

    property var courses: courseManager.courses
    property int selectedCourse: -1
    property int selectedSubject: -1

    // Функція для підрахунку прогресу
    function calculateProgress(tasks) {
        if (!tasks || tasks.length === 0) return 0;
        var completed = 0;
        for (var i = 0; i < tasks.length; i++) {
            if (tasks[i].completed) completed++;
        }
        return completed / tasks.length;
    }

    // Функція для підрахунку прогресу всього курсу
    function calculateCourseProgress(course) {
        var totalTasks = 0;
        var completedTasks = 0;

        if (!course.subjects) return 0;

        for (var i = 0; i < course.subjects.length; i++) {
            var subject = course.subjects[i];
            if (subject.tasks) {
                totalTasks += subject.tasks.length;
                for (var j = 0; j < subject.tasks.length; j++) {
                    if (subject.tasks[j].completed) completedTasks++;
                }
            }
        }

        return totalTasks > 0 ? completedTasks / totalTasks : 0;
    }

    // Заголовок
    Row {
        id: header
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.margins: 15
        height: 40
        spacing: 10

        Button {
            id: backBtn
            text: "← Назад"
            visible: selectedCourse !== -1 || selectedSubject !== -1
            onClicked: {
                if (selectedSubject !== -1) {
                    selectedSubject = -1;
                } else if (selectedCourse !== -1) {
                    selectedCourse = -1;
                }
            }
        }

        Label {
            width: parent.width - (backBtn.visible ? backBtn.width + 10 : 0)
            text: {
                if (selectedSubject !== -1) {
                    return courses[selectedCourse].subjects[selectedSubject].name;
                } else if (selectedCourse !== -1) {
                    return courses[selectedCourse].name;
                } else {
                    return "Курси";
                }
            }
            font.pixelSize: 20
            font.bold: true

        }
    }

    // Поле ввода и кнопка
    Row {
        id: inputRow
        anchors.top: header.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.margins: 15
        anchors.topMargin: 10
        height: 40
        spacing: 10

        TextField {
            id: inputField
            width: parent.width - addBtn.width - 10
            placeholderText: {
                if (selectedSubject !== -1) return "Назва завдання";
                if (selectedCourse !== -1) return "Назва предмету";
                return "Назва курсу";
            }
        }

        Button {
            id: addBtn
            text: "Додати"
            enabled: inputField.text.trim() !== ""
            onClicked: {
                var text = inputField.text.trim();
                if (selectedSubject !== -1) {
                    courseManager.addTask(selectedCourse, selectedSubject, text);
                } else if (selectedCourse !== -1) {
                    courseManager.addSubject(selectedCourse, text);
                } else {
                    courseManager.addCourse(text);
                }
                inputField.text = "";
            }
        }
    }

    // Разделитель
    Rectangle {
        id: separator
        anchors.top: inputRow.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.margins: 15
        anchors.topMargin: 10
        height: 1
        color: "#e0e0e0"
    }

    // Список
    ListView {
        id: listView
        anchors.top: separator.bottom
        anchors.bottom: parent.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.margins: 15
        anchors.topMargin: 10
        spacing: 5
        clip: true

        model: {
            if (selectedSubject !== -1) {
                var subject = courses[selectedCourse].subjects[selectedSubject];
                return subject.tasks ? subject.tasks.length : 0;
            } else if (selectedCourse !== -1) {
                return courses[selectedCourse].subjects.length;
            } else {
                return courses.length;
            }
        }

        delegate: Rectangle {
            id: delegateItem
            width: listView.width
            height: selectedSubject !== -1 ? 90 : 70
            color: "white"
            border.color: "#d0d0d0"
            border.width: 1
            radius: 4

            property bool isTask: selectedSubject !== -1

            // Для курсов и предметов
            Column {
                visible: !isTask
                anchors.fill: parent
                anchors.margins: 10
                spacing: 5

                Row {
                    width: parent.width
                    height: 30
                    spacing: 10

                    Label {
                        width: parent.width - arrow.width - deleteBtn.width - 20
                        anchors.verticalCenter: parent.verticalCenter
                        anchors.leftMargin: 10
                        text: {
                            if (selectedCourse !== -1) {
                                return courses[selectedCourse].subjects[index].name;
                            } else {
                                var course = courses[index];
                                var subjectCount = course.subjects.length;
                                var taskCount = 0;
                                for (var i = 0; i < subjectCount; i++) {
                                    if (course.subjects[i].tasks) {
                                        taskCount += course.subjects[i].tasks.length;
                                    }
                                }
                                return course.name + " (Предметів: " + subjectCount + ", Завдань: " + taskCount + ")";
                            }
                        }
                        font.pixelSize: 14
                        elide: Text.ElideRight
                        x: 10
                    }

                    Button {
                        id: deleteBtn
                        text: "×"
                        width: 22
                        height: 22

                        anchors.top: parent.top
                        anchors.topMargin: 20
                        anchors.right: parent.right
                        anchors.rightMargin: 8

                        font.pixelSize: 14
                        font.bold: true

                        background: Rectangle {
                            color: deleteBtn.hovered ? "#e6e6e6" : "#f5f5f5"
                            radius: 3
                        }

                        onClicked: {
                            if (selectedCourse !== -1) {
                                courseManager.removeSubject(selectedCourse, index)
                            } else {
                                courseManager.removeCourse(index)
                            }
                        }
                    }

                    Label {
                        id: arrow
                        text: "›"
                        font.pixelSize: 18
                        color: "#888"
                        anchors.verticalCenter: parent.verticalCenter
                    }
                }

                // Прогресс-бар
                Row {
                    width: parent.width
                    spacing: 10

                    ProgressBar {
                        width: parent.width - progressLabel.width - 10
                        value: {
                            if (selectedCourse !== -1) {
                                var subject = courses[selectedCourse].subjects[index];
                                return calculateProgress(subject.tasks);
                            } else {
                                return calculateCourseProgress(courses[index]);
                            }
                        }

                        background: Rectangle {
                            implicitHeight: 8
                            radius: 4
                            color: "#e0e0e0"
                        }

                        contentItem: Item {
                            implicitHeight: 8

                            Rectangle {
                                width: parent.parent.visualPosition * parent.width
                                height: parent.height
                                radius: 4
                                color: "#4CAF50"
                            }
                        }
                    }

                    Label {
                        id: progressLabel
                        text: {
                            var progress;
                            if (selectedCourse !== -1) {
                                var subject = courses[selectedCourse].subjects[index];
                                progress = calculateProgress(subject.tasks);
                            } else {
                                progress = calculateCourseProgress(courses[index]);
                            }
                            return Math.round(progress * 100) + "%";
                        }
                        font.pixelSize: 12
                        color: "#666"
                        anchors.verticalCenter: parent.verticalCenter
                    }
                }

                MouseArea {
                    anchors.fill: parent
                    anchors.rightMargin: deleteBtn.width + arrow.width + 20
                    hoverEnabled: true
                    onClicked: {
                        if (selectedCourse !== -1) {
                            selectedSubject = index;
                        } else {
                            selectedCourse = index;
                        }
                    }
                    onEntered: delegateItem.color = "#f0f0f0"
                    onExited: delegateItem.color = "white"
                }
            }

            // Для заданий
            Column {
                visible: isTask
                anchors.fill: parent
                anchors.margins: 10
                spacing: 5

                Row {
                    width: parent.width
                    spacing: 10

                    CheckBox {
                        id: completedCheckBox
                        checked: courses[selectedCourse].subjects[selectedSubject].tasks[index].completed
                        anchors.verticalCenter: parent.verticalCenter

                        onClicked: {
                            courseManager.updateTaskCompleted(selectedCourse, selectedSubject, index, checked);
                        }
                    }

                    Label {
                        width: parent.width - completedCheckBox.width - taskDeleteBtn.width - 30
                        anchors.verticalCenter: parent.verticalCenter
                        text: courses[selectedCourse].subjects[selectedSubject].tasks[index].name
                        font.pixelSize: 14
                        elide: Text.ElideRight
                        font.strikeout: courses[selectedCourse].subjects[selectedSubject].tasks[index].completed
                    }

                    Button {
                        id: taskDeleteBtn
                        text: "×"
                        width: 30
                        height: 30
                        anchors.verticalCenter: parent.verticalCenter
                        font.pixelSize: 20

                        background: Rectangle {
                            color: deleteBtn.hovered ? "#e6e6e6" : "#f5f5f5"
                            radius: 3
                        }

                        onClicked: {
                            courseManager.removeTask(selectedCourse, selectedSubject, index);
                        }
                    }
                }

                Row {
                    width: parent.width
                    spacing: 10

                    Column {
                        width: (parent.width - 20) / 3
                        spacing: 3

                        Label {
                            text: "Отримана оцінка:"
                            font.pixelSize: 10
                        }

                        TextField {
                            id: gradeField
                            width: parent.width
                            placeholderText: "0"
                            text: courses[selectedCourse].subjects[selectedSubject].tasks[index].grade
                            selectByMouse: true
                            font.pixelSize: 12

                            onEditingFinished: {
                                courseManager.updateTaskGrade(selectedCourse, selectedSubject, index, text);
                            }
                        }
                    }

                    Column {
                        width: (parent.width - 20) / 3
                        spacing: 3

                        Label {
                            text: "Максимальна оцінка:"
                            font.pixelSize: 10
                        }

                        TextField {
                            id: maxGradeField
                            width: parent.width
                            placeholderText: "100"
                            text: courses[selectedCourse].subjects[selectedSubject].tasks[index].max_grade
                            selectByMouse: true
                            font.pixelSize: 12

                            onEditingFinished: {
                                courseManager.updateTaskMaxGrade(selectedCourse, selectedSubject, index, text);
                            }
                        }
                    }

                    Column {
                        width: (parent.width - 20) / 3
                        spacing: 3

                        Label {
                            text: "Dead Line:"
                            font.pixelSize: 10
                        }

                        TextField {
                            id: dateField
                            width: parent.width
                            placeholderText: "ДД.ММ.РРРР"
                            text: courses[selectedCourse].subjects[selectedSubject].tasks[index].date
                            selectByMouse: true
                            font.pixelSize: 12

                            onEditingFinished: {
                                courseManager.updateTaskDate(selectedCourse, selectedSubject, index, text);
                            }
                        }
                    }
                }
            }
        }
    }
}
