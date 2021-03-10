#include "xml_document.h"
#include "xml_reader.h"
#include "xml_writer.h"
#include "errors.h"

BEGIN_NAMESPACE_LIB

const string XmlNode::m_null;

//////////////////////////////////////////////////////////////////////////
XmlNode::XmlNode(Type type)
{
    m_type   = type;
    m_parent = 0;
    m_owndoc = 0;
}

XmlNode::XmlNode(const string& nameValue, Type type)
{
    m_type = type;

    if (isLeaf()) m_value = nameValue;
    else m_name = nameValue;

    m_parent = 0;
    m_owndoc = 0;
}

XmlNode::XmlNode(const string& name, const XmlAttr& attr)
{
    m_type = Element;
    m_name = name;
    m_parent = 0;
    m_owndoc = 0;

    if (attr.isValid()) m_attrs.push_back(attr);
}

XmlNode::~XmlNode()
{
    removeAllChild();
    removeAllAttr();
}

void XmlNode::write(XmlWriter& w) const
{
    if (m_type == Document)
    {
        XmlNode* decl = findChild("xml");

        if (decl == 0 || decl->type() != XmlDeclaration)
        {
            w.declaration("xml").attribute("version", "1.0").attribute("encoding", "utf-8").end();
        }

        for (int n = 0; n < childCount(); n++)
        {
            child(n)->write(w);
        }
    }
    else if (m_type == Element && m_name.size())
    {
        w.element(m_name);

        for (int n = 0; n < attrCount(); n++)
        {
            const XmlAttr* xmlAttr = attr(n);
            w.attribute(xmlAttr->name, xmlAttr->value);
        }

        for (int n = 0; n < childCount(); n++)
        {
            child(n)->write(w);
        }

        w.end();
    }
    else if (m_type == Text)
    {
        w.text(m_value);
    }
    else if (m_type == Comments)
    {
        w.comments(m_value);
    }
}

void XmlNode::setOwnerDocument(XmlDocument* doc)
{
    m_owndoc = doc;
    for (int n = 0; n < childCount(); n++) child(n)->setOwnerDocument(doc);
}

string XmlNode::path() const
{
    if (m_type == Document) return "/";

    if (m_parent)
    {
        string base = trimEnd(m_parent->path(), "/");
        return base + "/" + m_name;
    }

    return string();
}

string XmlNode::innerXml() const
{
    string result;
    StringStream stream(result);

    XmlWriter w(&stream);

    for (int n = 0; n < childCount(); n++)
        child(n)->write(w);
    
    return result;
}

string XmlNode::outerXml() const
{
    string result;
    StringStream stream(result);

    XmlWriter w(&stream);
    write(w);

    return result;
}

const string& XmlNode::contentText() const
{
    for (int n = 0; n < m_nodes.size(); n++)
    {
        if (m_nodes[n]->isText()) return m_nodes[n]->value();
    }

    return m_null;
}

string XmlNode::innerText() const
{
    if (isLeaf()) return m_value;

    string result;

    for (int n = 0; n < m_nodes.size(); n++)
    {
        result += m_nodes[n]->innerText();
    }

    return result;
}

void XmlNode::setInnerText(const string& value)
{
    if (isLeaf()) { m_value = value; return; }

    if (childCount() == 1 && child(0)->type() == Text)
    {
        child(0)->setValue(value);
    }
    else
    {
        removeAllChild();
        appendChild(new XmlNode(value, Text));
    }
}

XmlNode* XmlNode::clone() const
{
    XmlNode* newNode = new XmlNode(m_type);

    newNode->setName(m_name);
    newNode->setValue(m_value);
    newNode->m_attrs = m_attrs;

    for (int n = 0; n < m_nodes.size(); n++)
    {
        XmlNode* child = m_nodes[n]->clone();
        newNode->appendChild(child);
    }

    return newNode;
}

XmlNode& XmlNode::insertAttr(int index, const string& name, const string& value)
{
    if (name.size()) m_attrs.insert(m_attrs.begin() + index, XmlAttr(name, value));
    return *this;
}

XmlNode& XmlNode::insertAttr(int index, const XmlAttr& attr)
{
    if (attr.isValid()) m_attrs.insert(m_attrs.begin() + index, attr);
    return *this;
}

