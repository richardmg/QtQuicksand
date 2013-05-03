import QtQuick 1.0

Rectangle {
    id: main
    color: "black"

    property Window currentWindow: sandWindow

    Item {
        id: contents
        anchors.fill: parent

        Window {
            id: sandWindow
            SandPage {
                id: sandPage
            }
        }
    }
}
