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
    Component.onCompleted: {
        frFilter.faceRecognition.introduceFolder("../../assets/known")

    }

    FaceRecognitionFilter{
        id: frFilter

        captureRect: {
            // setup bindings
            videoOutput.contentRect;
            videoOutput.sourceRect;
            return videoOutput.mapRectToSource(
                        videoOutput.mapNormalizedRectToItem(Qt.rect(
                                                                0, 0, 1, 1
                                                                )));
        }
        faceRecognition{
            mode: FaceRecognition.Recognize
            distanceThreshold: 0.6
            onRecognizeProcessEnded: {
                rm.updateRects(names, positions)
            }
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
    RecognizeManager{
        id: rm

        anchors.fill: parent
    }
}
