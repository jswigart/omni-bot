var entity3d;

function createEntity3d() {
    var component = Qt.createComponent("entity.qml");
    if (component.status == Component.Error) {
        console.log("Error loading component:", component.errorString());
    }
    entity3d = component.createObject(mapView, {"x": 100, "y": 100});
}

function detroyEntity3d() {
    entity3d.destroy();
}

//function updatePos( x, y, z ) {
//    entity3d.x = x;
//    entity3d.y = y;
//    entity3d.z = z;
//}

//function updateYaw( newYaw ) {
//    entity3d.transform[0].angle = newYaw;
//}
