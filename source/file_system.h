#ifndef FILE_SYSTEM_H
#define FILE_SYSTEM_H

#include "types.h"
#include "smart.h"
#include "delegate.h"
#include "datetime.h"

BEGIN_NAMESPACE_LIB

class Stream;

class FileItem;
class FolderItem;
class FileSystem;

typedef std::vector<FileItem*> FileItems;

struct OverwriteCtrl
{
    enum {
        Unkown   = 0,
        Yes      = 1,
        No       = 2,
        YesToAll = 3,
        NoToAll  = 4,
    };
};

struct FileCopyContext
{
    int64       bytesCopied;
    int64       totalBytes;
    int         filesCopied;
    int         totalFiles;

    const char* sourceFile;
    const char* targetFile;

    bool        cancel;
    bool        exists;
    int         overwrite;

    bool        skipExists () { return overwrite == OverwriteCtrl::No || overwrite == OverwriteCtrl::NoToAll; }

    FileCopyContext() : bytesCopied(0), totalBytes(0), filesCopied(0), totalFiles(0),
        sourceFile(0), targetFile(0), cancel(false), exists(false), overwrite(0) {}
};

typedef delegate<void, FileCopyContext*> FileCopyHandler;

enum FilePermissions
{
    AllowNone      = 0,
    AllowList      = 0x01,
    AllowRead      = 0x02,
    AllowWrite     = 0x04,
    AllowRemove    = 0x08,
    AllowRename    = 0x10,
    AllowCreate    = 0x20,
};

class FileItem : public RefCounted
{
public:
    virtual string      name        ();

    virtual int64       size        ();

    virtual DateTime    date        ();

    virtual int         type        ();

    virtual string      path        ();

    virtual int         permissions ();

    virtual FolderItem* parent      ();

    virtual bool        isFolder    ();

    virtual bool        exists      ();

    virtual FileSystem* fileSystem  ();
    
    virtual bool        remove      ();

    virtual bool        rename      (const string& newName);

    virtual Stream*     open        (int mode, int access);

    virtual void        setType     (int value);

    bool                isFile      ()  { return !isFolder(); }

protected:
    FileItem (FolderItem* parent, FileSystem* system = 0);
    virtual ~FileItem();

    FileSystem* m_system;
    FolderItem* m_parent;
    int         m_type;
};

class FolderItem : public FileItem
{
public:
    virtual bool        isFolder        ();

    virtual int         permissions     ();

    virtual FileItems   enumItems       ();

    virtual FileItem*   findItem        (const string& name);
    
    virtual FileItem*   createFile      (const string& name);

    virtual FolderItem* createFolder    (const string& name);

    virtual void        moveHere        (FileItem* item, FileCopyHandler* callback = 0, FileCopyContext* context = 0);

    virtual void        copyHere        (FileItem* item, FileCopyHandler* callback = 0, FileCopyContext* context = 0);

protected:
    FolderItem (FolderItem* parent, FileSystem* system = 0);
    virtual ~FolderItem () {}
};

class FileSystem
{
public:
    virtual void        connect    ()   {}

    virtual void        disconnect ()   {}

    virtual FolderItem* rootFolder ()   { return 0; }
};

//////////////////////////////////////////////////////////////////////////

class LocalFileItem : public FileItem
{
public:
    LocalFileItem(const string& path, FolderItem* owner = 0);

    virtual string      name        ();

    virtual int64       size        ();

    virtual DateTime    date        ();

    virtual string      path        ();

    virtual int         permissions ();

    virtual bool        isFolder    ();
    
    virtual bool        exists      ();

    virtual bool        remove      ();

    virtual bool        rename      (const string& newName);

    virtual Stream*     open        (int mode, int access);
    
protected:
    string  m_path;
};

class LocalFolderItem : public FolderItem 
{
public:
    LocalFolderItem(const string& path, FolderItem* owner = 0);

    virtual string      name        ();

    virtual int64       size        ();

    virtual DateTime    date        ();
    
    virtual string      path        ();

    virtual int         permissions ();

    virtual bool        isFolder    ();

    virtual bool        exists      ();

    virtual bool        remove      ();

    virtual bool        rename      (const string& newName);

    virtual FileItems   enumItems   ();

    virtual FileItem*   findItem    (const string& name);

    virtual FileItem*   createFile  (const string& name);

    virtual FolderItem* createFolder(const string& name);

protected:
    string  m_path;
};

class LocalFileSystem : public FileSystem
{
public:
    static LocalFileSystem* instance     ();

    static FileItem*        findItem     (const string& path, FolderItem* parent = 0);
    static FileItem*        getFile      (const string& path, FolderItem* parent = 0);
    static FolderItem*      getFolder    (const string& path, FolderItem* parent = 0);

    static FileItem*        createFile   (const string& path, FolderItem* parent = 0);
    static FolderItem*      createFolder (const string& path, FolderItem* parent = 0);

    virtual FolderItem*     rootFolder   ();

protected:
    static LocalFileSystem* m_instance;
};

END_NAMESPACE_LIB

#endif//FILE_SYSTEM_H
