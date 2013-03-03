import QtQuick 1.0
import Qt3D 1.0
import Qt3D.Shapes 1.0

Rectangle {
    id: mainWindow
    width: 790
    height: 520
    color: "#c2c2c2"
    focus: true

Viewport {
    objectName: "viewport"
    id: viewport
    anchors.fill: parent
    picking: true
    //showPicking : true
    blending: true
    focus: true
    navigation: false
    anchors.right: parent.right
    anchors.left: parent.left
    anchors.bottom: parent.bottom
    anchors.top: parent.top

    property double camX : 0;
    property double camY : 0;
    property double camZ : 500;

    property alias scene: root;

    camera: Camera {
        id: cam
        eye: Qt.vector3d( viewport.camX, viewport.camY, viewport.camZ )
        center: Qt.vector3d( 0, 0, 0 )
        farPlane: 10000
    }

    Item3D {
        objectName: "root"
        id: root
    }

    Mesh {
        //dumpInfo: true
        meshName: "humanoid"
        //options:
        source: "./cache/humanoid.obj"
    }

    ///////////////////////////////////////////
    Sphere {
         radius: 20
         effect: Effect {
             color: "cyan"
         }
         Component.onCompleted: {
             console.debug( "Sphere parent " + parent );
             console.debug( "Sphere x " + x + " y " + y );
         }
     }
    Sphere {
        radius: 10
        x: 100
        y: 100
        effect: Effect {
            color: "cyan"
        }

        Component.onCompleted: {
            console.debug( "Sphere parent " + parent );
            console.debug( "Sphere x " + x + " y " + y );
         }
     }

    Item3D {
        Cube  {
            x: 50
            y: 50
            scale: 32
            effect: Effect {
                blending: true
                color: "cyan"
                texture: "../images/ent_dogtags.png"
            }

            Component.onCompleted: {
                console.debug( "Cube parent " + parent );
                console.debug( "Cube x " + x + " y " + y );
             }
        }
    }
    ///////////////////////////////////////////

    function setCameraFocus(camX, camY, camZ) {
        console.debug( "camx " + x + " camy " + y );
        cam.center = Qt.vector3d(camX, camY, camZ)
    }

    MouseArea {
        id: mouseArea
        property double lastMouseX;
        property double lastMouseY;

        //drag.filterChildren: true
        anchors.fill: parent
        acceptedButtons: Qt.LeftButton | Qt.RightButton | Qt.MiddleButton
        hoverEnabled: true
        onClicked: {
            if ( mouse.button == Qt.RightButton ) {
                console.debug( "onClicked " );
            } else {
                mouse.accepted = false;
            }
        }
        onPressed: {
            if ( mouse.button == Qt.RightButton ) {
                lastMouseX = mouse.x;
                lastMouseY = mouse.y;
            } else {
                mouse.accepted = false;
            }
        }
        onPositionChanged: {
            var stepSize = 20;
            if ( mouse.modifiers & Qt.ShiftModifier ) {
                stepSize = 50;
            }

            var deltaX = mouse.x - lastMouseX;
            var deltaY = mouse.y - lastMouseY;

            if ( pressedButtons & Qt.MiddleButton ) {
                viewport.camZ += deltaY * stepSize;
            }

            if ( pressedButtons & Qt.RightButton ) {
                //console.debug( "delta " + deltaX + " " + deltaY );
                viewport.camX -= deltaX * stepSize;
                viewport.camY += deltaY * stepSize;
            }

            lastMouseX = mouse.x;
            lastMouseY = mouse.y;
        }
    }
    Keys.onPressed: {
       console.debug( "onPressed " + event.text );

        var stepSize = 50;
        if ( event.modifiers & Qt.ShiftModifier ) {
            stepSize = 100;
        }

        if (event.key == Qt.Key_Right) {
		cam.eye.x += stepSize;
        } else if (event.key == Qt.Key_Left) {
		cam.eye.x -= stepSize;		
        } else if (event.key == Qt.Key_Up) {
		cam.eye.y -= stepSize;
        } else if (event.key == Qt.Key_Down) {
		cam.eye.y += stepSize;
        } else if (event.key == Qt.Key_Plus) {
            event.accepted = true;
            //cam.eye.z = cam.eye.z + stepSize;
	    
	    cam.fieldOfView  += 1;
        } else if (event.key == Qt.Key_Minus) {
            event.accepted = true;
            //cam.eye.z = cam.eye.z - stepSize;
	    
	    cam.fieldOfView  -= 1;
        }
    }
}
}
