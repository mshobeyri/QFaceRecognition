import QtQuick 2.0

Rectangle {
    id: iroot

    x: (parent.width - width)/2
    y: (parent.height - height)/2
    width: height
    height: Math.min(parent.width,parent.height) / 2
    visible: false
    color: "black"

    property var imgObj

    function open(image,error){
        itf.text = ""
        itf.forceActiveFocus()
        ierrorBox.visible = error
        img.source = image.url
        iroot.visible = true
        imgObj = image
    }

    Column{
        anchors.fill: parent
        Item{
            width: parent.width
            height: parent.height - 40
            Image{
                id: img

                anchors.fill: parent
                anchors.margins: 5
            }
        }
        Rectangle{
            id: ierrorBox

            width: parent.width
            height: 40
            color: "red"
            Text {
                anchors.centerIn: parent
                text: "More than one face detected(click to retry)"
            }
            MouseArea{
                anchors.fill: parent
                onClicked: {
                    iroot.visible = false
                }
            }
        }

        Row{
            visible: !ierrorBox.visible
            width: parent.width
            height: 40
            Rectangle{
                color: "white"
                width: parent.width - 40
                height: 40
                TextInput{
                    id: itf

                    color: "black"
                    text: ""
                    anchors.verticalCenter: parent.verticalCenter
                    x:5
                }
            }

            Rectangle{
                width: 40
                height: 40
                color: itf.text===""?"lightgreen":"green"
                Text{
                    anchors.centerIn: parent
                    color: "white"
                    text: "Ok"
                }
                MouseArea{
                    anchors.fill: parent
                    onClicked: {
                        if(itf.text==="")
                            return
                        frFilter.faceRecognition.introduce(itf.text,imgObj.image)
                        iroot.visible = false
                    }
                }
            }
        }
    }
}
