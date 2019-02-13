# QFaceRecognition
Qt/Qml lib for face detection/recogntion based on dlib

##Features

* command line base recogntion
* works with image files directly
* video filter for recognize face works whit Qt Camera
* Qml support

##Build And Run

 first get dlib and build it using dlib build manual form [here](https://github.com/ageitgey/face_recognition).
 
 then put dlib.lib file in lib folder in root of project.
 
 remember to install git lfs to get model files and set their pathes to **QFaceRecognition** class using _setModel_ function. something like this:
 
 '''
     QFaceRecognition::setModel(
        "../../model/dlib_face_recognition_resnet_model_v1.dat",
        "../../model/shape_predictor_5_face_landmarks.dat");
'''
 
 ##Examples
 ###console face recognition
 
 the code api is so simple:
 
 '''
 
int
main(int argc, char* argv[]) {
     QFaceRecognition f;
    f.introduceFolder(argv[1]); //known images folder path
    qDebug() << f.recognizeFolder(argv[2]);  unknown images folder path
	}
'''
 
you can run the project like this:

> ./ConsoleFaceRecognition <known image folder> <unknown images folder>

example:

>./ConsoleFaceRecognition ../../assets/known ../../assets/unknown

set the pathes relative.

the output is name of known files paired with path of unknown files.

 ###qml face recognition
 
 this is a real time face recognition project whit an output like this:
 
 
 
 you can see using video filter example in qml code here
 
'''
import QtQuick 2.0
import QtQuick.Window 2.0
import FaceRecognition 1.0
import QtMultimedia 5.2

Window {
    id: iwin
    visible: true
    width: 640
    height: 480
    title: qsTr("Face Recognition")    
	Camera
    {
        id:camera
    }
    FaceRecognitionFilter{
        id: frFilter

        captureRect: {
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
                console.log(names, positions)
            }
        }
    }

    VideoOutput{
        id: videoOutput

        source: camera
        autoOrientation: true
        fillMode: VideoOutput.Stretch
        filters: [ frFilter ]
    }
 
}
'''
* use mode to switch ability of application form Detecting and Recognizing.
* use distanceThreshold to change threshold of accepting two face is from same person.
* you can limit capture image to captureRect section

this are the signals you can use
'''
    void faceDetected(QRect position);
    void faceRecognized(QString name, QRect position);
    void detectionProcessEnded(QList<QRect> positions); //emit just when mode is detection
    void recognizeProcessEnded(QStringList names, QStringList positions);
'''

**special thanks to saeed khodaigani**
 
 
 
 
 
 
