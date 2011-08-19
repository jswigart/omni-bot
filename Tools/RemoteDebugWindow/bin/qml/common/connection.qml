import QtQuick 1.0
import Qt3D 1.0
import Qt3D.Shapes 1.0

Item3D {
    id: conn

    x: ( fromx + tox ) * 0.5
    y: ( fromy + toy ) * 0.5
    z: ( fromz + toz ) * 0.5

    // from the remote host
    property real fromx: 0
    property real fromy: 0
    property real fromz: 0

    property real tox: 0
    property real toy: 0
    property real toz: 0

    property color color: "blue"

//    Component.onCompleted: {
//        console.debug( "connection " );
//    }

    Line {
        width: 1.0
        vertices: [
            conn.fromx, conn.fromy, conn.fromz,
            conn.tox, conn.toy, conn.toz,
            conn.x,conn.y,conn.z,
        ]
        effect: Effect {
            color: conn.color
            useLighting: false
        }
    }
}
