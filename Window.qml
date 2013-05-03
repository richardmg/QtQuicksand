import QtQuick 1.0

Item {
    id: window
    anchors.fill: parent
    opacity: 0.0
    scale: 0.0

    states: [
        State {
            name: "show"
            when: main.currentWindow === window
            PropertyChanges { target: window; opacity: 1.0; scale: 1.0; enabled: true }
        },
        State {
            name: "hide"
            when: main.currentWindow !== window
            PropertyChanges { target: window; opacity: 0.0; scale: 0.0; enabled: false }
        }
    ]

    transitions: [
        Transition {
            to: "show"
            SequentialAnimation {
                NumberAnimation { properties: "opacity, scale"; duration: 200 }

            }
        },
        Transition {
            to: "hide"
            SequentialAnimation {
                NumberAnimation { properties: "opacity, scale"; duration: 200 }
            }
        }
    ]
}
