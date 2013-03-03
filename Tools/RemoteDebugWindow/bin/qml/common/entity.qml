import QtQuick 1.0
import Qt3D 1.0
import Qt3D.Shapes 1.0

Item3D {
    id: entity
    enabled: true
    parent: viewport.scene

    state: "StateBase"

    // from the remote host
    x: 0
    y: 0
    z: 0

    property int handle: 0
    property string name: "NoName"
    property int classid: 0
    property int teamid: 0
    property real yaw: 0
    property real pitch: 0
    property int health: 1.0
    property int maxhealth: 1.0
    property int armor: 0.0
    property int maxarmor: 0.0
//    property real entRadius : 64.0
//    property real entHeight : 64.0
    property real entSizeX: 5.0
    property real entSizeY: 5.0
    property real entSizeZ: 5.0
    property real fov: 90.0;

    property bool showCylinder: true;
    property bool showBox: true;

    // state properties
    property color teamColor : "white";
    property color activeColor : getActiveColor()

    property string className: "NoClass"
    property url classTexture: ""

    function getActiveColor() {
        if ( health > 0 && maxhealth > 0 )
            return teamColor;
        else return "grey";
    }

    Component.onCompleted: {
        var entAttribs = {
            'handle' : handle,
            'name' : name,
            'className' : className
        }
        //entList.addEntity(entAttribs);
	
	console.debug( "x " + x );
    }
    Component.onDestruction: {
        //entList.detroyEntity(entity.handle);
    }
    onNameChanged: {
        console.debug( "name " + name );
        //entList.updateEntity( { 'handle' : handle, 'name' : name } )
    }
    onClassidChanged: {
        if ( classid == 1 ) {
            entity.state = "StateSoldier"
        } else if ( classid == 2 ) {
            entity.state = "StateMedic"
        } else if ( classid == 3 ) {
            entity.state = "StateEngineer"
        } else if ( classid == 4 ) {
            entity.state = "StateFieldOps"
        } else if ( classid == 5 ) {
            entity.state = "StateCovertOps"
        } else {
            entity.state = "StateBase"
        }
        //entList.updateEntity( { 'handle' : handle, 'className' : className } )
    }
    onTeamidChanged: {
        if ( teamid == 1 ) {
            teamColor = "red";
        } else if ( teamid == 2 ) {
            teamColor = "blue";
        } else {
            teamColor = "white";
        }

        //entList.updateEntity( { 'handle' : handle, 'team' : teamid } )
    }
    onXChanged: {
        console.debug( "x " + x );
    }
    onYChanged: {
        console.debug( "y " + y );
    }
    onZChanged: {
        console.debug( "z " + z );
    }
    onYawChanged: {
        //console.debug( "x " + x + " y " + y );
        //mapView.setCameraFocus(x, y, 0)
        console.debug( "yaw " + entity.yaw );
    }
    onHealthChanged: {
        //entList.updateEntity( { 'handle' : handle, 'health' : health } )
    }
    onMaxhealthChanged: {
        //entList.updateEntity( { 'handle' : handle, 'maxhealth' : maxhealth } )
    }
    onFovChanged: {
    }

    Item3D {
        id: entCylinder
        enabled: entity.showCylinder

        Cylinder {
            id: cyl
            radius: entity.entSizeZ
            length: Math.max( entity.entSizeX, entity.entSizeY ) * 0.5
            effect: Effect {
                color: entity.activeColor
            }
        }

        // FOV lines
        Line {
            id: fovLines
            width: 2.0
            vertices: [
                Math.sin(-fov * 0.5 * Math.PI/180.0) * 100.0, Math.cos(-fov * 0.5 * Math.PI/180.0) * 100.0, 0,
                0, 0, 0,
                Math.sin(fov * 0.5 * Math.PI/180.0) * 100.0, Math.cos(fov * 0.5 * Math.PI/180.0) * 100.0, 0,
            ]
            effect: Effect {
                color: "black"
                useLighting: false
            }
        }
    }

    Item3D {
        id: entBox
        enabled: entity.showBox

        Cube  {
            scale: 1
            effect: Effect {
                blending: true
                color: entity.activeColor
                texture: entity.classTexture
            }

            onClicked: {
                console.debug( "Clicked " + entity.name );
            }
            onDoubleClicked: {
                console.debug( "DblClicked " + entity.name );
            }
            onHoverEnter: {
                console.debug( "HoverEnter " + entity.name );
            }
            onHoverLeave: {
                console.debug( "HoverLeave " + entity.name );
            }
            onPressed: {
                console.debug( "Pressed " + entity.name );
            }
            onReleased: {
                console.debug( "Released " + entity.name );
            }

            transform: [
                 Scale3D { scale: Qt.vector3d(entity.entSizeX, entity.entSizeY, entity.entSizeZ) }
            ]
        }
    }

    transform: [
        Rotation3D {
            id: yaw
            angle: entity.yaw
            axis: Qt.vector3d(0, 0, 1)
        }
    ]

    states: [
        State {
            name: "StateBase"
            PropertyChanges {
                target: entity
                className: "UnknownBaseClass"
                classTexture: "../images/ent_dogtags.png"
                showCylinder: false
                showBox: true
            }
        },
        State {
            name: "StateSoldier"
            PropertyChanges {
                target: entity
                className: "Soldier"
                classTexture: "../images/class_soldier.png"
                showCylinder: true
                showBox: false
            }
        },
        State {
            name: "StateMedic"
            PropertyChanges {
                target: entity
                className: "Medic"
                classTexture: "../images/class_medic.png"
                showCylinder: true
                showBox: false
            }
        },
        State {
            name: "StateEngineer"
            PropertyChanges {
                target: entity
                classTexture: "../images/class_engineer.png"
                className: "Engineer"
                showCylinder: true
                showBox: false
            }
        },
        State {
            name: "StateFieldOps"
            PropertyChanges {
                target: entity
                className: "Field-Ops"
                classTexture: "../images/class_fieldops.png"
                showCylinder: true
                showBox: false
            }
        },
        State {
            name: "StateCovertOps"
            PropertyChanges {
                target: entity
                className: "Covert-Ops"
                classTexture: "../images/class_covertops.png"
                showCylinder: true
                showBox: false
            }
        }
    ]
//    Image {
//        id: classImg
//        x: 0
//        y: 0
//        width: 64
//        height: 64
//        opacity: 1
//        smooth: false
//        source: ""
//        sourceSize.height: 64
//        sourceSize.width: 64
//        fillMode: Image.Stretch
//    }
}
