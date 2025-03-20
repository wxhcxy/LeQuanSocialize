

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtMultimedia

Item {
    id: root

    required property MediaPlayer mediaPlayer
    property int mediaPlayerState: mediaPlayer.playbackState
    property alias muted: audio.muted
    property alias volume: audio.volume

    height: frame.height

    opacity: 1

    Behavior on opacity { NumberAnimation { duration: 300 }}

    function updateOpacity() {
        //hover is not usable in mobile platforms
        if (Qt.platform.os == "android" || Qt.platform.os == "ios")
            return;

        if (playbackControlHoover.hovered || mediaPlayerState != MediaPlayer.PlayingState || !mediaPlayer.hasVideo)
            root.opacity = 1;
        else
            root.opacity = 1; // 0; TODO: enable opacity change when HoverHandle is fixed
    }

    Connections {
        target: mediaPlayer
        function onPlaybackStateChanged() { updateOpacity() }
        function onHasVideoChanged() { updateOpacity() }
    }

    HoverHandler {
        id: playbackControlHoover
        margin: 50
        onHoveredChanged: updateOpacity()
    }

    Frame {
        id: frame
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom

        background: Rectangle {
            color: "white"
        }

        ColumnLayout {
            id: playbackControlPanel
            anchors.fill: parent
            anchors.leftMargin: 10
            anchors.rightMargin: 10
            //底部
            RowLayout {
                id: playerButtons

                Layout.fillWidth: true

                //播放速率
                PlaybackRateControl {
                    Layout.minimumWidth: 100
                    Layout.maximumWidth: 150
                    Layout.fillHeight: true
                    Layout.fillWidth: true
                    mediaPlayer: root.mediaPlayer
                }

                Item {
                    Layout.fillWidth: true
                }

                //暂停/播放/停止
                RowLayout {
                    Layout.alignment: Qt.AlignCenter
                    id: controlButtons

                    RoundButton {
                        id: pauseButton
                        radius: 50.0
                        icon.source:"qrc:/images/images/pause.png"
                        onClicked: mediaPlayer.pause()
                    }

                    RoundButton {
                        id: playButton
                        radius: 50.0
//                        text: "\u25B6";
                        icon.source:"qrc:/images/images/play.png"
                        onClicked: mediaPlayer.play()
                    }

                    RoundButton {
                        id: stopButton
                        radius: 50.0
                        icon.source:"qrc:/images/images/stop.png"
                        onClicked: mediaPlayer.stop()
                    }
                }

                Item {
                    Layout.fillWidth: true
                }

                //音量高低
                AudioControl {
                    id: audio
                    Layout.minimumWidth: 100
                    Layout.maximumWidth: 150
                    Layout.fillWidth: true
                    mediaPlayer: root.mediaPlayer
                }
            }

            //进度条
            PlaybackSeekControl {
                Layout.fillWidth: true
                mediaPlayer: root.mediaPlayer
            }
        }
    }

    states: [
        State {
            name: "playing"
            when: mediaPlayerState == MediaPlayer.PlayingState
            PropertyChanges { target: pauseButton; visible: true}
            PropertyChanges { target: playButton; visible: false}
            PropertyChanges { target: stopButton; visible: true}
        },
        State {
            name: "stopped"
            when: mediaPlayerState == MediaPlayer.StoppedState
            PropertyChanges { target: pauseButton; visible: false}
            PropertyChanges { target: playButton; visible: true}
            PropertyChanges { target: stopButton; visible: false}
        },
        State {
            name: "paused"
            when: mediaPlayerState == MediaPlayer.PausedState
            PropertyChanges { target: pauseButton; visible: false}
            PropertyChanges { target: playButton; visible: true}
            PropertyChanges { target: stopButton; visible: true}
        }
    ]

}

