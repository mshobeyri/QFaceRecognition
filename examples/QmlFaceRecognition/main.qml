import QtQuick 2.9
import QtQuick.Window 2.2
import FaceRecognition 1.0
import QtMultimedia 5.5

Window {
    visible: true
    width: 640
    height: 480
    title: qsTr("Face Recognition")
    Camera
    {
        id:camera
        focus {
            focusMode: CameraFocus.FocusContinuous
            focusPointMode: CameraFocus.FocusPointAuto
        }
    }

    FaceRecognitionFilter{
        id: frFilter
    }

    VideoOutput
    {
        id: videoOutput
        source: camera

        autoOrientation: true
        fillMode: VideoOutput.Stretch
        filters: [ frFilter ]
        MouseArea {
            anchors.fill: parent
            onClicked: {
                camera.focus.customFocusPoint = Qt.point(mouse.x / width,  mouse.y / height);
                camera.focus.focusMode = CameraFocus.FocusMacro;
                camera.focus.focusPointMode = CameraFocus.FocusPointCustom;
            }
        }
    }
}
