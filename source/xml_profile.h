#ifndef LIB_XML_PROFILE_H
#define LIB_XML_PROFILE_H

#include "xml_document.h"

BEGIN_NAMESPACE_LIB

class Stream;

class XmlProfile
{
public:
    XmlProfile ();

    XmlProfile (const string& path);

    XmlProfile (Stream* stream);

    ~XmlProfile();


    void            load                (const string& path);

    void            load                (Stream* stream);

    void            loadXml             (const string& content);

    void            save                (const string& path);

    void            save                (Stream* stream);

    void            clear               ()  { m_document.clear(); }


    XmlDocument*    document            ()  { return &m_document; }

    XmlNode*        currentNode         ()  { return m_node;      }

    bool            createMode          ()  { return m_createMode; }

    void            setCreateMode       (bool value) { m_createMode = value; }


    bool            moveTo              (const string& path, const XmlAttr& attr = XmlAttr());

    bool            moveToChild         (const string& path, const XmlAttr& attr = XmlAttr());

    bool            moveToChild         (); // enters the child section, it stays at the first child when call moveToNextSibling

    bool            moveToParent        ();

    bool            moveToSibling       (const string& name = string(), const XmlAttr& attr = XmlAttr());

    bool            moveToPrevSibling   (const string& name = string(), const XmlAttr& attr = XmlAttr());

    bool            moveToNextSibling   (const string& name = string(), const XmlAttr& attr = XmlAttr());

    
    bool            moveTo              (const string& path, const string& attr, const StringValue& value)   { return moveTo(path, XmlAttr(attr, value)); }

    bool            moveToChild         (const string& path, const string& attr, const StringValue& value)   { return moveToChild (path, XmlAttr(attr, value)); }

    bool            moveToSibling       (const string& name, const string& attr, const StringValue& value)   { return moveToSibling(name, XmlAttr(attr, value)); }

    bool            moveToPrevSibling   (const string& name, const string& attr, const StringValue& value)   { return moveToPrevSibling(name, XmlAttr(attr, value)); }

    bool            moveToNextSibling   (const string& name, const string& attr, const StringValue& value)   { return moveToNextSibling(name, XmlAttr(attr, value)); }
    
    
    bool            hasChild            ()  { return m_node->hasChild();    }

    bool            hasAttr             ()  { return m_node->hasAttr();     }

    bool            hasAttr             (const string& name, const string& value = string())  { return m_node->hasAttr(name, value); }

    int             childCount          ()  { return m_node->childCount();  }

    int             attrCount           ()  { return m_node->attrCount();   }

    void            removeChild         (int index)             { m_node->removeChild(index); }

    void            removeAttr          (const string& name)    { m_node->removeAttr(name);   }

    void            removeAllChild      ()  { m_node->removeAllChild(); }

    void            removeAllAttr       ()  { m_node->removeAllAttr();  }

    
    //////////////////////////////////////////////////////////////////////////

    const string&   elementName         ()  { return m_node->name();    }

    int64           contentInt          ();

    double          contentFloat        ();

    bool            contentBool         ();

    const string&   contentText         ();


    int64           attrInt             (const string& attr, int64 defval = 0);

    double          attrFloat           (const string& attr, double defval = 0);

    bool            attrBool            (const string& attr, bool defval = false);

    const string&   attrText            (const string& attr);

    int             attrText            (const string& attr, char* buffer, int size);


    int64           childInt            (const string& name, int64 defval = 0);

    double          childFloat          (const string& name, double defval = 0);

    bool            childBool           (const string& name, bool defval = false);

    const string&   childText           (const string& name);

    int             childText           (const string& name, char* buffer, int size);
    
    // const char* is implicit converted to bool instead of const string&
    int64           childInt            (const string& name, const char* attr, int64 defval = 0);

    double          childFloat          (const string& name, const char* attr, double defval = 0);

    bool            childBool           (const string& name, const char* attr, bool defval = false);

    const string&   childText           (const string& name, const char* attr);

    int             childText           (const string& name, const char* attr, char* buffer, int size);


    //////////////////////////////////////////////////////////////////////////

    XmlProfile&     setElementName      (const string& value)  { m_node->setName(value); return *this; }

    XmlProfile&     setContentInt       (int64 value);

    XmlProfile&     setContentFloat     (double value, int digit = 2);

    XmlProfile&     setContentBool      (bool value);

    XmlProfile&     setContentText      (const string& value);

    XmlProfile&     setContentValue     (const StringValue& value);


    XmlProfile&     setAttrInt          (const string& attr, int64 value);

    XmlProfile&     setAttrFloat        (const string& attr, double value, int digit = 2);

    XmlProfile&     setAttrExp          (const string& attr, double value, int digit = 2);

    XmlProfile&     setAttrBool         (const string& attr, bool value);

    XmlProfile&     setAttrText         (const string& attr, const string& value);

    XmlProfile&     setAttrValue        (const string& attr, const StringValue& value);


    XmlProfile&     setChildInt         (const string& name, int64 value);

    XmlProfile&     setChildFloat       (const string& name, double value, int digit = 2);

    XmlProfile&     setChildExp         (const string& name, double value, int digit = 2);

    XmlProfile&     setChildBool        (const string& name, bool value);

    XmlProfile&     setChildText        (const string& name, const string& value);

    XmlProfile&     setChildValue       (const string& name, const StringValue& value);


    XmlProfile&     setChildInt         (const string& name, const string& attr, int64 value);

    XmlProfile&     setChildFloat       (const string& name, const string& attr, double value, int digit = 2);

    XmlProfile&     setChildExp         (const string& name, const string& attr, double value, int digit = 2);

    XmlProfile&     setChildBool        (const string& name, const string& attr, bool value);

    XmlProfile&     setChildText        (const string& name, const string& attr, const string& value);

    XmlProfile&     setChildValue       (const string& name, const string& attr, const StringValue& value);

protected:
    typedef XmlNode* (XmlNode::*SiblingMethod)(const string&, const XmlAttr&) const;

    bool            moveToSiblingHelper (const string& name, const XmlAttr& attr, SiblingMethod func, int offset);

    XmlNode*        createNodePath      (XmlNode* node, const string& namePath, const XmlAttr& attr);

protected:
    XmlDocument m_document;
    XmlNode*    m_node;
    bool        m_createMode;
    bool        m_unamedNode;
};

END_NAMESPACE_LIB

#endif//LIB_XML_PROFILE_H
