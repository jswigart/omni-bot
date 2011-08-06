import QtQuick 1.0

import "common" as Common

Rectangle {
    id: mainWindow
    width: 790
    height: 520
    color: "#c2c2c2"
    focus: true

    function setEntityInfo(name,className) {
        entityInfoTxt.text = name + " : " + className;
    }

    Common.entityList {
        id: entList
    }

    Common.mapView {
        id: mapView
        anchors.fill: parent
    }

    Rectangle {
        id: entityInfoPane
        y: 500
        width: 790
        height: 20
        color: "#80ffffff"
        anchors.right: parent.right
        anchors.rightMargin: 0
        anchors.left: parent.left
        anchors.leftMargin: 0
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 0

        Text {
            id: entityInfoTxt
            x: 5
            y: 3
            width: 80
            height: 20
            text: ""
            font.pixelSize: 12
        }
    }
}
