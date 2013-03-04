import QtQuick 1.0
import Qt3D 1.0
import Qt3D.Shapes 1.0

Item3D {
    id: waypoint
    // from the remote host
    x: 0
    y: 0
    z: 0

    property string name: ""
    property int uid: 0
    property int entityid: 0
    property real yaw: 0
    property real radius: 0
    property real offsetTop: 100
    property real offsetBottom: 0
    property color color: "white"
    property int uniqueId: 0

    Component.onCompleted: {
        console.debug( "wp " + name + " id ", uid );
    }

    Line {
        id: wpLine
        width: 2.0
        vertices: [
            waypoint.x, waypoint.y, waypoint.z + offsetTop,
            waypoint.x, waypoint.y, waypoint.z + offsetBottom,
        ]
        effect: Effect {
            color: waypoint.color
            useLighting: false
        }
    }

    Sphere {
        radius: waypoint.radius
        effect: Effect {
            color: waypoint.color
        }
    }
}
