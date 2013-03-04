import QtQuick 1.0

Rectangle {
    id: entityList
    width: 100

    anchors.top: parent.top
    anchors.left: parent.left
    anchors.bottom: parent.bottom

    color: "#80808080"

    Component.onCompleted: {
    }

    function addEntity(attribs) {
        entityModel.append( attribs )
    }

    function detroyEntity(handle) {
        for (var i = 0; i < entityModel.count; i = i+1)
        {
            if ( entityModel.get( i ).handle == handle )
            {
                entityModel.remove( i )
                break;
            }
        }
    }

    function updateEntity(attribs) {
        console.debug( "updateEntity " + entityModel.count + " handle " + attribs.handle )

        for (var i = 0; i < entityModel.count; i = i + 1)
        {
            if ( entityModel.get( i ).handle == attribs.handle )
            {
                // copy all provided props
                for (var prop in attribs) {
                    console.debug( "setProperty " + prop + " " + attribs[prop])
                    entityModel.setProperty( i, prop, attribs[prop])
                }
                break;
            }
        }
    }

    ListModel {
        id: entityModel
    }
    Component {
        id: entityDelegate
        Row {
            spacing: 10
            Text { text: name }
            Text { text: className }
        }
    }
    ListView {
        id: entityListView
        width: 100
        anchors.fill: parent
        anchors.topMargin: 0
        delegate: entityDelegate
        model: entityModel
    }
}