XmlNode& XmlNode::appendAttr(const string& name, const string& value)
{
    if (name.size()) m_attrs.push_back(XmlAttr(name, value));
    return *this;
}

XmlNode& XmlNode::appendAttr(const XmlAttr& attr)
{
    if (attr.isValid()) m_attrs.push_back(attr);
    return *this;
}

void XmlNode::removeAttr(const string& name)
{
    int index = indexOfAttr(name);
    if (index > -1) removeAttr(index);
}

void XmlNode::removeAttr(int index)
{
    m_attrs.erase(m_attrs.begin() + index);
}

void XmlNode::removeAllAttr()
{
    m_attrs.clear();
}

XmlNode& XmlNode::insertChild(int index, XmlNode* node)
{
    if (node->parent()) node->parent()->removeChild(node);

    m_nodes.insert(m_nodes.begin() + index, node);
    node->m_parent = this;
    node->setOwnerDocument(m_owndoc);

    return *node;
}

XmlNode& XmlNode::appendChild(XmlNode* node)
{
    if (node->parent()) node->parent()->removeChild(node);

    m_nodes.push_back(node);
    node->m_parent = this;
    node->setOwnerDocument(m_owndoc);

    return *node;
}

XmlNode& XmlNode::appendChild(const string& name, const string& value)
{
    XmlNode* node = new XmlNode(name);
    node->appendChild(new XmlNode(value, Text));

    return appendChild(node);
}

XmlNode& XmlNode::appendChild(const string& name)
{
    XmlNode* node = new XmlNode(name);
    return appendChild(node);
}

void XmlNode::removeChild(XmlNode* node)
{
    int index = indexOfChild(node);
    if (index > -1) removeChild(index);
}

void XmlNode::removeChild(int index)
{
    XmlNode* node = m_nodes[index];
    delete node;

    m_nodes.erase(m_nodes.begin() + index);
}

void XmlNode::removeAllChild()
{
    for (int n = 0; n < childCount(); n++)
    {
        delete child(n);
    }

    m_nodes.clear();
}

void XmlNode::remove()
{
    if (m_parent) m_parent->removeChild(this);
    else delete this;
}

XmlNode* XmlNode::sibling(const string& name, const XmlAttr& attr) const
{
    XmlNode* next = nextSibling(name, attr);
    return next ? next : prevSibling(name, attr);
}

XmlNode* XmlNode::prevSibling(const string& name, const XmlAttr& attr) const
{
    if (m_parent)
    {
        int index = m_parent->indexOfChild(this);

        for (int n = index - 1; n > -1; n--)
        {
            XmlNode* node = m_parent->child(n);
            if ((name.empty() || node->name() == name) && node->hasAttr(attr)) return node;
        }
    }

    return 0;
}

XmlNode* XmlNode::nextSibling(const string& name, const XmlAttr& attr) const
{
    if (m_parent)
    {
        int index = m_parent->indexOfChild(this);

        for (int n = index + 1; n < m_parent->childCount(); n++)
        {
            XmlNode* node = m_parent->child(n);
            if ((name.empty() || node->name() == name) && node->hasAttr(attr)) return node;
        }
    }

    return 0;
}

XmlNode* XmlNode::prevSibling() const
{
    int index = m_parent ? m_parent->indexOfChild(this) : -1;
    if (index > 0 ) return m_parent->m_nodes[index - 1];

    return 0;
}

XmlNode* XmlNode::nextSibling() const
{
    int index = m_parent ? m_parent->indexOfChild(this) : -1;
    if (index >-1 && index < m_parent->childCount() - 1) return m_parent->m_nodes[index + 1];

    return 0;
}

XmlNode* XmlNode::findChild(const string& name, int sequence) const
{
    for (int n = 0 ; n < childCount(); n++)
    {
        if ((name.empty() || child(n)->name() == name) && sequence-- == 0) return child(n);
    }

    return 0;
}

XmlNode* XmlNode::findChild(const string& name, const XmlAttr& attr, int sequence) const
{
    for (int n = 0 ; n < childCount(); n++)
    {
        if ((name.empty() || child(n)->name() == name) && 
            child(n)->hasAttr(attr) &&
            sequence-- == 0) return child(n);
    }

    return 0;
}

