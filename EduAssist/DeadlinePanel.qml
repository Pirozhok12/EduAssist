import QtQuick 2.15
import QtQuick.Controls 2.15

Rectangle {
    id: deadlinePanel
    width: 300
    height: 330
    color: "#ffffff"
    radius: 16
    border.color: "#e2e8f0"
    border.width: 2

    property string currentDate: ""
    property var upcomingDeadlines: []

    Column {
        anchors.fill: parent
        anchors.margins: 16
        spacing: 12

        Rectangle {
            width: parent.width
            height: 45
            radius: 12
            gradient: Gradient {
                GradientStop { position: 0.0; color: "#6366f1" }
                GradientStop { position: 1.0; color: "#8b5cf6" }
            }

            Label {
                anchors.centerIn: parent
                text: "Сьогодні: " + currentDate
                font.pixelSize: 15
                font.bold: true
                color: "white"
            }
        }

        Label {
            text: "Найближчі  дедлайни"
            font.pixelSize: 13
            font.bold: true
            color: "#1e293b"
        }

        Column {
            width: parent.width
            spacing: 8

            Repeater {
                model: upcomingDeadlines

                Rectangle {
                    width: parent.width
                    height: 65
                    color: "#f8fafc"
                    radius: 10
                    border.color: "#e2e8f0"
                    border.width: 1

                    Column {
                        anchors.fill: parent
                        anchors.margins: 10
                        spacing: 4

                        Label {
                            width: parent.width
                            text: modelData.subjectName + " - " + modelData.taskName
                            font.pixelSize: 13
                            font.bold: true
                            elide: Text.ElideRight
                            color: "#1e293b"
                        }

                        Rectangle {
                            width: parent.width
                            height: 22
                            color: "#fef2f2"
                            radius: 6
                            border.color: "#fecaca"
                            border.width: 1
                            anchors.bottom: parent.bottom
                            anchors.left: parent.left
                            anchors.right: parent.right

                            Label {
                                anchors.centerIn: parent
                                text: modelData.date
                                font.pixelSize: 11
                                color: "#ef4444"
                                font.bold: true
                            }
                        }
                    }
                }
            }
        }
            }
}
