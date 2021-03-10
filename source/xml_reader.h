#ifndef LIB_XML_READER_H
#define LIB_XML_READER_H

#include "stream_reader.h"
#include "xml_document.h"

BEGIN_NAMESPACE_LIB

//////////////////////////////////////////////////////////////////////////
class XmlReader
{
public:
    XmlReader (Stream* stream, bool ownStream);
    XmlReader (const string& filename);
    XmlReader (const char* filename);
    ~XmlReader();

    typedef XmlNode::Type NodeType;

public:
    const XmlNode&  node                ()  { return m_node;          }

    NodeType        type                ()  { return m_node.type();   }

    const string&   name                ()  { return m_node.name();   }

    const string&   value               ()  { return m_node.value();  }

    int             depth               ()  { return m_depth;          }

    bool            eof                 ()  { return m_reader.eof();   }

    bool            hasAttributes       ()  { return m_node.hasAttr(); }

    uint            numAttributes       ()  { return m_node.attrCount(); }

    const string&   attribute           (int index);

    const string&   attribute           (const string& name);

    int             attributeAsInt      (const string& name)    { return Convert::toInt(attribute(name)); }

    bool            attributeAsBool     (const string& name)    { return Convert::toBool(attribute(name)); }

    double          attributeAsFloat    (const string& name)    { return Convert::toFloat(attribute(name)); }
    
    bool            isStartElement      (const string& name)    { return type() == XmlNode::Element && name == m_node.name(); }

    bool            isStartElement      ()  { return type() == XmlNode::Element; }

    bool            isEndElement        ()  { return type() == XmlNode::EndElement; }

    bool            isEmptyElement      ()  { return type() == XmlNode::Element && m_empty; }

    bool            isText              ()  { return type() == XmlNode::Text; }

    //////////////////////////////////////////////////////////////////////////

    bool            read                ();

    void            skip                ();

    void            readStartElement    ();

    void            readStartElement    (const string& name);

    void            readEndElement      ();

    bool            readToFollowing     (const string& name);

    bool            readToNextSibling   (const string& name);

    bool            readToDescendant    (const string& name);

    string          readElementContent         ();

    int             readElementContentAsInt    ()   { return Convert::toInt(readElementContent());   }

    uint            readElementContentAsUInt   ()   { return Convert::toUInt(readElementContent());  }

    bool            readElementContentAsBool   ()   { return Convert::toBool(readElementContent());  }

    double          readElementContentAsFloat  ()   { return Convert::toFloat(readElementContent()); }

protected:

    NodeType        detectNextNode      ();

    void            clearCurrentNode    ();

    void            readElementNode     ();

    void            readEndElementNode  ();

    void            readAttributes      ();

    void            readCommentsNode    ();

    void            readTextNode        ();

    void            readProcessingInstructionNode();

    void            readDocumentTypeNode  ();

    void            readCDataNode       ();

protected:
    StreamReader    m_reader;
    XmlNode         m_node;
    bool            m_empty;
    int             m_depth;
};

END_NAMESPACE_LIB

#endif //__LIB_XML_READER_H__
