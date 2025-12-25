import QtQuick 2.15
import QtQuick.Controls 2.15

ListView {
    id: listView
    spacing: 10
    clip: true

    property int selectedCourse: -1
    property int selectedSubject: -1
    property var courses: []

    signal courseSelected(int index)
    signal subjectSelected(int index)

    function calculateProgress(tasks) {
        if (!tasks || tasks.length === 0) return 0;
        var completed = 0;
        for (var i = 0; i < tasks.length; i++) {
            if (tasks[i].completed) completed++;
        }
        return completed / tasks.length;
    }

    function calculateCourseProgress(course) {
        var totalTasks = 0;
        var completedTasks = 0;
        if (!course || !course.subjects) return 0;
        for (var i = 0; i < course.subjects.length; i++) {
            var subject = course.subjects[i];
            if (subject && subject.tasks) {
                totalTasks += subject.tasks.length;
                for (var j = 0; j < subject.tasks.length; j++) {
                    if (subject.tasks[j].completed) completedTasks++;
                }
            }
        }
        return totalTasks > 0 ? completedTasks / totalTasks : 0;
    }

    model: {
        // Перевірка на валідність даних
        if (!courses || courses.length === 0) return 0;

        if (selectedSubject !== -1 && selectedCourse !== -1) {
            if (selectedCourse >= courses.length) return 0;
            var course = courses[selectedCourse];
            if (!course || !course.subjects || selectedSubject >= course.subjects.length) return 0;
            var subject = course.subjects[selectedSubject];
            return (subject && subject.tasks) ? subject.tasks.length : 0;
        } else if (selectedCourse !== -1) {
            if (selectedCourse >= courses.length) return 0;
            var c = courses[selectedCourse];
            return (c && c.subjects) ? c.subjects.length : 0;
        } else {
            return courses.length;
        }
    }

    delegate: Rectangle {
        id: delegateItem
        width: listView.width
        height: selectedSubject !== -1 ? 110 : 85
        color: "#ffffff"
        radius: 14
        border.color: "#e2e8f0"
        border.width: 2

        property bool isTask: selectedSubject !== -1
        property bool isHovered: false

        Behavior on border.color {
            ColorAnimation { duration: 150 }
        }

        // Для курсів та предметів
        Column {
            visible: !isTask
            anchors.fill: parent
            anchors.margins: 14
            spacing: 10

            Row {
                width: parent.width
                height: 30
                spacing: 12

                Label {
                    width: parent.width - arrow.width - deleteBtn.width - 24
                    anchors.top: parent.top
                    anchors.topMargin: 20
                    anchors.left: parent.left
                    anchors.leftMargin: 8
                    text: {
                        // Перевірка на валідність даних
                        if (selectedCourse !== -1) {
                            if (!courses || selectedCourse >= courses.length) return "";
                            var course = courses[selectedCourse];
                            if (!course || !course.subjects || index >= course.subjects.length) return "";
                            var subject = course.subjects[index];
                            return subject ? subject.name : "";
                        } else {
                            if (!courses || index >= courses.length) return "";
                            var c = courses[index];
                            if (!c) return "";
                            var subjectCount = (c.subjects) ? c.subjects.length : 0;
                            var taskCount = 0;
                            if (c.subjects) {
                                for (var i = 0; i < subjectCount; i++) {
                                    if (c.subjects[i] && c.subjects[i].tasks) {
                                        taskCount += c.subjects[i].tasks.length;
                                    }
                                }
                            }
                            return c.name + " • " + subjectCount + " предметів • " + taskCount + " завдань";
                        }
                    }
                    font.pixelSize: 15
                    font.bold: true
                    elide: Text.ElideRight
                    color: "#1e293b"
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

                    background: Rectangle {
                        color: deleteBtn.hovered ? "#e6e6e6" : "#f5f5f5"
                        radius: 3
                    }

                    contentItem: Text {
                        text: deleteBtn.text
                        color: "#ef4444"
                        font: deleteBtn.font
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
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
                    font.pixelSize: 20
                    font.bold: true
                    color: "#8b5cf6"
                    anchors.verticalCenter: parent.verticalCenter
                }
            }

            Row {
                width: parent.width
                spacing: 12

                Rectangle {
                    width: parent.width - progressLabel.width - 12
                    height: 10
                    radius: 5
                    color: "#e2e8f0"

                    Rectangle {
                        width: {
                            var progress;
                            if (selectedCourse !== -1) {
                                if (!courses || selectedCourse >= courses.length) return 0;
                                var course = courses[selectedCourse];
                                if (!course || !course.subjects || index >= course.subjects.length) return 0;
                                var subject = course.subjects[index];
                                progress = subject ? calculateProgress(subject.tasks) : 0;
                            } else {
                                if (!courses || index >= courses.length) return 0;
                                progress = calculateCourseProgress(courses[index]);
                            }
                            return parent.width * progress;
                        }
                        height: parent.height
                        radius: 5

                        gradient: Gradient {
                            GradientStop { position: 0.0; color: "#10b981" }
                            GradientStop { position: 1.0; color: "#34d399" }
                        }

                        Behavior on width {
                            NumberAnimation { duration: 300; easing.type: Easing.OutCubic }
                        }
                    }
                }

                Label {
                    id: progressLabel
                    text: {
                        var progress;
                        if (selectedCourse !== -1) {
                            if (!courses || selectedCourse >= courses.length) return "0%";
                            var course = courses[selectedCourse];
                            if (!course || !course.subjects || index >= course.subjects.length) return "0%";
                            var subject = course.subjects[index];
                            progress = subject ? calculateProgress(subject.tasks) : 0;
                        } else {
                            if (!courses || index >= courses.length) return "0%";
                            progress = calculateCourseProgress(courses[index]);
                        }
                        return Math.round(progress * 100) + "%";
                    }
                    font.pixelSize: 13
                    font.bold: true
                    color: "#10b981"
                    anchors.verticalCenter: parent.verticalCenter
                }
            }

            MouseArea {
                anchors.fill: parent
                anchors.rightMargin: deleteBtn.width + arrow.width + 24
                hoverEnabled: true
                cursorShape: Qt.PointingHandCursor

                onClicked: {
                    if (selectedCourse !== -1) {
                        subjectSelected(index);
                    } else {
                        courseSelected(index);
                    }
                }

                onEntered: {
                    delegateItem.isHovered = true;
                    delegateItem.border.color = "#6366f1";
                }

                onExited: {
                    delegateItem.isHovered = false;
                    delegateItem.border.color = "#e2e8f0";
                }
            }
        }

        // Для завдань
        Column {
            visible: isTask
            anchors.fill: parent
            anchors.margins: 14
            spacing: 8

            Row {
                width: parent.width
                spacing: 12

                CheckBox {
                    id: completedCheckBox
                    checked: {
                        if (!courses || selectedCourse >= courses.length) return false;
                        var course = courses[selectedCourse];
                        if (!course || !course.subjects || selectedSubject >= course.subjects.length) return false;
                        var subject = course.subjects[selectedSubject];
                        if (!subject || !subject.tasks || index >= subject.tasks.length) return false;
                        return subject.tasks[index].completed;
                    }
                    anchors.verticalCenter: parent.verticalCenter

                    onClicked: {
                        courseManager.updateTaskCompleted(selectedCourse, selectedSubject, index, checked);
                    }
                }

                Label {
                    width: parent.width - completedCheckBox.width - taskDeleteBtn.width - 36
                    anchors.verticalCenter: parent.verticalCenter
                    text: {
                        if (!courses || selectedCourse >= courses.length) return "";
                        var course = courses[selectedCourse];
                        if (!course || !course.subjects || selectedSubject >= course.subjects.length) return "";
                        var subject = course.subjects[selectedSubject];
                        if (!subject || !subject.tasks || index >= subject.tasks.length) return "";
                        return subject.tasks[index].name || "";
                    }
                    font.pixelSize: 15
                    font.bold: true
                    elide: Text.ElideRight
                    font.strikeout: {
                        if (!courses || selectedCourse >= courses.length) return false;
                        var course = courses[selectedCourse];
                        if (!course || !course.subjects || selectedSubject >= course.subjects.length) return false;
                        var subject = course.subjects[selectedSubject];
                        if (!subject || !subject.tasks || index >= subject.tasks.length) return false;
                        return subject.tasks[index].completed;
                    }
                    color: {
                        if (!courses || selectedCourse >= courses.length) return "#1e293b";
                        var course = courses[selectedCourse];
                        if (!course || !course.subjects || selectedSubject >= course.subjects.length) return "#1e293b";
                        var subject = course.subjects[selectedSubject];
                        if (!subject || !subject.tasks || index >= subject.tasks.length) return "#1e293b";
                        return subject.tasks[index].completed ? "#94a3b8" : "#1e293b";
                    }
                }

                Button {
                    id: taskDeleteBtn
                    text: "×"
                    width: 30
                    height: 30
                    anchors.verticalCenter: parent.verticalCenter
                    font.pixelSize: 20

                    background: Rectangle {
                        color: taskDeleteBtn.hovered ? "#e6e6e6" : "#f5f5f5"
                        radius: 3
                    }

                    contentItem: Text {
                        text: taskDeleteBtn.text
                        color: "#ef4444"
                        font: taskDeleteBtn.font
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
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
                    spacing: 5

                    Label {
                        text: "Отримана оцінка"
                        font.pixelSize: 10
                        color: "#64748b"
                    }

                    TextField {
                        width: parent.width
                        height: 32
                        placeholderText: "0"
                        text: {
                            if (!courses || selectedCourse >= courses.length) return "";
                            var course = courses[selectedCourse];
                            if (!course || !course.subjects || selectedSubject >= course.subjects.length) return "";
                            var subject = course.subjects[selectedSubject];
                            if (!subject || !subject.tasks || index >= subject.tasks.length) return "";
                            return subject.tasks[index].grade || "";
                        }
                        selectByMouse: true
                        font.pixelSize: 13
                        validator: RegularExpressionValidator { regularExpression: /^\d*\.?\d*$/ }

                        background: Rectangle {
                            color: "#f8fafc"
                            radius: 8
                            border.color: parent.activeFocus ? "#6366f1" : "#e2e8f0"
                            border.width: 1
                        }

                        onEditingFinished: {
                            if (text.trim() !== "") {
                                courseManager.updateTaskGrade(selectedCourse, selectedSubject, index, text);
                            }
                        }
                    }
                }

                Column {
                    width: (parent.width - 20) / 3
                    spacing: 5

                    Label {
                        text: "Максимальна оцінка"
                        font.pixelSize: 10
                        color: "#64748b"
                    }

                    TextField {
                        width: parent.width
                        height: 32
                        placeholderText: "100"
                        text: {
                            if (!courses || selectedCourse >= courses.length) return "";
                            var course = courses[selectedCourse];
                            if (!course || !course.subjects || selectedSubject >= course.subjects.length) return "";
                            var subject = course.subjects[selectedSubject];
                            if (!subject || !subject.tasks || index >= subject.tasks.length) return "";
                            return subject.tasks[index].max_grade || "";
                        }
                        selectByMouse: true
                        font.pixelSize: 13
                        validator: RegularExpressionValidator { regularExpression: /^\d*\.?\d*$/ }

                        background: Rectangle {
                            color: "#f8fafc"
                            radius: 8
                            border.color: parent.activeFocus ? "#6366f1" : "#e2e8f0"
                            border.width: 1
                        }

                        onEditingFinished: {
                            if (text.trim() !== "") {
                                courseManager.updateTaskMaxGrade(selectedCourse, selectedSubject, index, text);
                            }
                        }
                    }
                }

                Column {
                    width: (parent.width - 20) / 3
                    spacing: 5

                    Label {
                        text: "Дедлайн"
                        font.pixelSize: 10
                        color: "#64748b"
                    }

                    TextField {
                        width: parent.width
                        height: 32
                        placeholderText: "ДД.ММ.РРРР"
                        text: {
                            if (!courses || selectedCourse >= courses.length) return "";
                            var course = courses[selectedCourse];
                            if (!course || !course.subjects || selectedSubject >= course.subjects.length) return "";
                            var subject = course.subjects[selectedSubject];
                            if (!subject || !subject.tasks || index >= subject.tasks.length) return "";
                            return subject.tasks[index].date || "";
                        }
                        selectByMouse: true
                        font.pixelSize: 13
                        validator: RegularExpressionValidator { regularExpression: /^\d{0,2}\.?\d{0,2}\.?\d{0,4}$/ }

                        background: Rectangle {
                            color: "#fef2f2"
                            radius: 8
                            border.color: parent.activeFocus ? "#ef4444" : "#fecaca"
                            border.width: 1
                        }

                        onEditingFinished: {
                            var datePattern = /^(\d{2})\.(\d{2})\.(\d{4})$/;
                            if (text.trim() === "" || datePattern.test(text)) {
                                courseManager.updateTaskDate(selectedCourse, selectedSubject, index, text);
                            } else if (text.trim() !== "") {
                                // Якщо формат неправильний, повертаємо старе значення
                                text = Qt.binding(function() {
                                    if (!courses || selectedCourse >= courses.length) return "";
                                    var course = courses[selectedCourse];
                                    if (!course || !course.subjects || selectedSubject >= course.subjects.length) return "";
                                    var subject = course.subjects[selectedSubject];
                                    if (!subject || !subject.tasks || index >= subject.tasks.length) return "";
                                    return subject.tasks[index].date || "";
                                });
                            }
                        }
                    }
                }
            }
        }
    }
}
