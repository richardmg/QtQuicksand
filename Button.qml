import QtQuick 1.1
import Touch 1.0

Image {
    width: 72
    height: 72
    source: touchArea.pressed ? "gfx/icon_" + text + "_pressed_38x38.svg" : "gfx/icon_" + text + "_unpressed_38x38.svg"

    //Since we are using SVG's make sure we render them to the optimal size
    sourceSize: Qt.size(width * 2, height * 2)

    property string text

    signal clicked()
    signal pressed()
    signal released()

    TouchArea {
        id: touchArea
        anchors.fill: parent

        property bool pressed : false;

        minimumTouches: 1
        maximumTouches: 1

        touchPoints: [
            TouchPoint { id: touchpoint1 }
        ]

        onTouchStart: {
            console.log("button start");
            parent.pressed();
            pressed = true;
        }

        onTouchEnd: {
            console.log("button pressed");
            pressed = false;
            parent.released();
            parent.clicked();
        }
    }


//    MouseArea {
//        id: mouseArea
//        anchors.fill: parent

//        onPressed: {
//            parent.pressed()
//        }
//        onReleased: {
//            parent.released()
//        }
//        onClicked: {
//            parent.clicked()
//        }
//    }

}
