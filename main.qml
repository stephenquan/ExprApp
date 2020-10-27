import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Window 2.12
import QtQuick.Layouts 1.12
import ArcGIS.AppFramework.Prototype 1.0

Window {
    visible: true
    width: 800
    height: 600
    title: qsTr("Expression App")

    QtObject {
        id: styles

        property double textPointSize: 12
        property color windowColor: "#e0ffe0"
        property color frameBorderColor: "#c0c0c0"
        property color inputBackgroundColor: "#ffffe0"
        property color outputBackgroundColor: "#e0e0ff"
        property color errorBackgroundColor: "#ffe0e0"
        property color optionsBackgroundColor: "#e0e0e0"
    }

    Page {
        anchors.fill: parent

        background:  Rectangle {
            color: styles.windowColor
        }

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

                RowLayout {
                    Layout.fillWidth: true

                    ColumnLayout {
                        Layout.fillWidth: true
                        Layout.preferredWidth: 500
                        Layout.alignment: Qt.AlignTop

                        Text {
                            text: qsTr("Input")
                            font.pointSize: styles.textPointSize
                        }

                        Frame {
                            Layout.fillWidth: true

                            background: Rectangle {
                                color: styles.inputBackgroundColor
                                border.color: styles.frameBorderColor
                            }

                            TextEdit {
                                id: textEdit

                                width: parent.width

                                //text: "1*2+3/4"
                                text: "1+2+3"
                                font.pointSize: styles.textPointSize
                                wrapMode: Text.WrapAtWordBoundaryOrAnywhere

                                onTextChanged: parse()
                            }
                        }
                    }

                    ColumnLayout {
                        Layout.fillWidth: true
                        Layout.preferredWidth: 500
                        Layout.alignment: Qt.AlignTop

                        Text {
                            text: qsTr("Result")
                            font.pointSize: styles.textPointSize
                        }

                        Frame {
                            Layout.fillWidth: true

                            background: Rectangle {
                                color: styles.outputBackgroundColor
                                border.color: styles.frameBorderColor
                            }

                            visible: resultText.text !== ""

                            Text {
                                id: resultText

                                width: parent.width

                                font.pointSize: styles.textPointSize
                                wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                            }
                        }

                        Frame {
                            Layout.fillWidth: true

                            visible: errorsText.text !== ""

                            background: Rectangle {
                                color: styles.errorBackgroundColor
                                border.color: styles.frameBorderColor
                            }

                            Text {
                                id: errorsText

                                width: parent.width

                                font.pointSize: styles.textPointSize
                                wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                                color: "red"
                            }
                        }

                    }
                }

                Frame {
                    Layout.fillWidth: true

                    background: Rectangle {
                        color: styles.optionsBackgroundColor
                        border.color: styles.frameBorderColor
                    }

                    ColumnLayout {
                        width: parent.width

                        Text {
                            text: qsTr("Options")
                            font.pointSize: styles.textPointSize
                        }

                        CheckBox {
                            id: abortOnErrorCheckBox

                            text: qsTr("AbortOnError")
                            font.pointSize: styles.textPointSize

                            onCheckedChanged: parse()
                        }

                        CheckBox {
                            id: continueOnErrorCheckBox

                            text: qsTr("ContinueOnError")
                            font.pointSize: styles.textPointSize

                            onCheckedChanged: parse()
                        }

                    }

                }
            }
        }
    }

    Expressions {
        id: expressions
    }

    Component.onCompleted: parse()

    function parse() {
        expressions.abortOnError = abortOnErrorCheckBox.checked;
        expressions.continueOnError = continueOnErrorCheckBox.checked;

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
