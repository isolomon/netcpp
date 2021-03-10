#include "json.h"
#include "convert.h"
#include "errors.h"

BEGIN_NAMESPACE_LIB

JsonNode::JsonNode()
{
    m_type   = Null;
    m_bool   = false;
    m_float  = 0;
    m_parent = 0;
}

JsonNode::JsonNode(const string& name)
{
    m_name   = name;
    m_type   = Null;
    m_bool   = false;
    m_float  = 0;
    m_parent = 0;
}

JsonNode::JsonNode(const string& name, const string& value)
{
    m_name   = name;
    m_text   = value;
    m_type   = Text;
    m_bool   = false;
    m_float  = 0;
    m_parent = 0;
}

JsonNode::JsonNode(const string& name, bool value)
{
    m_name   = name;
    m_type   = Bool;
    m_bool   = value;
    m_float  = 0;
    m_parent = 0;
}

JsonNode::JsonNode(const string& name, double value)
{
    m_name   = name;
    m_type   = Number;
    m_bool   = false;
    m_float  = value;
    m_parent = 0;
}

JsonNode::~JsonNode()
{
    clear();
}

strings JsonNode::childNames()
{
    strings result;

    for (iterator it = m_nodes.begin(); it != m_nodes.end(); ++it) result.push_back((*it)->name());

    return result;
}

int JsonNode::childIndex(const string& name)
{
    for (int n = 0; n < m_nodes.size(); ++n) if (m_nodes[n]->name() == name) return n;
    return -1;
}

int JsonNode::childIndex(JsonNode* node)
{
    for (int n = 0; n < m_nodes.size(); ++n) if (m_nodes[n] == node) return n;
    return -1;
}

JsonNode* JsonNode::child(const string& key)
{
    for (iterator it = m_nodes.begin(); it != m_nodes.end(); ++it) if ((*it)->name() == key) return *it;
    return 0;
}

JsonNode* JsonNode::clone()
{
    JsonNode* node = new JsonNode();

    node->m_type  = m_type;
    node->m_bool  = m_bool;
    node->m_name  = m_name;
    node->m_text  = m_text;
    node->m_float = m_float;
    node->m_parent = 0;

    for (iterator it = m_nodes.begin(); it != m_nodes.end(); ++it)
    {
        node->appendChild((*it)->clone());
    }

    return node;
}

void JsonNode::setName(const string& name)
{
    m_name = name;
}

void JsonNode::setNull()
{
    m_type = Null;
}

void JsonNode::setBoolean(bool value)
{
    m_type = Bool;
    m_bool = value;
}

void JsonNode::setText(const string& value)
{
    m_type = Text;
    m_text = value;
}

void JsonNode::setNumber(double value)
{
    m_type = Number;
    m_float = value;
}

void JsonNode::setArray()
{
    m_type = Array;
}

void JsonNode::setObject()
{
    m_type = Object;
}

JsonNode* JsonNode::setChild(int index, JsonNode* node)
{
    if (node) node->setParent(this);

    int size = m_nodes.size();
    for (int n = size; n < index + 1; ++n) m_nodes.push_back(0);

    JsonNode*& prev = m_nodes[index];
    if (prev != node) freeNode(prev);

    prev = node;
    return node;
}

JsonNode* JsonNode::setChild(const string& name, JsonNode* node)
{
    int index = childIndex(name);
    if (index < 0) return appendChild(node);

    JsonNode*& prev = m_nodes[index];
    if (prev != node) freeNode(prev);

    prev = node;
    return node;
}

void JsonNode::setParent(JsonNode* node)
{
    m_parent = node;
}

JsonNode* JsonNode::setChildNull(int index)
{
    JsonNode* node = ensureCreated(index);

    node->setNull();
    return node;
}

JsonNode* JsonNode::setChildBool(int index, bool value)
{
    JsonNode* node = ensureCreated(index);

    node->setBoolean(value);
    return node;
}

JsonNode* JsonNode::setChildNum(int index, double value)
{
    JsonNode* node = ensureCreated(index);

    node->setNumber(value);
    return node;
}

JsonNode* JsonNode::setChildText(int index, const string& value)
{
    JsonNode* node = ensureCreated(index);

    node->setText(value);
    return node;
}

JsonNode* JsonNode::setChildNull(const string& name)
{
    JsonNode* node = ensureCreated(name);

    node->setNull();
    return node;
}

JsonNode* JsonNode::setChildBool(const string& name, bool value)
{
    JsonNode* node = ensureCreated(name);

    node->setBoolean(value);
    return node;
}

JsonNode* JsonNode::setChildNum(const string& name, double value)
{
    JsonNode* node = ensureCreated(name);
    
    node->setNumber(value);
    return node;
}

JsonNode* JsonNode::setChildText(const string& name, const string& value)
{
    JsonNode* node = ensureCreated(name);

    node->setText(value);
    return node;
}
//////////////////////////////////////////////////////////////////////////
JsonNode* JsonNode::insertChild(int index, JsonNode* node)
{
    m_nodes.insert(m_nodes.begin() + index, node);
    node->setParent(this);
    return node;
}

JsonNode* JsonNode::insertChild(int index)
{
    return insertChild(index, new JsonNode());
}

JsonNode* JsonNode::appendChild(JsonNode* node)
{
    m_nodes.push_back(node);
    node->setParent(this);
    return node;
}

JsonNode* JsonNode::appendChild()
{
    return appendChild(new JsonNode());
}

