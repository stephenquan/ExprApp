#ifndef __Expressions__
#define __Expressions__

#include <QObject>
#include <QByteArray>
#include <QIODevice>
#include <QBuffer>
#include <QList>
#include <QStack>
#include <QVariant>

class ExpressionChar
{
public:
    ExpressionChar(int _ch = EOF, int _lineNo = 0, int _column = 0) :
        ch(_ch),
        lineNo(_lineNo),
        column(_column)
    {
    }

    ExpressionChar(const ExpressionChar& other) :
        ch(other.ch),
        lineNo(other.lineNo),
        column(other.column)
    {
    }

    int ch;
    int lineNo;
    int column;
};

class Expressions : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QVariant values READ values NOTIFY valuesChanged)
    Q_PROPERTY(QVariant errors READ errors NOTIFY errorsChanged)

public:
    Expressions(QObject* parent = nullptr);

    Q_INVOKABLE bool loadFromString(const QString& str);

signals:
    void valuesChanged();
    void errorsChanged();
    void error(int lineNo, int column, const QString& message);
    void element(const QString type, const QVariant value);

protected:
    bool parse(const QString& str);
    bool parse(const QByteArray& bytes);
    bool parse(QIODevice* device);

    bool parseSymbol(char symbol);
    bool parseNumber(float& number);
    bool parseEOF();

    bool parseFactor();
    bool parseTerm();
    bool parseExpr();
    bool parse();

    void notifyElement(const QString& type, const QVariant& value = QVariant());
    void notifyError(int lineNo, int column, const QString& message);

    ExpressionChar read();
    void unread(const ExpressionChar& ch);

    QVariant values() const { return m_Elements; }
    QVariant errors() const { return m_Errors; }

    QIODevice* m_Device;
    int m_DeviceLineNo;
    int m_DeviceColumn;
    int m_LineNo;
    int m_Column;
    QStack<ExpressionChar> m_Stack;
    QVariantList m_Elements;
    QVariantList m_Errors;

};

#endif
