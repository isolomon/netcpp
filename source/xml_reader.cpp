#include "xml_reader.h"
#include "files.h"
#include "errors.h"

BEGIN_NAMESPACE_LIB

//////////////////////////////////////////////////////////////////////////
XmlReader::XmlReader(Stream* stream, bool ownStream) : m_reader(stream, ownStream), m_empty(false), m_depth(0), m_node(XmlNode::None)
{
}

XmlReader::XmlReader(const char* filename) : m_reader(filename), m_empty(false), m_depth(0), m_node(XmlNode::None)
{
}

XmlReader::XmlReader(const string& filename) : m_reader(filename.c_str()), m_empty(false), m_depth(0), m_node(XmlNode::None)
{
}

XmlReader::~XmlReader()
{
    m_reader.close();
}

const string& XmlReader::attribute(const string& name)
{
    XmlAttrs::iterator it = m_node.m_attrs.begin();

    for ( ; it != m_node.m_attrs.end(); ++it)
    {
        if (it->name == name) return it->value;
    }

    return XmlNode::nullStr();
}

const string& XmlReader::attribute(int index)
{
    if (index < 0 || index > numAttributes() - 1) throw IndexOutOfRangeException("Attribute Index");
    return m_node.m_attrs[index].value;
}

bool XmlReader::read()
{
    if (isStartElement() && !isEmptyElement()) m_depth++;

    clearCurrentNode();

    NodeType type = detectNextNode();

    if (type == XmlNode::Element)
    {
        readElementNode();
    }
    else if (type == XmlNode::EndElement)
    {
        readEndElementNode();
        m_depth--;
    }
    else if (type == XmlNode::Text)
    {
        readTextNode();
    }
    else if (type == XmlNode::Comments)
    {
        readCommentsNode();
    }
    else if (type == XmlNode::ProcessingInstruction || type == XmlNode::XmlDeclaration)
    {
        readProcessingInstructionNode();
    }
    else if (type == XmlNode::DocumentType)
    {
        readDocumentTypeNode();
    }
    else if (type == XmlNode::CDATA)
    {
        readCDataNode();
    }
    else if (type == XmlNode::None)
    {
        return false;
    }
    else
    {
        throw NotSupportedException();
    }

    m_node.m_type = type;
    return true;
}

void XmlReader::clearCurrentNode()
{
    m_node.m_type = XmlNode::None;
    m_node.m_name.clear();
    m_node.m_value.clear();
    m_node.m_attrs.clear();
    
    m_empty = false;
}

XmlReader::NodeType XmlReader::detectNextNode()
{
    NodeType type = XmlNode::None;

    m_reader.mark();

    try
    {
        m_reader.skipWhitespace();

        int c = m_reader.read();

        if (c == '<')
        {
            c = m_reader.read();

            if (c == '!')
            {
                c = m_reader.read();
                if (c == '-') type = XmlNode::Comments;
                else if (c == '[') type = XmlNode::CDATA;
                else if (c == 'D') type = XmlNode::DocumentType;
                else throw FormatException("Invalid Xml format");
            }
            else if (c == '?')
            {
                type = XmlNode::ProcessingInstruction;
                if (m_reader.startWith("xml")) type = XmlNode::XmlDeclaration;
            }
            else if (c == '/') type = XmlNode::EndElement;
            else type = XmlNode::Element;
        }
        else if (c == -1)
        {
            type = XmlNode::None;
        }
        else
        {
            type = XmlNode::Text;
        }
    }
    catch (...)
    {
        m_reader.reset();
        throw;
    }

    m_reader.reset();
    return type;
}

void XmlReader::readElementNode()
{
    m_reader.moveTo('<');

    m_node.m_name = m_reader.readToken(":", false);

    readAttributes();

    int c = m_reader.read();
    if (c == '>') return;
    
    if (c == '/')
    {
        c = m_reader.read();
        if (c != '>') throw XmlException();
        m_empty = true;
    }
    else throw XmlException();
}

void XmlReader::readEndElementNode()
{
    m_reader.moveTo("</");

    m_node.m_name = m_reader.readToken(":", false);

    m_reader.moveTo('>');
}

void XmlReader::readTextNode()
{
    string value = m_reader.readTo("<", false);
    m_node.m_value = XmlNode::unescape(value);
}

void XmlReader::readProcessingInstructionNode()
{
    m_reader.moveTo("<?");

    m_node.m_name = m_reader.readToken(":", false);

    readAttributes();

    m_reader.moveTo("?>");
}

void XmlReader::readCDataNode()
{
    if (!m_reader.moveTo("<![CDATA[")) throw XmlException("Invalid Xml format");
    m_node.m_value = m_reader.readTo("]]>");
}

void XmlReader::readDocumentTypeNode()
{
    if (!m_reader.moveTo("<!DOCTYPE ")) throw XmlException("Invalid Xml format");

    bool hasSubset = false;

    m_reader.mark();

    if (m_reader.moveToFirstOf("[>", false))
    {
        int ch = m_reader.read();
        hasSubset = (ch == '[');
    }

    m_reader.reset();

    m_node.m_value.clear();
    if (hasSubset) m_node.m_value = m_reader.readTo("]");

    m_node.m_value += m_reader.readTo(">");
}

void XmlReader::readCommentsNode()
{
    m_reader.moveTo("<!--");
    string value = m_reader.readTo("-->");
	
    m_node.m_value = XmlNode::unescape(value);
}

void XmlReader::readAttributes()
{
    while (true)
    {
        m_reader.skipWhitespace();

        int c = m_reader.peek();
        if (!isalnum(c)) break;

        string name = m_reader.readToken(":", false);
        m_reader.skipCharacters(" \t\r\n=");

        char quote[2] = { m_reader.read(), 0 };
        string value = m_reader.readToFirstOf(quote);
        string unesc = XmlNode::unescape(value);

        m_node.m_attrs.push_back(XmlAttr(name, unesc));
    }
}

//////////////////////////////////////////////////////////////////////////

void XmlReader::readStartElement()
{
    if (isStartElement()) read();
    else throw XmlException("Not a start element");
}

void XmlReader::readStartElement(const string& name)
{
    if (isStartElement(name)) read();
    else throw XmlException("Not a start element with specified name");
}

void XmlReader::readEndElement()
{
    if (isEndElement()) read();
    else throw XmlException("Not a end element");
}

void XmlReader::skip()
{
    int level = depth();

    if (isStartElement() && !isEmptyElement())
    {
        while(read())
        {
            if (isEndElement() && level == depth()) break;
        }
    }

    read();
}

bool XmlReader::readToFollowing(const string& name)
{
    while (read())
    {
        if (isStartElement(name)) return true;
    }

    return false;
}

bool XmlReader::readToNextSibling(const string& name)
{
    int level = depth();

    while(read())
    {
        if (depth() < level) return false;
        if (depth() == level && isStartElement(name)) return true;
    }

    return false;
}

bool XmlReader::readToDescendant(const string& name)
{
    int level = depth();

    while (read())
    {
        if (depth() <= level) return false;
        if (isStartElement(name)) return true;
    }

    return false;
}

string XmlReader::readElementContent()
{
    if (isText()) return m_node.m_value;

    string result;

    if (isStartElement() && !isEmptyElement())
    {
        int level = depth();

        while(read())
        {
            if (isText()) result.append(value());

            if (isEndElement() && level == depth()) break;
        }
    }

    return result;
}

END_NAMESPACE_LIB
