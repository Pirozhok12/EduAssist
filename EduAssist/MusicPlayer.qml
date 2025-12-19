import QtQuick
import QtMultimedia
import QtCore

Item {
    id: root

    property alias player: player
    property alias audio: audio
    property var tracks: ["qrc:/Maler_Simfoniya_2.mp3", "qrc:/Maler_Simfoniya_10.mp3"]
    property var names: ["Simfoniya_2", "Simfoniya_10"]

    Settings {
        id: appCfg
        property int lastTrack: 0
        property real lastVolume: 0.7
        property real lastPosition: 0
    }

    MediaPlayer {
        id: player
        source: tracks[appCfg.lastTrack]
        audioOutput: AudioOutput {
            id: audio
            volume: appCfg.lastVolume
        }

        Component.onCompleted: {
            position = appCfg.lastPosition
            play()
        }

        onMediaStatusChanged: {
            if (mediaStatus === MediaPlayer.EndOfMedia) {
                appCfg.lastTrack = (appCfg.lastTrack + 1) % tracks.length
                source = tracks[appCfg.lastTrack]
                play()
            }
        }
    }

    function saveState() {
        appCfg.lastPosition = player.position
        appCfg.lastVolume = audio.volume
    }
}
