import QtQuick 1.0
import QuickSandModel 1.0

Item {
    id: sandboxpage
    anchors.fill: parent

    signal selectOpenFile

    property int currentTool: SandBox.SANDTOOL_PUSH

    onCurrentToolChanged: {
        sandbox.setTool(currentTool)
    }

    SandBox {
        id: sandbox
        anchors.fill: parent
        onShowToolBar: { toolbar.state = "show" }
        onHideToolBar: { toolbar.state = "" }

    }

    Row {
        id: toolbar
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.bottom: sandboxpage.bottom;
        anchors.bottomMargin: 15;
        height: 80
        spacing: 10
        opacity: 0

        Button {
            text: "new"
            onClicked: sandbox.reset()
        }
        Button {
            text: "add"
            source:(currentTool == SandBox.SANDTOOL_ADD) ? "gfx/icon_add_pressed_38x38.svg" : "gfx/icon_add_unpressed_38x38.svg"
            onClicked: currentTool = SandBox.SANDTOOL_ADD
        }
        Button {
            text: "push"
            source:(currentTool == SandBox.SANDTOOL_PUSH) ? "gfx/icon_push_pressed_38x38.svg" : "gfx/icon_push_unpressed_38x38.svg"
            onClicked: currentTool = SandBox.SANDTOOL_PUSH
        }
        Button {
            text: "eraser"
            source:(currentTool == SandBox.SANDTOOL_SCRATCH_TO_TARGET) ? "gfx/icon_eraser_pressed_38x38.svg" : "gfx/icon_eraser_unpressed_38x38.svg"
            onClicked: currentTool = SandBox.SANDTOOL_SCRATCH_TO_TARGET
        }
        Button {
            text: "tornado"
            source:(currentTool == SandBox.SANDTOOL_PULL) ? "gfx/icon_tornado_pressed_38x38.svg" : "gfx/icon_tornado_unpressed_38x38.svg"
            onClicked: currentTool = SandBox.SANDTOOL_PULL
        }
        Button {
            text: "light"
            source:(currentTool == SandBox.SANDTOOL_LIGHT) ? "gfx/icon_light_direction_pressed_38x38.svg" : "gfx/icon_light_direction_unpressed_38x38.svg"
            onClicked: currentTool = SandBox.SANDTOOL_LIGHT
        }
        states: State {
            name: "show"
            PropertyChanges { target: toolbar; opacity: 1.0; enabled: true }
        }

        transitions: Transition {
            NumberAnimation { properties: "opacity"; duration: 500 }
        }
    }
}
