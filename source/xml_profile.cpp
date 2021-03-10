#include "xml_profile.h"
#include "files.h"
#include "errors.h"

BEGIN_NAMESPACE_LIB

XmlProfile::XmlProfile() : m_node(&m_document), m_createMode(true), m_unamedNode(false)
{
}

XmlProfile::XmlProfile(const string& path) : m_document(path), m_node(&m_document), m_createMode(false), m_unamedNode(false)
{
}

XmlProfile::XmlProfile(Stream* stream) : m_document(stream), m_node(&m_document), m_createMode(false), m_unamedNode(false)
{
}

XmlProfile::~XmlProfile()
{
}

void XmlProfile::load(const string& path)
{
    m_document.load(path);
    m_node = &m_document;
    m_createMode = false;
    m_unamedNode = false;
}

void XmlProfile::load(Stream* stream)
{
    m_document.load(stream);
    m_node = &m_document;
    m_createMode = false;
    m_unamedNode = false;
}

void XmlProfile::loadXml(const string& content)
{
    m_document.loadXml(content);
    m_node = &m_document;
    m_createMode = false;
    m_unamedNode = false;
}

void XmlProfile::save(const string& path)
{
    m_document.save(path);
}

void XmlProfile::save(Stream* stream)
{
    m_document.save(stream);
}

//////////////////////////////////////////////////////////////////////////

bool XmlProfile::moveTo(const string& namePath, const XmlAttr& attr)
{
    m_unamedNode = false;

    bool findAny = startWith(namePath, "//");
    
    // double slash match (//abc) will start from document node
    XmlNode* found = (findAny ? &m_document : m_node)->selectNode(namePath, attr);

    if (found)
    {
        m_node = found;
    }    
    else if (m_createMode)
    {
        string newPath = findAny ? trim(namePath, "/") : namePath;

        if (startWith(newPath, "/"))
        {
            // absolute node path should create from document node
            m_node = createNodePath(&m_document, newPath, attr);
        }
        else
        {
            m_node = createNodePath(m_node, namePath, attr);
        }
    }

    return found;
}

bool XmlProfile::moveToChild(const string& namePath, const XmlAttr& attr)
{
    m_unamedNode = false;

    // node selection will allways start at current node
    XmlNode* found = m_node->selectNode(namePath, attr);

    if (found)
    {
        m_node = found;
    }
    else if (m_createMode)
    {
        // only create the real path body from current node
        m_node = createNodePath(m_node, trim(namePath, "/."), attr);
    }

    return found;
}

bool XmlProfile::moveToChild()
{
    XmlNode* found = m_node->firstChild();

    if (found)
    {
        m_node = found;
        m_unamedNode = true;
    }
    else if (m_createMode)
    {
        XmlNode* node = new XmlNode();
        m_node->appendChild(node);
        m_node = node;
        m_unamedNode = true;
    }

    return found;
}

bool XmlProfile::moveToSibling(const string& name, const XmlAttr& attr)
{
    return moveToSiblingHelper(name, attr, &XmlNode::sibling, 1);
}

bool XmlProfile::moveToPrevSibling(const string& name, const XmlAttr& attr)
{
    return moveToSiblingHelper(name, attr, &XmlNode::prevSibling, 0);
}

bool XmlProfile::moveToNextSibling(const string& name, const XmlAttr& attr)
{
    return moveToSiblingHelper(name, attr, &XmlNode::nextSibling, 1);
}

bool XmlProfile::moveToSiblingHelper(const string& name, const XmlAttr& attr, SiblingMethod func, int offset)
{
    if (m_unamedNode)
    {
        m_unamedNode = false;

        if (m_createMode) { m_node->setName(name).setAttr(attr); return true; }
        else if (name.empty() || m_node->isNode(name, attr)) return true;
    }

    XmlNode* found = (m_node->*func)(name, attr);
    if (found) { m_node = found; return true; }

    if (m_createMode)
    {
        int index = m_node->indexOfThis();

        if (index > -1)
        {
            XmlNode* node = new XmlNode(name, attr);
            m_node->parent()->insertChild(index + offset, node);
            m_node = node;

            return true;
        }
    }

    return false;
}


bool XmlProfile::moveToParent()
{
    m_unamedNode = false;

    XmlNode* found = m_node->parent();
    if (found) m_node = found;

    return found;
}

