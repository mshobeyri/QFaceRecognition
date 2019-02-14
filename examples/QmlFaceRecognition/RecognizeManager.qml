import QtQuick 2.0

Item {
    property var obejctList: []

    function updateRects(names, positions){
        while(obejctList.length > 0){
            obejctList.pop().destroy()
        }

        var component = Qt.createComponent("RecognizeRect.qml")
        for(var i=0;i<names.length;i++){
            if(component.status == Component.Ready){
                var c = component.createObject(irectsContainer);
                obejctList.push(c)
                var p = positions[i].split(',');
                c.x = p[0]
                c.y = p[1]
                c.width = p[2]
                c.height = p[3]
                c.name = names[i]
            }
        }
    }

    Item{
        id: irectsContainer

        anchors.fill: parent
    }
}
