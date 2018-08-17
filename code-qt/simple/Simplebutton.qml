import QtQuick 2.0

// `id` must starts with lowcase
// `type name, filename, must start with uppercase

Rectangle {
    id: simpleButton

    property string label

    signal buttonClick

    onButtonClick: {
        console.log(buttonLabel.text + " clicked")
    }

    color: "grey"
    width:150; height:75

    Text {
        id: buttonLabel
        anchors.centerIn: parent
        // text: "Simple Button"
        text: label
    }

    MouseArea {
        id: buttonMouseArea
        anchors.fill: parent
        onClicked: buttonClick()
    }
}

