#include "Expressions.h"
#include <QBuffer>
#include <ctype.h>
#include <QDebug>

//----------------------------------------------------------------------
//
//----------------------------------------------------------------------

Expressions::Expressions(QObject* parent) :
    QObject(parent),
    m_Device(nullptr),
    m_DeviceLineNo(0),
    m_DeviceColumn(0),
    m_LineNo(0),
    m_Column(0)
{
}

//----------------------------------------------------------------------
//
//----------------------------------------------------------------------

bool Expressions::loadFromString(const QString& str)
{
    return parse(str);
}

//----------------------------------------------------------------------
//
//----------------------------------------------------------------------

bool Expressions::parse(const QString& str)
{
    return parse(str.toUtf8());
}

//----------------------------------------------------------------------
//
//----------------------------------------------------------------------

bool Expressions::parse(const QByteArray& bytes)
{
    QBuffer buffer;
    buffer.setData(bytes);
    buffer.open(QIODevice::ReadOnly);
    bool ok = parse(&buffer);
    buffer.close();
    return ok;
}

//----------------------------------------------------------------------
//
//----------------------------------------------------------------------

bool Expressions::parse(QIODevice* device)
{
    m_Device = device;
    m_LineNo = m_DeviceLineNo = 1;
    m_Column = m_DeviceColumn = 1;

    return parse();
}

//----------------------------------------------------------------------
//
//----------------------------------------------------------------------

ExpressionChar Expressions::read()
{
    if (m_Stack.length())
    {
        ExpressionChar stackCh = m_Stack.pop();
        m_LineNo = stackCh.lineNo;
        m_Column = stackCh.column;
        return stackCh;
    }

    if (!m_Device)
    {
        return ExpressionChar();
    }

    char ch;
    qint64 count = m_Device->read(&ch, 1);
    if (count < 1)
    {
        return ExpressionChar();
    }

    ExpressionChar _ch(ch, m_DeviceLineNo, m_DeviceColumn);

    m_DeviceColumn++;
    if (ch == '\n')
    {
        m_DeviceColumn = 1;
        m_DeviceLineNo++;
    }

    m_LineNo = m_DeviceLineNo;
    m_Column = m_DeviceColumn;

    return _ch;
}

//----------------------------------------------------------------------
//
//----------------------------------------------------------------------

void Expressions::unread(const ExpressionChar& ch)
{
    m_Stack.push(ch);
}

//----------------------------------------------------------------------
//
//----------------------------------------------------------------------

bool Expressions::parseSymbol(char symbol)
{
    ExpressionChar ch = read();
    while (ch.ch != EOF && isspace(ch.ch))
    {
        ch = read();
    }

    if (ch.ch == EOF)
    {
        return false;
    }

    if (ch.ch != symbol)
    {
        unread(ch);
        return false;
    }

    return true;
}

//----------------------------------------------------------------------
//
//----------------------------------------------------------------------

bool Expressions::parseNumber(float& number)
{
    ExpressionChar ch = read();
    while (ch.ch != EOF && isspace(ch.ch))
    {
        ch = read();
    }

    if (ch.ch == EOF)
    {
        return false;
    }

    if (!isdigit(ch.ch))
    {
        unread(ch);
        return false;
    }

    QByteArray _number;
    while (ch.ch != EOF && isdigit(ch.ch))
    {
        _number.append(static_cast<char>(ch.ch));
        ch = read();
    }

    if (ch.ch != EOF)
    {
        unread(ch);
    }

    number = _number.toFloat();
    return true;
}

//----------------------------------------------------------------------
//
//----------------------------------------------------------------------

bool Expressions::parseEOF()
{
    ExpressionChar ch = read();
    while (ch.ch != EOF && isspace(ch.ch))
    {
        ch = read();
    }

    if (ch.ch != EOF)
    {
        qDebug() << Q_FUNC_INFO << "Unexpected char at EOF: " << ch.ch;
        unread(ch);
        return false;
    }

    return true;
}

//----------------------------------------------------------------------
//
//----------------------------------------------------------------------

bool Expressions::parseFactor()
{
    if (parseSymbol('('))
    {
        if (!parseExpr())
        {
            return false;
        }

        if (!parseSymbol(')'))
        {
            return false;
        }

        return true;
    }

    float number;
    if (parseNumber(number))
    {
        notifyElement("typeNumber", number);
        return true;
    }

    return false;
}

//----------------------------------------------------------------------
//
//----------------------------------------------------------------------

bool Expressions::parseTerm()
{
    if (!parseFactor())
    {
        return false;
    }

    if (parseSymbol('*'))
    {
        if (!parseTerm())
        {
            notifyError(m_LineNo, m_Column, "Missing term when processing: term ::= factor * term");
            return false;
        }

        notifyElement("typeOperationMultiply");
        return true;
    }

    if (parseSymbol('/'))
    {
        if (!parseTerm())
        {
            notifyError(m_LineNo, m_Column, "Missing term when processing: term ::= factor / term");
            return false;
        }

        notifyElement("typeOperationDivide");
        return true;
    }

    return true;
}

//----------------------------------------------------------------------
//
//----------------------------------------------------------------------

bool Expressions::parseExpr()
{
    if (!parseTerm())
    {
        return false;
    }

    if (parseSymbol('+'))
    {
        if (!parseExpr())
        {
            notifyError(m_LineNo, m_Column, "Missing term when processing: expr ::= term + expr");
            return false;
        }

        notifyElement("typeOperationAdd");
        return true;
    }

    if (parseSymbol('-'))
    {
        if (!parseExpr())
        {
            notifyError(m_LineNo, m_Column, "Missing term when processing: expr ::= term - expr");
            return false;
        }

        notifyElement("typeOperationSubtract");
        return true;
    }

    return true;
}
//----------------------------------------------------------------------
//
//----------------------------------------------------------------------

bool Expressions::parse()
{
    m_Elements.clear();
    emit valuesChanged();

    m_Errors.clear();
    emit errorsChanged();

    if (!parseExpr())
    {
        notifyError(m_LineNo, m_Column, "Cannot identify expression");
        qDebug() << m_Elements;
        return false;
    }

    if (!parseEOF())
    {
        notifyError(m_LineNo, m_Column, "Unexpected trailing data");
        return false;
    }

    return true;
}

//----------------------------------------------------------------------
//
//----------------------------------------------------------------------

void Expressions::notifyElement(const QString& type, const QVariant& value)
{
    QVariantMap item;
    item["type"] = type;
    item["value"] = value;
    m_Elements.append(item);

    emit valuesChanged();
    emit element(type, value);
}

//----------------------------------------------------------------------
//
//----------------------------------------------------------------------

void Expressions::notifyError(int lineNo, int column, const QString& message)
{
    qDebug() << Q_FUNC_INFO << lineNo << column << message;

    QVariantMap item;
    item["lineNo"] = lineNo;
    item["column"] = column;
    item["message"] = message;
    m_Errors.append(item);

    emit errorsChanged();
    emit error(lineNo, column, message);
}

//----------------------------------------------------------------------
//
//----------------------------------------------------------------------
