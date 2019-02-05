import QtQuick 2.9
import QtQuick.Window 2.2
import FaceRecognition 1.0

Window {
    visible: true
    width: 640
    height: 480
    title: qsTr("Face Recognition")
    FaceRecognition {
        id: fr
    }

    MouseArea {
        anchors.fill: parent
        onClicked: fr.introduceFolder("C:/Projects/QFaceRecognition/assets")
    }
}
