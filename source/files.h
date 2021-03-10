#ifndef LIB_FILES_H
#define LIB_FILES_H

#include "stream.h"
#include "datetime.h"

BEGIN_NAMESPACE_LIB

struct FileMode
{
    enum
    {
        Create,         // create if not exist, otherwise truncate it
        CreateNew,      // ensure the file is created
        OpenOrCreate,   // open an existing file or create a new file
        Open,           // open an existing file
        Append,         // open an existing file and move the file end
        Truncate,       // open an existing file and truncate it
    };
};

struct FileAccess
{
    enum
    {
        ReadOnly  = 0x01,
        WriteOnly = 0x02,
        ReadWrite = 0x03,
    };
};

class FileStream : public Stream
{
public:
    FileStream (const char* filename, int mode, int access)   { init(filename, mode, access);           }
    FileStream (const string& filename, int mode, int access) { init(filename.c_str(), mode, access);   }
    virtual ~FileStream ();

    virtual bool canRead        ();
    virtual bool canWrite       ();
    virtual bool canSeek        ();

    // operations
    virtual int  read           (void* data, int offset, int size);
    virtual int  write          (const void* data, int offset, int size);
    virtual int  seek           (int offset, int origin);
    virtual int  position       ();
    virtual int  length         ();
    virtual void setLength      (int value);
    virtual void flush          ();
    virtual void close          ();

protected:
    void init (const char* filename, int mode, int access);

private:
    #ifdef WIN32
    Handle m_handle;
    #else
    int m_handle;
    #endif
    int m_access;
};

//////////////////////////////////////////////////////////////////////////
class File
{
public:
    static FileStream*  create          (const char* path);
    static FileStream*  open            (const char* path, int mode, int access = FileAccess::ReadWrite);
    static FileStream*  openRead        (const char* path);
    static FileStream*  openWrite       (const char* path);
    static FileStream*  openAppend      (const char* path);

    static void         truncate        (const char* path)  { FileStream(path, FileMode::Create, FileAccess::ReadWrite); }
    static void         touch           (const char* path);
    static void         copy            (const char* sourcePath, const char* destPath, bool overwrite);
    static void         move            (const char* sourcePath, const char* destPath, bool overwrite);
    static bool         remove          (const char* path);
    static bool         exists          (const char* path);
    static bool         isMissing       (const char* path)  { return !exists(path); }
    static int          length          (const char* path);
    static int          totalLines      (const char* path);

    static FileStream*  create          (const string& path)            { return create(path.c_str());      }
    static FileStream*  open            (const string& path, int mode, int access = FileAccess::ReadWrite)  { return open(path.c_str(), mode, access);  }
    static FileStream*  openRead        (const string& path)            { return openRead(path.c_str());    }
    static FileStream*  openWrite       (const string& path)            { return openWrite(path.c_str());   }
    static FileStream*  openAppend      (const string& path)            { return openAppend(path.c_str());  }

    static void         truncate        (const string& path)            { return truncate(path.c_str());    }
    static void         touch           (const string& path)            { return touch(path.c_str());       }
    static void         copy            (const string& sourcePath, const string& destPath, bool overwrite)  { copy(sourcePath.c_str(), destPath.c_str(), overwrite); }
    static void         move            (const string& sourcePath, const string& destPath, bool overwrite)  { move(sourcePath.c_str(), destPath.c_str(), overwrite); }
    static bool         remove          (const string& path)            { return remove(path.c_str());      }
    static bool         exists          (const string& path)            { return exists(path.c_str());      }
    static bool         isMissing       (const string& path)            { return !exists(path.c_str());     }
    static int          length          (const string& path)            { return length(path.c_str());      }
    static int          totalLines      (const string& path)            { return totalLines(path.c_str());  }

    static DateTime     modifyTime      (const char* path);
    static DateTime     accessTime      (const char* path);
    static DateTime     modifyTime      (const string& path)            { return modifyTime(path.c_str());  }
    static DateTime     accessTime      (const string& path)            { return accessTime(path.c_str());  }

    static void         setAccessTime   (const char* path,   const DateTime& value);
    static void         setModifyTime   (const char* path,   const DateTime& value);    
    static void         setAccessTime   (const string& path, const DateTime& value) { setAccessTime(path.c_str(), value); }
    static void         setModifyTime   (const string& path, const DateTime& value) { setModifyTime(path.c_str(), value); }

