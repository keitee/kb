import QtQuick 2.0

// http://doc.qt.io/qt-5/gettingstartedqml.html#declaring-a-textarea
// Getting Started Programming with Qt Quick
// `id` must starts with lowcase
// `type name`, filename, must start with uppercase

Rectangle {
    id: simpleButton

    property string label

    // A buttonClick() signal is declared in Button.qml by placing the signal
    // keyword in front of the signal name. All signals have their handlers
    // automatically created, their names starting with on. As a result, the
    // onButtonClick is buttonClick's handler. The onButtonClick is then
    // assigned an action to perform. In our button example, the onClicked mouse
    // handler will simply call onButtonClick, which displays a text. The
    // onButtonClick enables outside objects to access the Button's mouse area
    // easily.

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

    // click functionality. signals are emitted and the connected slot is
    // called.

    // The MouseArea has many signal handlers that are called during mouse
    // movements within the specified MouseArea boundaries. One of them is
    // onClicked and it is called whenever the acceptable mouse button is
    // clicked, the left click being the default. We can bind actions to the
    // onClicked handler. In our example, console.log() outputs text whenever
    // the mouse area is clicked. The function console.log() is a useful tool
    // for debugging purposes and for outputting text.
    //
    // onClicked is `signal handler`
    // onClicked: console.log(buttonLabel.text + " clicked")

    MouseArea {
        id: buttonMouseArea
        anchors.fill: parent
        onClicked: buttonClick()
    }
}