//////////////////////////////////////////////////////////////////////////

int64 XmlProfile::contentInt()
{
    return Convert::toInt64(m_node->contentText());
}

bool XmlProfile::contentBool()
{
    return Convert::toBool(m_node->contentText());
}

double XmlProfile::contentFloat()
{
    return Convert::toFloat(m_node->contentText());
}

const string& XmlProfile::contentText()
{
    return m_node->contentText();
}

int64 XmlProfile::attrInt(const string& attr, int64 defval)
{
    XmlAttr* p = m_node->findAttr(attr);
    return p ? Convert::toInt64(p->value) : defval;
}

double XmlProfile::attrFloat(const string& attr, double defval)
{
    XmlAttr* p = m_node->findAttr(attr);
    return p ? Convert::toFloat(p->value) : defval;
}

bool XmlProfile::attrBool(const string& attr, bool defval)
{
    XmlAttr* p = m_node->findAttr(attr);
    return p ? Convert::toBool(p->value) : defval;
}

const string& XmlProfile::attrText(const string& attr)
{
    XmlAttr* p = m_node->findAttr(attr);
    return p ? p->value : XmlNode::nullStr();
}

int XmlProfile::attrText(const string& attr, char* buffer, int size)
{
    string text = attrText(attr);

    uint numBytes = min(text.size(), size - 1);
    strncpy(buffer, text.c_str(), numBytes);

    return numBytes;
}

int64 XmlProfile::childInt(const string& name, int64 defval)
{
    XmlNode* node = m_node->findChild(name);    
    return node ? Convert::toInt64(node->contentText()) : defval;
}

int64 XmlProfile::childInt(const string& name, const char* attr, int64 defval)
{
    XmlNode* node = m_node->findChild(name);
    XmlAttr* xatt = node ? node->findAttr(attr) : 0;

    return xatt ? Convert::toInt64(xatt->value) : defval;
}

double XmlProfile::childFloat(const string& name, double defval)
{
    XmlNode* node = m_node->findChild(name);
    return node ? Convert::toFloat(node->contentText()) : defval;
}

double XmlProfile::childFloat(const string& name, const char* attr, double defval)
{
    XmlNode* node = m_node->findChild(name);
    XmlAttr* xatt = node ? node->findAttr(attr) : 0;

    return xatt ? Convert::toFloat(xatt->value) : defval;
}

bool XmlProfile::childBool(const string& name, bool defval)
{
    XmlNode* node = m_node->findChild(name);    
    return node ? Convert::toBool(node->contentText()) : defval;
}

bool XmlProfile::childBool(const string& name, const char* attr, bool defval)
{
    XmlNode* node = m_node->findChild(name);
    XmlAttr* xatt = node ? node->findAttr(attr) : 0;

    return xatt ? Convert::toBool(xatt->value) : defval;
}

const string& XmlProfile::childText(const string& name)
{
    XmlNode* node = m_node->findChild(name);
    return node ? node->contentText() : XmlNode::nullStr();
}

int XmlProfile::childText(const string& name, char* buffer, int size)
{
    string text = childText(name);

    uint numBytes = min(text.size(), size - 1);
    strncpy(buffer, text.c_str(), numBytes);

    return numBytes;
}

const string& XmlProfile::childText(const string& name, const char* attr)
{
    XmlNode* node = m_node->findChild(name);
    XmlAttr* xatt = node ? node->findAttr(attr) : 0;

    return xatt ? xatt->value : XmlNode::nullStr();
}

int XmlProfile::childText(const string& name, const char* attr, char* buffer, int size)
{
    string text = childText(name, attr);

    uint numBytes = min(text.size(), size - 1);
    strncpy(buffer, text.c_str(), numBytes);

    return numBytes;
}

//////////////////////////////////////////////////////////////////////////

XmlProfile& XmlProfile::setContentFloat(double value, int digit)
{
    m_node->setInnerText(Convert::toString(value, digit));
    return *this;
}

XmlProfile& XmlProfile::setContentInt(int64 value)
{
    m_node->setInnerText(Convert::toString(value));
    return *this;
}

XmlProfile& XmlProfile::setContentBool(bool value)
{
    m_node->setInnerText(Convert::toString(value));
    return *this;
}

