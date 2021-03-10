#include "xml_writer.h"
#include "errors.h"

BEGIN_NAMESPACE_LIB

//////////////////////////////////////////////////////////////////////////

XmlWriter::XmlWriter(Stream* stream) : m_writer(stream), m_indent(2), m_firstTime(true), m_state(TagClosed), m_hasText(false)
{
}


XmlWriter::XmlWriter(const char* filename) : m_writer(filename), m_indent(2), m_firstTime(true), m_state(TagClosed), m_hasText(false)
{
}

XmlWriter::XmlWriter(const string& filename) : m_writer(filename.c_str()), m_indent(2), m_firstTime(true), m_state(TagClosed), m_hasText(false)
{

}

XmlWriter::~XmlWriter()
{
    close();
}


void XmlWriter::close()
{
    m_writer.close();
}


XmlWriter& XmlWriter::declaration(const string& name)
{
    if (m_state != TagClosed) closeOpenTag();
    
    writeOpenTag("?" + name);

    m_state = DeclarationTag;
    m_tags.push_back(name);

    return *this;
}

XmlWriter& XmlWriter::element(const string& name)
{
    if (m_state != TagClosed) closeOpenTag();

    writeOpenTag(name);

    m_state = ElementTag;
    m_tags.push_back(name);

    return *this;
}

void XmlWriter::writeOpenTag(const string& name)
{
    if (!m_firstTime) m_writer.writeLine();
    if (m_firstTime) m_firstTime = false;

    m_writer.write(string(m_indent * depth(), ' '));
    m_writer.write("<");
    m_writer.write(name);
}

void XmlWriter::writeEndTag(const string& name)
{
    if (!m_hasText)
    {
        m_writer.writeLine();
        m_writer.write(string(m_indent * depth(), ' '));
    }

    m_writer.write(format("</%s>", name.c_str()));
}

void XmlWriter::closeOpenTag()
{
    if (m_state == ElementTag) m_writer.write(">");
    if (m_state == DeclarationTag) m_writer.write("?>");

    m_state = TagClosed;
}

XmlWriter& XmlWriter::end()
{
    if (m_tags.size() == 0) throw InvalidOperationException();

    string name = m_tags.back();
    m_tags.pop_back();

    if (m_state == DeclarationTag)
    {
        m_writer.write("?>");
    }
    else if (m_state == ElementTag)
    {
        m_writer.write(" />");
    }
    else // end element
    {
        writeEndTag(name);
    }

    m_hasText = false;
    m_state = TagClosed;

    return *this;
}

XmlWriter& XmlWriter::attribute(const string& name, const StringValue& value)
{
    if (m_state == TagClosed) throw InvalidOperationException();

    string escaped = XmlNode::escape(value);

    string attr = format(" %s=\"%s\"", name.c_str(), escaped.c_str());
    m_writer.write(attr);

    return *this;
}

XmlWriter& XmlWriter::comments(const string& value)
{
    if (m_state != TagClosed) closeOpenTag();

    string escaped = XmlNode::escape(value);

    writeOpenTag("!--");
    string line = format("%s-->", escaped.c_str());
    m_writer.write(line);

    return *this;
}

XmlWriter& XmlWriter::text(const StringValue& value)
{
    if (m_state != TagClosed) closeOpenTag();

    string escaped = XmlNode::escape(value);

    m_writer.write(escaped.c_str());
    m_hasText = true;

    return *this;
}

END_NAMESPACE_LIB
