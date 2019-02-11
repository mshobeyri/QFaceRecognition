import QtQuick 2.9
import QtQuick.Window 2.2
import FaceRecognition 1.0
import QtMultimedia 5.5

Window {
    id: iwin
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
        captureRect: {
            // setup bindings
            videoOutput.contentRect;
            videoOutput.sourceRect;
            return videoOutput.mapRectToSource(videoOutput.mapNormalizedRectToItem(Qt.rect(
                                                                                       0, 0, 1, 1
                                                                                       )));
        }
        faceRecognition{
            mode: FaceRecognition.Recognize
        }

        onFaceRecognized: {
            rr.isKnown = true
            rr.name = name
            rr.x = position.x
            rr.y = position.y
            rr.width = position.width
            rr.height = position.height
        }
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
    RecognizeRect{
        id: rr
        width: 100
        height: 100
    }
}
