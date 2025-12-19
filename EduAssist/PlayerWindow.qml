import QtQuick
import QtQuick.Controls
import QtQuick.LocalStorage

ApplicationWindow {
    id: playerWindow
    width: 250
    height: 200
    minimumWidth: 250
    minimumHeight: 200
    maximumWidth: 300
    maximumHeight: 250
    title: "QML Аудіоплеєр"

    property var musicPlayer

    function getDatabase() {
        return LocalStorage.openDatabaseSync(
            "PlayerDB",
            "1.0",
            "Track play counter",
            100000
        )
    }

    function initDB() {
        var db = getDatabase()
        db.transaction(function(tx) {
            tx.executeSql("CREATE TABLE IF NOT EXISTS plays(name TEXT PRIMARY KEY, count INTEGER)")
        })
    }

    function increasePlayCount(trackName) {
        var db = getDatabase()
        db.transaction(function(tx) {
            var res = tx.executeSql("SELECT count FROM plays WHERE name = ?", [trackName])
            if (res.rows.length === 0) {
                tx.executeSql("INSERT INTO plays(name, count) VALUES(?, ?)", [trackName, 1])
            } else {
                var newCount = res.rows.item(0).count + 1
                tx.executeSql("UPDATE plays SET count = ? WHERE name = ?", [newCount, trackName])
            }
        })
    }

    function getPlayCount(trackName) {
        var db = getDatabase()
        var count = 0
        db.transaction(function(tx) {
            var res = tx.executeSql("SELECT count FROM plays WHERE name = ?", [trackName])
            if (res.rows.length > 0)
                count = res.rows.item(0).count
        })
        return count
    }

    Column {
        anchors.fill: parent
        anchors.margins: 10
        spacing: 10

        ComboBox {
            id: trackSelector
            anchors.horizontalCenter: parent.horizontalCenter
            width: parent.width
            model: musicPlayer ? musicPlayer.names : []
            currentIndex: musicPlayer ? getCurrentTrackIndex() : 0

            function getCurrentTrackIndex() {
                var currentSource = musicPlayer.player.source.toString()
                for (var i = 0; i < musicPlayer.tracks.length; i++) {
                    if (currentSource.indexOf(musicPlayer.tracks[i]) !== -1) {
                        return i
                    }
                }
                return 0
            }

            onCurrentIndexChanged: {
                if (musicPlayer && currentIndex >= 0) {
                    musicPlayer.player.source = musicPlayer.tracks[currentIndex]
                    musicPlayer.player.play()
                    increasePlayCount(musicPlayer.names[currentIndex])
                    updatePlayCountLabel()
                }
            }
        }

        Grid {
            id: controlRow
            anchors.horizontalCenter: parent.horizontalCenter
            columns: 6
            spacing: 5

            Button {
                text: "<<"
                implicitWidth: 35
                implicitHeight: 35
                onClicked: {
                    trackSelector.currentIndex = (trackSelector.currentIndex - 1 + musicPlayer.tracks.length) % musicPlayer.tracks.length
                }
            }
            Button {
                text: "-5с"
                implicitWidth: 35
                implicitHeight: 35
                onClicked: {
                    musicPlayer.player.position = Math.max(0, musicPlayer.player.position - 5000)
                }
            }
            Button {
                text: ">"
                implicitWidth: 35
                implicitHeight: 35
                onClicked: {
                    musicPlayer.player.play()
                }
            }
            Button {
                text: "||"
                implicitWidth: 35
                implicitHeight: 35
                onClicked: {
                    musicPlayer.player.pause()
                }
            }
            Button {
                text: "+5с"
                implicitWidth: 35
                implicitHeight: 35
                onClicked: {
                    musicPlayer.player.position = Math.min(musicPlayer.player.duration, musicPlayer.player.position + 5000)
                }
            }
            Button {
                text: ">>"
                implicitWidth: 35
                implicitHeight: 35
                onClicked: {
                    trackSelector.currentIndex = (trackSelector.currentIndex + 1) % musicPlayer.tracks.length
                }
            }
        }

        Row {
            anchors.horizontalCenter: parent.horizontalCenter
            spacing: 10
            width: parent.width

            Label {
                text: "Гучність:"
                anchors.verticalCenter: parent.verticalCenter
            }
            Slider {
                id: volumeSlider
                from: 0
                to: 1
                stepSize: 0.01
                value: musicPlayer ? musicPlayer.audio.volume : 0.7
                width: parent.width - 80
                anchors.verticalCenter: parent.verticalCenter
                onValueChanged: {
                    if (musicPlayer) {
                        musicPlayer.audio.volume = value
                    }
                }
            }
        }

        Text {
            id: playCountLabel
            anchors.horizontalCenter: parent.horizontalCenter
            text: "Прослуховувань: 0"
            font.bold: true
        }
    }

    Component.onCompleted: {
        initDB()
        updatePlayCountLabel()
    }

    onClosing: {
        if (musicPlayer) {
            musicPlayer.saveState()
        }
    }

    function updatePlayCountLabel() {
        if (musicPlayer && trackSelector.currentIndex >= 0)
            playCountLabel.text = "Прослуховувань: " + getPlayCount(musicPlayer.names[trackSelector.currentIndex])
    }
}
