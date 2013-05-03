import QtQuick 1.0

BorderImage {
    id: border
    width: 150
    height: 150
    source: pressed ? "images/shadow2.png" : "images/shadow1.png"
    smooth: true
    border.left: 10
    border.top: 10
    border.right: 10
    border.bottom: 10

    property alias content: photo.source
    property bool  pressed: false

    Rectangle {
        anchors {
            fill: parent
            leftMargin: border.pressed ? 8 : 2
            topMargin: border.pressed ? 8 : 2
            rightMargin: border.pressed ? 8 : 8
            bottomMargin: border.pressed ? 8 : 8
        }
        color: "black"
        smooth: true

        Image {
            id: photo
            anchors {
                fill: parent
                leftMargin: 2
                topMargin: 2
                rightMargin: 2
                bottomMargin: 2
            }
            smooth: true
            asynchronous: true

            onStatusChanged: {
                if (status == Image.Error) source = "images/error.png"
            }

            Image {
                anchors.centerIn: parent
                width: 40
                height: 40
                source: "images/busy.png"
                visible: parent.status == Image.Loading
                NumberAnimation on rotation {
                    running: parent.visible
                    from: 0
                    to: 360
                    loops: Animation.Infinite
                    duration: 1200
                }
            }
        }
    }
}
