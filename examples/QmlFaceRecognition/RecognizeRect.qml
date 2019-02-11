import QtQuick 2.0

Item{
    id: iroot

    property string name
    property bool isKnown: name !== ""
    property color color: isKnown?"green":"red"
    Rectangle {
        border.width: 1
        border.color: iroot.color
        anchors.fill: parent
        color: "transparent"

        Rectangle{
            color: iroot.color
            height: itxt.paintedHeight+10
            anchors{
                left: parent.left
                right: parent.right
                bottom: parent.bottom
            }
            Text{
                id: itxt
                anchors
                {
                    left: parent.left
                    leftMargin: 5
                    verticalCenter: parent.verticalCenter
                }
                text: isKnown? iroot.name : "Unknown(click to introduce)"
                color: "white"
            }
            MouseArea{
                anchors.fill: parent
                enabled: !isKnown
                onClicked: {

                }
            }
        }
    }
}
