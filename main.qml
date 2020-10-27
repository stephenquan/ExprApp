import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Window 2.12
import QtQuick.Layouts 1.12
import ArcGIS.AppFramework.Prototype 1.0

Window {
    visible: true
    width: 640
    height: 480
    title: qsTr("Hello World")

    Flickable {
        id: flickable

        anchors.fill: parent
        anchors.margins: 10

        contentWidth: columnLayout.width
        contentHeight: columnLayout.height
        clip: true

        ColumnLayout {
            id: columnLayout

            width: flickable.width

            Text {
                text: qsTr("Input")
                font.pointSize: styles.textPointSize
            }

            TextEdit {
                id: textEdit

                Layout.fillWidth: true

                //text: "1*2+3/4"
                text: "1+2+3"
                font.pointSize: styles.textPointSize
                wrapMode: Text.WrapAtWordBoundaryOrAnywhere
            }

            Item { Layout.preferredHeight: 10 }

            Button {
                text: qsTr("Parse")
                font.pointSize: styles.textPointSize

                onClicked: {
                    resultText.text = "";
                    errorsText.text = "";

                    let ok = expressions.loadFromString(textEdit.text);
                    if (!ok) {
                        errorsText.text = JSON.stringify(expressions.errors, undefined, 2);
                        return;
                    }

                    resultText.text = JSON.stringify(expressions.values, undefined, 2);
                }

            }

            Item { Layout.preferredHeight: 10 }

            Text {
                text: qsTr("Result")
                font.pointSize: styles.textPointSize

                visible: resultText.visible
            }

            Text {
                id: resultText

                Layout.fillWidth: true

                font.pointSize: styles.textPointSize
                wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                visible: text !== ""
            }

            Text {
                text: qsTr("Errors")
                font.pointSize: styles.textPointSize

                visible: errorsText.visible
            }

            Text {
                id: errorsText

                Layout.fillWidth: true

                font.pointSize: styles.textPointSize
                wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                visible: text !== ""
                color: "red"
            }

        }

    }

    QtObject {
        id: styles

        property double textPointSize: 12
    }

    Expressions {
        id: expressions

    }
}
