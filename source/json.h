#ifndef LIB_JSON_H
#define LIB_JSON_H

#include "reader.h"
#include "writer.h"

BEGIN_NAMESPACE_LIB

class JsonNode;

typedef std::vector<JsonNode*> JsonNodes;

class JsonNode
{
public:
    enum NodeType { Null = 0, Bool, Integer, Number, Text, Array, Object, ArrayEnd, ObjectEnd, };
    typedef JsonNodes::iterator iterator;

public:
    JsonNode();

    JsonNode(const string& name);

    JsonNode(const string& name, const string& value);

    JsonNode(const string& name, bool value);

    JsonNode(const string& name, double value);

    ~JsonNode();

public:
    const string&   name            ()  { return m_name;            }

    NodeType        type            ()  { return (NodeType)m_type;  }

    const string&   text            ()  { return m_text;            }

    double          number          ()  { return m_float;           }

    bool            boolean         ()  { return m_bool;            }

    bool            isNull          ()  { return m_type == Null;    }

    bool            isBool          ()  { return m_type == Bool;    }

    bool            isNumber        ()  { return m_type == Number;  }

    bool            isText          ()  { return m_type == Text;    }

    bool            isArray         ()  { return m_type == Array;   }

    bool            isObject        ()  { return m_type == Object;  }


    strings         childNames      ();

    int             childIndex      (JsonNode* node);

    int             childIndex      (const string& name);

    int             childCount      ()          { return m_nodes.size(); }

    JsonNode*       childAt         (int index) { return m_nodes[index]; }

    JsonNode*       child           (const string& name);

    JsonNode*       parent          ()  { return m_parent;      }

    JsonNode*       clone           ();

    //////////////////////////////////////////////////////////////////////////
    void            setName         (const string& name);

    void            setNull         ();

    void            setBoolean      (bool value);
    
    void            setNumber       (double value);

    void            setText         (const string& value);

    void            setParent       (JsonNode* node);

    void            setArray        ();

    void            setObject       ();

    // child operations will not modify the node type, youn need to call setArray or setObject explicitly
    JsonNode*       setChildNull    (int index);
    JsonNode*       setChildBool    (int index, bool value);
    JsonNode*       setChildNum     (int index, double value);
    JsonNode*       setChildText    (int index, const string& value);
    JsonNode*       setChild        (int index, JsonNode* value);

    JsonNode*       setChildNull    (const string& name);
    JsonNode*       setChildBool    (const string& name, bool value);
    JsonNode*       setChildNum     (const string& name, double value);
    JsonNode*       setChildText    (const string& name, const string& value);
    JsonNode*       setChild        (const string& name, JsonNode* value);

    JsonNode*       insertChild     (int index);
    JsonNode*       insertChild     (int index, JsonNode* node);

    JsonNode*       appendChild     ();
    JsonNode*       appendChild     (JsonNode* node);

    void            removeChild     (int index);
    void            removeChild     (const string& name);
    void            removeAll       ();

    void            clear           ();

public:
    static JsonNode*    load        (const string& filename);
    static JsonNode*    load        (Stream* stream);
    static JsonNode*    load        (StreamReader& reader);
    static JsonNode*    fromString  (const string& content);

    void                save        (const string& filename);
    void                save        (Stream* stream);
    void                write       (StreamWriter& reader);
    string              toString    ();

protected:
    static JsonNode*    readValue    (StreamReader& r);
    static JsonNode*    readObject   (StreamReader& r);
    static JsonNode*    readArray    (StreamReader& r);

protected:
    JsonNode (const JsonNode& other);
    JsonNode& operator = (const JsonNode& other);

    union value_type { bool bv; double fv; int64 iv; };

    void        freeNode (JsonNode* node);
    JsonNode*   ensureCreated (int index);
    JsonNode*   ensureCreated (const string& name);

protected:
    byte        m_type;
    bool        m_bool;
    string      m_name;
    string      m_text;
    double      m_float;

    JsonNodes   m_nodes;
    JsonNode*   m_parent;
};

END_NAMESPACE_LIB

#endif //LIB_JSON_H
