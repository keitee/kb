import QtQuick 2.0

Rectangle {
    id: rectangle1
    width: 609
    height: 224
    color: "transparent"

    Column {
        anchors.centerIn: parent
        id: column1
        x: -53
        y: -242
        width: 732
        height: 437

        // filename is type name.
        // can directly create a Button object by declaring Button{}, similar to
        // a Rectangle{} declaration.
        //
        // access `properties` exposed.
        // New property bindings can be declared within the newly created
        // buttons, effectively overwriting the properties set in Button.qml. 

        Simplebutton {
            id: saveButton
            label: "Save"
            height: rectangle1.height
            width: rectangle1.width/2
            onButtonClick: console.log("simple menu clicked")
        }
    }
}