    static int          append          (const char* path,   const void* data, int size);
    static int          append          (const char* path,   const char* text)   { return append(path, text, text ? strlen(text) : 0); }
    static int          append          (const string& path, const string& text) { return append(path.c_str(), text.c_str(), text.size()); }
	
    static int          appendLine      (const char* path, const void* data, int size);
    static int          appendLine      (const char* path, const char* text)     { return appendLine(path, text, text ? strlen(text) : 0); }
    static int          appendLine      (const string& path, const string& text) { return appendLine(path.c_str(), text.c_str(), text.size()); }

    static string       readContent     (Stream* stream);
    static string       readContent     (const char* path);
    static string       readContent     (const string& path) { return readContent(path.c_str()); }

    static void         writeContent    (Stream* stream, const string& content);
    static void         writeContent    (const char*  path, const string& content);
    static void         writeContent    (const string& path, const string& content) { writeContent(path.c_str(), content); }

    static strings      readAllLines    (const char* path);
    static void         writeAllLines   (const char* path, const strings& lines);
};

class Directory
{
public:
    static void         create          (const char* path, bool recursive = true);
    static void         copy            (const char* sourcePath, const char* destPath, bool overwrite);
    static void         move            (const char* sourcePath, const char* destPath, bool overwrite);
    static void         remove          (const char* path);
    static bool         exists          (const char* path);

    static void         purge           (const char* path);
    static void         purge           (const string& path) { purge(path.c_str()); }

    static void         create          (const string& path, bool recursive = true) { create(path.c_str(), recursive); }
    static void         copy            (const string& sourcePath, const string& destPath, bool overwrite) { copy(sourcePath.c_str(), destPath.c_str(), overwrite); }
    static void         move            (const string& sourcePath, const string& destPath, bool overwrite) { move(sourcePath.c_str(), destPath.c_str(), overwrite); }
    static void         remove          (const string& path) { remove(path.c_str());}
    static bool         exists          (const string& path) { return exists(path.c_str()); }

    static strings      getDirectories  (const char* path);
    static strings      getFiles        (const char* path, const char* ends = 0, bool trimEnds = false);

    static strings      getDirectories  (const string& path)    { return getDirectories(path.c_str()); }
    static strings      getFiles        (const string& path, const char* ends = 0, bool trimEnds = false)  { return getFiles(path.c_str(), ends, trimEnds); }

    static string       currentDir      ();
    static void         changeDir       (const char* path);
};

class Path
{
public:
    static string       combine         (const char* path1, const char* path2);
    static string       combine         (const char* path1, const char* path2, const char* path3);
    static string       combine         (const char* path1, const char* path2, const char* path3, const char* path4);
    static string       normalize       (const char* path);
    static string       relative        (const char* from, const char* to);

    static string       combine         (const string& path1, const string& path2)
                                        { return combine(path1.c_str(), path2.c_str()); }
    static string       combine         (const string& path1, const string& path2, const string& path3)
                                        { return combine(path1.c_str(), path2.c_str(), path3.c_str()); }
    static string       combine         (const string& path1, const string& path2, const string& path3, const string& path4)
                                        { return combine(path1.c_str(), path2.c_str(), path3.c_str(), path4.c_str()); }
    
    static string       toUnix          (const string& path);
    static string       toWindows       (const string& path);

    static string       getDirName      (const char* path);
    static string       getDirName      (const string& path)    { return getDirName(path.c_str()); }

    static string       getExtention    (const char* path);
    static string       getExtention    (const string& path);

    static string       getFileName     (const string& path);
    static string       getFileNameWithoutExtention (const string& path);
    
    static bool         isUrl           (const string& path);
    static bool         isRooted        (const string& path);
	
    static string       freeDirName     (const string& path, const string& name, const char* suffix = "_%d");
    static string       freeFileName    (const string& path, const string& name, const char* suffix = "_%d");

protected:
    struct PathComponent { const char* name; short length; bool upper; };
    
    static int          parseComponents  (const char* path, PathComponent* components);
    static int          parseComponents  (const char* path, PathComponent* components, int offset);
    static string       componentsToPath (PathComponent* components, int count);
};

END_NAMESPACE_LIB

#endif//LIB_FILES_H
