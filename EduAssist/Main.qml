import QtQuick 2.15
import QtQuick.Controls 2.15

ApplicationWindow {
    id: root
    visible: true
    width: 700
    height: 550
    title: "Управление курсами"

    property var courses: []
    property int selectedCourse: -1
    property int selectedSubject: -1

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
                    return "Курсы";
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
                if (selectedSubject !== -1) return "Название задания";
                if (selectedCourse !== -1) return "Название предмета";
                return "Название курса";
            }
        }

        Button {
            id: addBtn
            text: "Добавить"
            enabled: inputField.text.trim() !== ""
            onClicked: {
                var text = inputField.text.trim();
                if (selectedSubject !== -1) {
                    // Добавляем задание
                    if (!courses[selectedCourse].subjects[selectedSubject].tasks) {
                        courses[selectedCourse].subjects[selectedSubject].tasks = [];
                    }
                    courses[selectedCourse].subjects[selectedSubject].tasks.push({
                        name: text,
                        grade: "",
                        date: ""
                    });
                } else if (selectedCourse !== -1) {
                    // Добавляем предмет
                    courses[selectedCourse].subjects.push({
                        name: text,
                        tasks: []
                    });
                } else {
                    // Добавляем курс
                    courses.push({
                        name: text,
                        subjects: []
                    });
                }
                courses = courses;
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
                // Задания
                var subject = courses[selectedCourse].subjects[selectedSubject];
                return subject.tasks ? subject.tasks.length : 0;
            } else if (selectedCourse !== -1) {
                // Предметы
                return courses[selectedCourse].subjects.length;
            } else {
                // Курсы
                return courses.length;
            }
        }

        delegate: Rectangle {
            id: delegateItem
            width: listView.width
            height: selectedSubject !== -1 ? 70 : 50
            color: "white"
            border.color: "#d0d0d0"
            border.width: 1
            radius: 4

            property bool isTask: selectedSubject !== -1

            // Для курсов и предметов - обычная кнопка
            Rectangle {
                visible: !isTask
                anchors.fill: parent
                color: mouseArea.containsMouse ? "#f0f0f0" : "transparent"
                radius: 4

                Row {
                    anchors.fill: parent
                    anchors.margins: 10
                    spacing: 10

                    Label {
                        width: parent.width - arrow.width - 10
                        text: {
                            if (selectedCourse !== -1) {
                                // Предметы
                                return courses[selectedCourse].subjects[index].name;
                            } else {
                                // Курсы
                                var course = courses[index];
                                var subjectCount = course.subjects.length;
                                var taskCount = 0;
                                for (var i = 0; i < subjectCount; i++) {
                                    if (course.subjects[i].tasks) {
                                        taskCount += course.subjects[i].tasks.length;
                                    }
                                }
                                return course.name + " (Предметов: " + subjectCount + ", Заданий: " + taskCount + ")";
                            }
                        }
                        font.pixelSize: 14
                        elide: Text.ElideRight
                    }

                    Label {
                        id: arrow
                        text: "›"
                        font.pixelSize: 18
                        color: "#888"
                    }
                }

                MouseArea {
                    id: mouseArea
                    anchors.fill: parent
                    hoverEnabled: true
                    onClicked: {
                        if (selectedCourse !== -1) {
                            // Нажатие на предмет - переходим к заданиям
                            selectedSubject = index;
                        } else {
                            // Нажатие на курс - переходим к предметам
                            selectedCourse = index;
                        }
                    }
                }
            }

            // Для заданий - редактируемые поля
            Row {
                visible: isTask
                anchors.fill: parent
                anchors.margins: 10
                spacing: 10

                // Название задания
                Column {
                    width: parent.width - gradeField.width - dateField.width - 20
                    anchors.verticalCenter: parent.verticalCenter

                    Label {
                        width: parent.width
                        text: courses[selectedCourse].subjects[selectedSubject].tasks[index].name
                        font.pixelSize: 14
                        elide: Text.ElideRight
                    }
                }

                // Поле оценки
                TextField {
                    id: gradeField
                    width: 80
                    anchors.verticalCenter: parent.verticalCenter
                    placeholderText: "Оценка"
                    text: courses[selectedCourse].subjects[selectedSubject].tasks[index].grade
                    selectByMouse: true

                    onEditingFinished: {
                        if (selectedSubject !== -1) {
                            courses[selectedCourse].subjects[selectedSubject].tasks[index].grade = text;
                            courses = courses;
                        }
                    }

                    onFocusChanged: {
                        if (!focus) {
                            courses[selectedCourse].subjects[selectedSubject].tasks[index].grade = text;
                            courses = courses;
                        }
                    }
                }

                // Поле даты
                TextField {
                    id: dateField
                    width: 120
                    anchors.verticalCenter: parent.verticalCenter
                    placeholderText: "ДД.ММ.ГГГГ"
                    text: courses[selectedCourse].subjects[selectedSubject].tasks[index].date
                    selectByMouse: true

                    onEditingFinished: {
                        if (selectedSubject !== -1) {
                            courses[selectedCourse].subjects[selectedSubject].tasks[index].date = text;
                            courses = courses;
                        }
                    }

                    onFocusChanged: {
                        if (!focus) {
                            courses[selectedCourse].subjects[selectedSubject].tasks[index].date = text;
                            courses = courses;
                        }
                    }
                }
            }
        }
    }
}
