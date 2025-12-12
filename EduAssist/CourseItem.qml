import QtQuick 2.15
import QtQuick.Controls 2.15

Rectangle {
    id: root
    width: parent ? parent.width : 300
    height: 50
    color: mouseArea.containsMouse ? "#f0f0f0" : "white"
    radius: 4

    property int itemIndex: -1
    property string itemText: ""
    property bool isCourse: true

    signal clicked(int index)

    border.width: 1
    border.color: "#d0d0d0"

    Row {
        anchors.fill: parent
        anchors.margins: 10
        spacing: 10

        Label {
            width: parent.width - (isCourse ? arrow.width + 10 : 0)
            text: itemText
            font.pixelSize: 14
            elide: Text.ElideRight
        }

        Label {
            id: arrow
            text: "›"
            font.pixelSize: 18
            color: "#888"
            visible: isCourse
        }
    }

    MouseArea {
        id: mouseArea
        anchors.fill: parent
        hoverEnabled: true
        onClicked: {
            if (isCourse) {
                root.clicked(itemIndex);
            }
        }
    }
}