XmlNode* XmlNode::findChild(const XmlAttr& attr, int sequence) const
{
    for (int n = 0 ; n < childCount(); n++)
    {
        if (child(n)->hasAttr(attr) && sequence-- == 0) return child(n);
    }

    return 0;
}

bool XmlNode::hasAttr(const XmlAttr& attr) const
{
    if (attr.isNull()) return true;

    bool nameOnly = attr.isEmpty();

    for (int n = 0; n < m_attrs.size(); n++)
    {
        if (nameOnly) { if (m_attrs[n].name == attr.name) return true; }
        else if (m_attrs[n] == attr) return true;        
    }

    return false;
}

const XmlAttr* XmlNode::findAttr(const string& name) const
{
    for (int n = 0; n < m_attrs.size(); n++)
    {
        if (m_attrs[n].name == name) return &m_attrs[n];
    }

    return 0;
}

XmlAttr* XmlNode::findAttr(const string& name)
{
    for (int n = 0; n < m_attrs.size(); n++)
    {
        if (m_attrs[n].name == name) return &m_attrs[n];
    }

    return 0;
}

const string& XmlNode::attr(const string& name) const
{
    for (int n = 0; n < m_attrs.size(); n++)
    {
        if (m_attrs[n].name == name) return m_attrs[n].value;
    }

    throw NotFoundException();
}

XmlNode& XmlNode::setAttr(const XmlAttr& attr)
{
    if (attr.isNull()) return *this;

    XmlAttr* xmlAttr = findAttr(attr.name);
    if (xmlAttr) xmlAttr->value = attr.value;
    else appendAttr(attr);

    return *this;
}

int XmlNode::indexOfChild(const XmlNode* node) const
{
    for (int n = 0; n < m_nodes.size(); n++)
    {
        if (m_nodes[n] == node) return n;
    }

    return -1;
}

int XmlNode::indexOfAttr(const string& name) const
{
    for (int n = 0; n < m_attrs.size(); n++)
    {
        if (m_attrs[n].name == name) return n;
    }

    return -1;
}

XmlNode* XmlNode::selectNode(const string& namePath, const XmlAttr& attr)
{
    if (startWith(namePath, ".."))
    {
        string newPath = trim(namePath.substr(2), "/");
        if (newPath.empty()) return m_parent;

        return m_parent ? m_parent->selectNode(newPath, attr) : 0;
    }

    bool findAny = false, findRoot = false;
    if (startWith(namePath, "//")) findAny = true;
    else if (startWith(namePath, "/")) findRoot = true;

    strings names = split(trim(namePath, "/"), "/");

    if (findRoot) return m_owndoc ? m_owndoc->internalFind(names, 0, attr, false) : 0;

    return internalFind(names, 0, attr, findAny);
}

XmlNodes XmlNode::selectNodes(const string& namePath, const XmlAttr& attr)
{
    XmlNodes results;

    if (startWith(namePath, ".."))
    {
        string newPath = trim(namePath.substr(2), "/");
        if (newPath.empty()) { results.push_back(m_parent); return results; }

        return m_parent ? m_parent->selectNodes(newPath, attr) : results;
    }

    bool findAny = false, findRoot = false;
    if (startWith(namePath, "//")) findAny = true;
    else if (startWith(namePath, "/")) findRoot = true;
        
    strings names = split(trim(namePath, "/"), "/");

    if (findRoot)
    {
        if (m_owndoc) m_owndoc->internalFindAll(names, 0, attr, false, results);
        return results;
    }
    
    internalFindAll(names, 0, attr, findAny, results);
    return results;
}

XmlNode* XmlNode::findNode(const string& namePath, const XmlAttr& attr)
{
    strings names = split(trim(namePath, "./"), "/");
    return internalFind(names, 0, attr, true);
}

XmlNodes XmlNode::findNodes(const string& namePath, const XmlAttr& attr)
{
    XmlNodes results;
    strings names = split(trim(namePath, "./"), "/");
    internalFindAll(names, 0, attr, true, results);
    return results;
}

