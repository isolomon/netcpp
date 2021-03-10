#ifndef LIB_XML_DOCUMENT_H
#define LIB_XML_DOCUMENT_H

#include "utils.h"

BEGIN_NAMESPACE_LIB

class Stream;
class XmlNode;
class XmlDocument;
class XmlReader;
class XmlWriter;

struct XmlAttr
{
    string name;
    string value;

    bool isEmpty() const  { return value.empty(); }

    bool isNull () const  { return name.empty();  }

    bool isValid() const  { return name.size();   }

    bool operator == (const XmlAttr& other) const { return name == other.name && value == other.value; }
    bool operator != (const XmlAttr& other) const { return name != other.name || value != other.value; }

    XmlAttr () {}
    XmlAttr (const string& name, const string& value) : name(name), value(value) {}
};

typedef std::vector<XmlNode*> XmlNodes;
typedef std::vector<XmlAttr>  XmlAttrs;

class XmlNode
{
public:
    enum Type
    {
        None,
        Element,
        EndElement,
        Attribute,
        Text,
        CDATA,
        Whitespace,
        Comments,
        DocumentType,
        ProcessingInstruction,
        XmlDeclaration,
        Document,
    };

public:
    XmlNode (Type type = Element);

    XmlNode (const string& nameValue, Type type = Element);

    XmlNode (const string& name, const XmlAttr& attr);

    ~XmlNode ();

    // own propertis
    const string&   name            () const                { return m_name;    }

    const string&   value           () const                { return m_value;   }

    Type            type            () const                { return m_type;    }

    virtual XmlNode& setName        (const string& value)   { m_name = value;  return *this; }

    virtual XmlNode& setValue       (const string& value)   { m_value = value; return *this; }

    virtual XmlNode& setType        (Type value)            { m_type = value;  return *this; }

    bool            isLeaf          () const { return m_type != Element && m_type != Document && m_type != EndElement; }

    bool            isText          () const { return m_type == Text; }

    bool            isNode          (const string& name) const { return m_name == name; }

    bool            isNode          (const string& name, const XmlAttr& attr) const { return m_name == name && hasAttr(attr); }

    string          path            () const;
    
    string          innerXml        () const;

    string          outerXml        () const;

    const string&   contentText     () const;

    string          innerText       () const;

    void            setInnerText    (const string& value);

    XmlNode*        clone           () const;

    // child nodes
    //////////////////////////////////////////////////////////////////////////    
    int             childCount      () const            { return m_nodes.size();   }

    bool            hasChild        () const            { return m_nodes.size() > 0; }

    XmlNode*        child           (int index) const   { return m_nodes.at(index); }

    XmlNode*        findChild       (const string& name, int sequence = 0) const;

    XmlNode*        findChild       (const string& name, const XmlAttr& attr, int sequence = 0) const;

    XmlNode*        findChild       (const XmlAttr& attr, int sequence = 0) const;

    int             indexOfChild    (const XmlNode* node) const;

    int             indexOfThis     () const    { return m_parent ? m_parent->indexOfChild(this) : -1; }


    XmlNode&        insertChild     (int indx, XmlNode* node);

    XmlNode&        appendChild     (XmlNode* node);

    XmlNode&        appendChild     (const string& name);

    XmlNode&        appendChild     (const string& name, const string& value);

    void            removeChild     (XmlNode* node);

    void            removeChild     (int index);

    void            removeAllChild  ();

    void            remove          ();


    XmlNode*        firstChild      () const    { return hasChild() ? m_nodes.front() : 0; }

    XmlNode*        lastChild       () const    { return hasChild() ? m_nodes.back() : 0; }

    XmlNode*        parent          () const    { return m_parent;  }

    XmlNode*        nextSibling     () const;

    XmlNode*        prevSibling     () const;


    XmlNode*        nextSibling     (const string& name, const XmlAttr& attr = XmlAttr()) const;

    XmlNode*        prevSibling     (const string& name, const XmlAttr& attr = XmlAttr()) const;

    XmlNode*        sibling         (const string& name, const XmlAttr& attr = XmlAttr()) const;


    XmlNode*        selectNode      (const string& namePath, const XmlAttr& attr = XmlAttr());

    XmlNodes        selectNodes     (const string& namePath, const XmlAttr& attr = XmlAttr());

    XmlNode*        findNode        (const string& namePath, const XmlAttr& attr = XmlAttr());

    XmlNodes        findNodes       (const string& namePath, const XmlAttr& attr = XmlAttr());

    // attributes
    //////////////////////////////////////////////////////////////////////////
    uint            attrCount       () const    { return m_attrs.size();    }

    bool            hasAttr         () const    { return m_attrs.size() > 0;}

    bool            hasAttr         (const XmlAttr& attr) const;

    bool            hasAttr         (const string& name, const string& value) const { return hasAttr(XmlAttr(name, value)); }

    const XmlAttr*  findAttr        (const string& name) const;

    XmlAttr*        findAttr        (const string& name);

    const string&   attr            (const string& name) const;

    const XmlAttr*  attr            (int index) const { return &m_attrs[index]; }

    XmlAttr*        attr            (int index)       { return &m_attrs[index]; }

    XmlNode&        setAttr         (const string& name, const string& value)   { return setAttr(XmlAttr(name, value)); }

    XmlNode&        setAttr         (const XmlAttr& attr);

    int             indexOfAttr     (const string& name) const;


    XmlNode&        insertAttr      (int index, const XmlAttr& attr);

    XmlNode&        insertAttr      (int index, const string& name, const string& value);

    XmlNode&        appendAttr      (const XmlAttr& attr);

    XmlNode&        appendAttr      (const string& name, const string& value);

    void            removeAttr      (int index);

    void            removeAttr      (const string& name);

    void            removeAllAttr   ();

public:
    static string   escape          (const string& value);
    static string   unescape        (const string& value);
    static const string& nullStr    () { return m_null; }

protected:
    XmlNode*        internalFind            (const strings& names, int level, const XmlAttr& attr, bool recursive);
    void            internalFindAll         (const strings& names, int level, const XmlAttr& attr, bool recursive, XmlNodes& results);
    
    XmlNode*        internalFindByAttr      (const string& name, const string& value, bool recursive);
    void            internalFindAllByAttr   (const string& name, const string& value, bool recursive, XmlNodes& results);
    
    void            write               (XmlWriter& w) const;

    void            setOwnerDocument    (XmlDocument* doc);

protected:
    string          m_name;
    string          m_value;
    Type            m_type;
    XmlAttrs        m_attrs;

    XmlDocument*    m_owndoc;
    XmlNode*        m_parent;
    XmlNodes        m_nodes;

    static const string m_null;
    friend class XmlReader;
};


//////////////////////////////////////////////////////////////////////////
//
class XmlDocument : public XmlNode
{
public:
    XmlDocument ();
    XmlDocument (const string& filename);
    XmlDocument (Stream* stream);
    ~XmlDocument ();

    void        load        (const string& filename);

    void        load        (Stream* stream);

    void        load        (XmlReader& reader);

    void        save        (const string& filename);

    void        save        (Stream* stream);

    void        save        (XmlWriter& writer);

    void        loadXml     (const string& content);
    
    XmlNode*    rootElement ();

    void        clear       ();

protected:
    virtual XmlNode& setType (Type) { return *this; }

protected:
    XmlNode*    m_root;
};

END_NAMESPACE_LIB

#endif //LIB_XML_DOCUMENT_H
