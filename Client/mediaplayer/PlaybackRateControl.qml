

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtMultimedia


//底部调节速率
Item {
    id: root

    required property MediaPlayer mediaPlayer

    RowLayout {
        anchors.fill: parent

        Slider {
            id: slider
            Layout.fillWidth: true
            snapMode: Slider.SnapOnRelease
            enabled: true
            from: 0.5
            to: 2.5
            stepSize: 0.5
            value: 1.0

            onMoved: { mediaPlayer.setPlaybackRate(value) }
        }
        Text {
            text: "Rate " + slider.value + "x"
        }
    }
}