//////////////////////////////////////////////////////////////////////////
XmlNode* XmlNode::internalFind(const strings& names, int level, const XmlAttr& attr, bool findAny)
{
    if (level >= names.size()) return 0;
    if (names[level] == ".") return internalFind(names, level + 1, attr, findAny);

    for (int n = 0; n < childCount(); n++)
    {
        XmlNode* node = child(n);

        if (node->name() == names[level])
        {
            if (level == names.size() - 1 && node->hasAttr(attr)) return node;

            XmlNode* found = node->internalFind(names, level + 1, attr, findAny);
            if (found) return found;
        }
        else if (findAny)
        {
            XmlNode* found = node->internalFind(names, 0, attr, findAny);
            if (found) return found;
        }
    }

    return 0;
}

//////////////////////////////////////////////////////////////////////////
void XmlNode::internalFindAll(const strings& names, int level, const XmlAttr& attr, bool findAny, XmlNodes& results)
{
    if (level >= names.size()) return;

    if (names[level] == ".")
    {
        internalFind(names, level + 1, attr, findAny);
        return;
    }

    for (int n = 0; n < childCount(); n++)
    {
        XmlNode* node = child(n);

        if (node->name() == names[level])
        {
            if (level == names.size() - 1 && node->hasAttr(attr)) results.push_back(node);

            node->internalFindAll(names, level + 1, attr, findAny, results);
        }
        else if (findAny)
        {
            node->internalFindAll(names, 0, attr, findAny, results);
        }
    }
}

string XmlNode::unescape(const string& value)
{
    int pos = value.find('&');
    if (pos == string::npos) return value;

    string result = value;

    replace(result, "&amp;",   "&" );
    replace(result, "&apos;",  "'" );
    replace(result, "&quot;",  "\"");
    replace(result, "&lt;",    "<" );
    replace(result, "&gt;",    ">" );

    return result;
}

string XmlNode::escape(const string& value)
{
    int pos = value.find_first_of("'\"&<>");
    if (pos == string::npos) return value;

    string result = value;
        
    replace(result, "&",  "&amp;");
    replace(result, "'",  "&apos;");
    replace(result, "\"", "&quot;");
    replace(result, "<",  "&lt;");
    replace(result, ">",  "&gt;");

    return result;
}

//////////////////////////////////////////////////////////////////////////
XmlDocument::XmlDocument() : XmlNode(Document)
{
    m_owndoc = this;
}

XmlDocument::XmlDocument(const string& filename) : XmlNode(Document)
{
    m_owndoc = this;

    try
    {
        load(filename);
    }
    catch(...)
    {
        clear();
        throw;
    }
}

XmlDocument::XmlDocument(Stream* stream) : XmlNode(Document)
{
    m_owndoc = this;

    try
    {
        load(stream);
    }
    catch(...)
    {
        clear();
        throw;
    }
}

XmlDocument::~XmlDocument()
{
    clear();
}

XmlNode* XmlDocument::rootElement()
{
    for (int n = 0; n < childCount(); n++)
    {
        XmlNode* node = child(n);
        if (node->type() == Element) return node;
    }

    return 0;
}

void XmlDocument::clear()
{
    removeAllChild();
    removeAllAttr();
}

void XmlDocument::loadXml(const string& content)
{
    StringStream stream(content);
    XmlReader r(stream);
    load(r);
}

void XmlDocument::load(const string& filename)
{
    XmlReader r(filename);
    load(r);
}

void XmlDocument::load(Stream* stream)
{
    XmlReader r(stream, false);
    load(r);
}

void XmlDocument::load(XmlReader& r)
{
    XmlNode* currentNode = this;

    clear();

    while (r.read())
    {
        if (r.isStartElement())
        {
            XmlNode* node = r.node().clone();
            currentNode->appendChild(node);

            //save root element for quick access
            if (m_root == 0) m_root = node;

            if (!r.isEmptyElement()) currentNode = node;
        }
        else if (r.isEndElement())
        {
            currentNode = currentNode->parent();
            if (currentNode == 0) currentNode = this;
        }
        else if (r.isText())
        {
            XmlNode* node = r.node().clone();
            currentNode->appendChild(node);
        }
    }
}

void XmlDocument::save(const string& filename)
{
    XmlWriter w(filename);
    write(w);
}

void XmlDocument::save(XmlWriter& writer)
{
    write(writer);
}

void XmlDocument::save(Stream* stream)
{
    XmlWriter w(stream);
    write(w);
}

END_NAMESPACE_LIB
