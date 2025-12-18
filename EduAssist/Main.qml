import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

ApplicationWindow {
    id: root
    visible: true
    width: 900
    height: 650
    title: "Управління завданнями"

    property var courses: courseManager.courses
    property int selectedCourse: -1
    property int selectedSubject: -1
    property string currentDate: ""

    readonly property color primaryColor: "#6366f1"
    readonly property color secondaryColor: "#8b5cf6"
    readonly property color accentColor: "#ec4899"
    readonly property color successColor: "#10b981"
    readonly property color warningColor: "#f59e0b"
    readonly property color dangerColor: "#ef4444"
    readonly property color backgroundColor: "#f8fafc"
    readonly property color cardBackground: "#ffffff"
    readonly property color borderColor: "#e2e8f0"
    readonly property color textPrimary: "#1e293b"
    readonly property color textSecondary: "#64748b"

    Component.onCompleted: {
        var xhr = new XMLHttpRequest();
        xhr.open("GET", "http://worldclockapi.com/api/json/est/now");
        xhr.onreadystatechange = function() {
            if (xhr.readyState === XMLHttpRequest.DONE) {
                if (xhr.status === 200) {
                    var data = JSON.parse(xhr.responseText);
                    var datetime = new Date(data.currentDateTime);
                    var year = datetime.getFullYear();
                    var month = String(datetime.getMonth() + 1).padStart(2, '0');
                    var day = String(datetime.getDate()).padStart(2, '0');
                    currentDate = day + "." + month + "." + year;
                } else {
                    var localDate = new Date();
                    var year = localDate.getFullYear();
                    var month = String(localDate.getMonth() + 1).padStart(2, '0');
                    var day = String(localDate.getDate()).padStart(2, '0');
                    currentDate = day + "." + month + "." + year;
                }
            }
        };
        xhr.send();
    }

    function getUpcomingDeadlines() {
        var allTasks = [];

        for (var i = 0; i < courses.length; i++) {
            var course = courses[i];
            if (!course.subjects) continue;

            for (var j = 0; j < course.subjects.length; j++) {
                var subject = course.subjects[j];
                if (!subject.tasks) continue;

                for (var k = 0; k < subject.tasks.length; k++) {
                    var task = subject.tasks[k];
                    if (!task.completed && task.date && task.date.trim() !== "") {
                        allTasks.push({
                            taskName: task.name,
                            subjectName: subject.name,
                            courseName: course.name,
                            date: task.date,
                            dateObj: parseDate(task.date)
                        });
                    }
                }
            }
        }

        allTasks.sort(function(a, b) {
            if (!a.dateObj || !b.dateObj) return 0;
            return a.dateObj - b.dateObj;
        });

        return allTasks.slice(0, 3);
    }

    function parseDate(dateStr) {
        if (!dateStr || dateStr.trim() === "") return null;
        var parts = dateStr.split(".");
        if (parts.length !== 3) return null;
        var day = parseInt(parts[0]);
        var month = parseInt(parts[1]) - 1;
        var year = parseInt(parts[2]);
        if (isNaN(day) || isNaN(month) || isNaN(year)) return null;
        return new Date(year, month, day);
    }

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

    Rectangle {
        anchors.fill: parent
        color: backgroundColor
    }

    DeadlinePanel {
        id: deadlinePanel
        anchors.top: parent.top
        anchors.right: parent.right
        anchors.margins: 20
        currentDate: root.currentDate
        upcomingDeadlines: getUpcomingDeadlines()
        z: 1
    }

    Column {
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: deadlinePanel.left
        anchors.bottom: parent.bottom
        anchors.margins: 20
        spacing: 15

        Row {
            id: header
            width: parent.width
            height: 45
            spacing: 12

            Rectangle {
                id: backBtn
                width: 80
                height: 40
                radius: 10

                property bool hovered: false


                visible: selectedCourse !== -1 || selectedSubject !== -1

                color: hovered ? Qt.darker(root.primaryColor, 1.1) : root.primaryColor

                Text {
                    text: "Назад"
                    color: "white"
                    font.pixelSize: 14
                    font.bold: true
                    anchors.centerIn: parent
                }

                MouseArea {
                    anchors.fill: parent
                    hoverEnabled: true
                    onEntered: backBtn.hovered = true
                    onExited: backBtn.hovered = false
                    onClicked: {
                        if (selectedSubject !== -1) {
                            selectedSubject = -1;
                        } else if (selectedCourse !== -1) {
                            selectedCourse = -1;
                        }
                    }
                }
            }
            Label {
                width: parent.width - (backBtn.visible ? backBtn.width + 12 : 0)
                height: 40
                text: {
                    if (selectedSubject !== -1) {
                        return courses[selectedCourse].subjects[selectedSubject].name;
                    } else if (selectedCourse !== -1) {
                        return courses[selectedCourse].name;
                    } else {
                        return "Мої курси";
                    }
                }
                font.pixelSize: 26
                font.bold: true
                color: root.textPrimary
                verticalAlignment: Text.AlignVCenter
            }
        }

        Row {
            width: parent.width
            height: 50
            spacing: 12

            TextField {
                id: inputField
                width: parent.width - addBtn.width - 12
                height: 50
                placeholderText: {
                    if (selectedSubject !== -1) return "Назва завдання";
                    if (selectedCourse !== -1) return "Назва предмету";
                    return "Назва курсу";
                }
                font.pixelSize: 14
                selectByMouse: true

                background: Rectangle {
                    color: root.cardBackground
                    radius: 12
                    border.color: inputField.activeFocus ? root.primaryColor : root.borderColor
                    border.width: 2
                }
            }

            Rectangle {
                id: addBtn
                width: 100
                height: 50
                radius: 12

                property bool hovered: false
                property bool enabled: inputField.text.trim() !== ""

                color: enabled ?
                       (hovered ? Qt.darker(root.successColor, 1.1) : root.successColor) :
                       "#cbd5e1"

                Text {
                    text: "Додати"
                    color: "white"
                    font.pixelSize: 14
                    font.bold: true
                    anchors.centerIn: parent
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                }

                MouseArea {
                    anchors.fill: parent
                    hoverEnabled: true
                    onEntered: addBtn.hovered = true
                    onExited: addBtn.hovered = false
                    onClicked: {
                        if (addBtn.enabled) {
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
                    cursorShape: addBtn.enabled ? Qt.PointingHandCursor : Qt.ArrowCursor
                }
            }
        }

        ContentList {
            width: parent.width
            height: parent.height - header.height - inputField.height - 30
            selectedCourse: root.selectedCourse
            selectedSubject: root.selectedSubject
            courses: root.courses

            onCourseSelected: root.selectedCourse = index
            onSubjectSelected: root.selectedSubject = index
        }
    }
}
