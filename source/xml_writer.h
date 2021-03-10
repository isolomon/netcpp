#ifndef LIB_XML_WRITER_H
#define LIB_XML_WRITER_H

#include "xml_document.h"
#include "stream_writer.h"

BEGIN_NAMESPACE_LIB

//////////////////////////////////////////////////////////////////////////
class XmlWriter
{
public:
    XmlWriter (Stream* stream);
    XmlWriter (const string& filename);
    XmlWriter (const char* filename);
    ~XmlWriter();

    int         depth           ()              { return m_tags.size(); }    

    int         indent          ()              { return m_indent;  }
    
    void        setIndent       (int value)     { m_indent  = value; }

    XmlWriter&  declaration     (const string& name);

    XmlWriter&  element         (const string& name);

    XmlWriter&  end             ();

    XmlWriter&  attribute       (const string& name, const StringValue& value);
        
    XmlWriter&  text            (const StringValue& value);

    XmlWriter&  comments        (const string& value);

    void        close           ();

protected:
    void        writeOpenTag    (const string& name);
    void        writeEndTag     (const string& name);
    void        closeOpenTag    ();

protected:
    StreamWriter    m_writer;
    int             m_indent;
    int             m_state;
    bool            m_hasText;
    bool            m_firstTime;

    std::vector<string> m_tags;

    enum TagState { TagClosed = 0, DeclarationTag, ElementTag, };
};

END_NAMESPACE_LIB

#endif //__LIB_XML_READER_H__