XmlProfile& XmlProfile::setContentText(const string& value)
{
    m_node->setInnerText(value);
    return *this;
}

XmlProfile& XmlProfile::setContentValue(const StringValue& value)
{
    m_node->setInnerText(value);
    return *this;
}

XmlProfile& XmlProfile::setAttrInt(const string& attr, int64 value)
{
    return setAttrText(attr, Convert::toString(value));
}

XmlProfile& XmlProfile::setAttrFloat(const string& attr, double value, int digit)
{
    return setAttrText(attr, Convert::toString(value, digit));
}

XmlProfile& XmlProfile::setAttrExp(const string& attr, double value, int digit)
{
    return setAttrText(attr, Convert::toString(value, digit, 'e'));
}

XmlProfile& XmlProfile::setAttrBool(const string& attr, bool value)
{
    return setAttrText(attr, Convert::toString(value));
}

XmlProfile& XmlProfile::setAttrText(const string& attr, const string& value)
{
    m_node->setAttr(attr, value);
    return *this;
}

XmlProfile& XmlProfile::setAttrValue(const string& attr, const StringValue& value)
{
    m_node->setAttr(attr, value);
    return *this;
}

XmlProfile& XmlProfile::setChildInt(const string& name, int64 value)
{
    return setChildText(name, Convert::toString(value));
}

XmlProfile& XmlProfile::setChildInt(const string& name, const string& attr, int64 value)
{
   return setChildText(name, attr, Convert::toString(value));
}

XmlProfile& XmlProfile::setChildFloat(const string& name, double value, int digit)
{
    return setChildText(name, Convert::toString(value, digit));
}

XmlProfile& XmlProfile::setChildFloat(const string& name, const string& attr, double value, int digit)
{
   return setChildText(name, attr, Convert::toString(value, digit));
}

XmlProfile& XmlProfile::setChildExp(const string& name, const string& attr, double value, int digit)
{
    return setChildText(name, attr, Convert::toString(value, digit, 'e'));
}

XmlProfile& XmlProfile::setChildExp(const string& name, double value, int digit)
{
    return setChildText(name, Convert::toString(value, digit, 'e'));
}

XmlProfile& XmlProfile::setChildBool(const string& name, bool value)
{
   return setChildText(name, Convert::toString(value));
}

XmlProfile& XmlProfile::setChildBool(const string& name, const string& attr, bool value)
{
    return setChildText(name, attr, Convert::toString(value));
}

XmlProfile& XmlProfile::setChildText(const string& name, const string& value)
{
    XmlNode* node = m_node->selectNode(name);

    if (node) node->setInnerText(value);
    else m_node->appendChild(name, value);

    return *this;
}

XmlProfile& XmlProfile::setChildValue(const string& name, const StringValue& value)
{
    return setChildText(name, value);
}

XmlProfile& XmlProfile::setChildText(const string& name, const string& attr, const string& value)
{
    XmlNode* node = m_node->selectNode(name);

    if (node) node->setAttr(attr, value);
    else m_node->appendChild(name).appendAttr(attr, value);

    return *this;
}

XmlProfile& XmlProfile::setChildValue(const string& name, const string& attr, const StringValue& value)
{
    return setChildText(name, attr, value);
}

XmlNode* XmlProfile::createNodePath(XmlNode* node, const string& namePath, const XmlAttr& attr)
{
    if (startWith(namePath, ".."))
    {
        XmlNode* parent = node->parent();

        if (parent)
        {
            string newPath = trimStart(namePath, "."); trimStart(newPath, "/");
            return createNodePath(parent, newPath, attr);
        }   
    }

    string newPath  = trim(namePath, "./");
    uint   slash    = newPath.find('/');
    string thisName = newPath.substr(0, slash);

    bool isLeaf = (slash == string::npos);

    XmlNode* thisNode = node->selectNode(thisName, isLeaf ? attr : XmlAttr());

    if (thisNode == 0)
    {
        thisNode = new XmlNode(thisName);
        if (isLeaf) thisNode->appendAttr(attr);

        node->appendChild(thisNode);
    }

    return isLeaf ? thisNode : createNodePath(thisNode, newPath.substr(slash + 1), attr);
}

END_NAMESPACE_LIB