void JsonNode::freeNode(JsonNode* node)
{
    if (node && node->parent() == this) delete node;
}

JsonNode* JsonNode::ensureCreated(const string& name)
{
    int index = childIndex(name);
    return (index < 0) ? appendChild() : m_nodes[index];
}

JsonNode* JsonNode::ensureCreated(int index)
{
    if (index < 0) throw InvalidArgumentException();

    int size = m_nodes.size();
    for (int n = size; n < index + 1; ++n) m_nodes.push_back(0);

    JsonNode*& node = m_nodes[index];
    if (node == 0) node = new JsonNode();

    return node;
}

void JsonNode::removeChild(int index)
{
    m_nodes.erase(m_nodes.begin() + index);
}

void JsonNode::removeChild(const string& name)
{
    for (iterator it = m_nodes.begin(); it != m_nodes.end(); ++it)
    {
        if ((*it)->name() == name)
        {
            freeNode(*it);
            m_nodes.erase(it);
            break;
        }
    }
}

void JsonNode::removeAll()
{
    for (iterator it = m_nodes.begin(); it != m_nodes.end(); ++it) freeNode(*it);
    m_nodes.clear();
}

void JsonNode::clear()
{
    removeAll();

    m_name.clear();
    m_text.clear();
    m_type = Null;
    m_bool = false;
    m_float = 0;
}

//////////////////////////////////////////////////////////////////////////
JsonNode* JsonNode::fromString(const string& content)
{
    StringReader reader(content);
    return load(reader);
}

JsonNode* JsonNode::load(Stream* stream)
{
    StreamReader reader(stream);
    return load(reader);
}

JsonNode* JsonNode::load(const string& filename)
{
    StreamReader reader(filename);
    return load(reader);
}

JsonNode* JsonNode::load(StreamReader& reader)
{
    try
    {
        return readValue(reader);
    }
    catch(...)
    {
        logmsg("Error occurred while load json document\n");
        throw;
    }
}

JsonNode* JsonNode::readValue(StreamReader& r)
{
    JsonNode* result = 0;

    r.skipWhitespace();
    int ch = r.peek();

    if (ch == '{')
    {
        r.read();
        result = readObject(r);
    }
    else if (ch == '[')
    {
        r.read();
        result = readArray(r);
    }
    else if (ch == '\"')
    {
        string value = r.readQuoted();
        result = new JsonNode(string(), value);
    }
    else if (ch == 'n')
    {
        r.moveToFirstOf(",]}", false);
        result = new JsonNode();
    }
    else if (ch == 't' || ch == 'f')
    {
        r.moveToFirstOf(",]}", false);
        result = new JsonNode(string(), ch == 't');
    }
    else if (ch == '-' || isdigit(ch))
    {
        string value = r.readToFirstOf(",]}", false);
        result = new JsonNode(string(), Convert::toFloat(value));
    }
    else
    {
        throw FormatException();
    }

    r.skipWhitespace();
    if (r.peek() == ',') r.read();

    return result;
}

JsonNode* JsonNode::readArray(StreamReader& r)
{
    JsonNode* result = new JsonNode;
    result->setArray();

    try
    {
        for (;;)
        {
            r.skipWhitespace();
            if (r.peek() == ']') { r.read(); break; }

            JsonNode* value = readValue(r);
            result->appendChild(value);
        }

        return result;
    }
    catch (...)
    {
        delete result;
        throw;
    }
}

JsonNode* JsonNode::readObject(StreamReader& r)
{
    JsonNode* result = new JsonNode();
    result->setObject();

    try
    {
        for (;;)
        {
            r.skipWhitespace();

            int ch = r.peek();
            if (ch == '}') { r.read(); break; }

            string name = (ch == '\"') ? r.readQuoted() : r.readTo(':', false);
            logmsg("name = %s\n", name.c_str());

            if (name == "statuses_count")
            {
                int b = 0;
            }

            if (r.moveTo(':') == false) throw FormatException();

            JsonNode* value = readValue(r);
            value->setName(name);

            result->appendChild(value);
        }

        return result;
    }
    catch (...)
    {
        delete result;
        throw;
    }
}

//////////////////////////////////////////////////////////////////////////

string JsonNode::toString()
{
    string result;
    StringStream stream(result);
    StreamWriter writer(&stream);

    write(writer);

    return result;
}

void JsonNode::save(Stream* stream)
{
    StreamWriter writer(stream);
    write(writer);
}

void JsonNode::save(const string& filename)
{
    StreamWriter writer(filename);
    write(writer);
}

void JsonNode::write(StreamWriter& w)
{
    if (m_name.size()) w.writeFormat("\"%s\":", m_name.c_str());

    if (isArray())
    {
        w.write('[');

        for (int n = 0; n < childCount(); n++)
        {
            childAt(n)->write(w);
            if (n < childCount() - 1) w.write(',');
        }
        w.write(']');
    }
    else if (isObject())
    {
        w.write('{');

        for (int n = 0; n < childCount(); n++)
        {
            childAt(n)->write(w);
            if (n < childCount() - 1) w.write(',');
        }
        w.write('}');
    }
    else if (isNull())
    {
        w.write("null");
    }
    else if (isBool())
    {
        w.write(m_bool ? "true" : "false");
    }
    else if (isNumber())
    {
        w.writeFormat("%.20g", m_float);
    }
    else if (isText())
    {
        w.writeFormat("\"%s\"", m_text.c_str());
    }
}

END_NAMESPACE_LIB
