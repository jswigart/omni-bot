import QtQuick 1.0
import Qt3D 1.0
import Qt3D.Shapes 1.0

Item3D {
    // from the remote host
    x: 0
    y: 0
    z: 0

    property string name: "NoName"
    property string tagName: "NoName"
    property int entityid: 0
    property int ownerid: 0
    property real yaw: 0
    property real pitch: 0
    property real sizex: 0
    property real sizey: 0
    property real sizez: 0
    property real defaultPriority: 0
    property int usersInProgress: 0
    property int maxUsersInProgress: 0
    property int usersInUse: 0
    property int maxUsersInUse: 0
    property int availableTeamMask: 0
    property int roleMask: 0

    Mesh {
        //dumpInfo: true
        meshName: "humanoid"
        //options:
        source: "./cache/humanoid.obj"
    }
}
