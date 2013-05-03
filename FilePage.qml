import QtQuick 1.0
import Qt.labs.folderlistmodel 1.0

Item {
    anchors.fill: parent

    signal closed(string fileName)

    function refresh() {
        filemodel.folder = "file:/"
    }

    Text {
        width: parent.width
        height: 100
        text: "Select a picture"
        font.pixelSize: 40
        color: "white"
        verticalAlignment: Text.AlignVCenter
    }

    Rectangle {
        y: 100
        width: parent.width
        height: 1
        color: "white"
    }

    Image {
        x: parent.width - width * 2 - 20
        y: 10
        width: 80
        height: 80
        source: backMouseArea.pressed ? "images/back2.png" : "images/back1.png"
        smooth: true
        MouseArea {
            id: backMouseArea
            anchors.fill: parent
            onClicked: {
                filemodel.folder = filemodel.parentFolder
            }
        }
    }

    Image {
        x: parent.width - width
        y: 10
        width: 80
        height: 80
        source: cancelMouseArea.pressed ? "images/cancel2.png" : "images/cancel1.png"
        smooth: true
        rotation: 45
        MouseArea {
            id: cancelMouseArea
            anchors.fill: parent
            onClicked: {
                closed("")
            }
        }
    }

    GridView {
        id: fileview
        anchors.fill: parent
        anchors.topMargin: 110
        clip: true
        cellWidth: 180
        cellHeight: 200

        model: FolderListModel {
            id: filemodel
            nameFilters: [ "*.png", "*.jpg" ]
            folder: "file:/"
        }

        delegate: Photo {
            width: 150
            height: 150
            content: fileIsDir ? "images/folder1.png" : ""
            pressed: mouseArea.pressed

            property bool folder: fileIsDir
            property string name: filePath

            Text {
                x: 0
                y: 160
                width: 150
                font.pixelSize: 30
                color: "white"
                text: fileBaseName
                horizontalAlignment: Text.AlignHCenter
                elide: Text.ElideMiddle
            }

            MouseArea {
                id: mouseArea
                anchors.fill: parent
                onClicked: {
                    if (folder)
                        filemodel.folder = "file:" + filePath
                    else
                        closed(name)
                }
            }
        }
    }
}
