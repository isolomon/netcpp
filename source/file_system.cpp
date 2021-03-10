#include "file_system.h"
#include "errors.h"
#include "files.h"
#include "utils.h"

BEGIN_NAMESPACE_LIB

FileItem::FileItem(FolderItem* parent, FileSystem* system) : m_parent(parent), m_system(system), m_type(0)
{
    if (parent) parent->addRef();
}

FileItem::~FileItem()
{
    if (m_parent) m_parent->release();
}

string FileItem::name()
{
    return string();
}

int64 FileItem::size()
{
    return -1;
}

DateTime FileItem::date()
{
    return DateTime(0);
}

int FileItem::type()
{
    return m_type;
}

string FileItem::path()
{
    return string();
}

int FileItem::permissions()
{
    return 0;
}

FolderItem* FileItem::parent()
{
    return m_parent;;
}

bool FileItem::isFolder()
{
    return false;
}

bool FileItem::exists()
{
    return true;
}

FileSystem* FileItem::fileSystem()
{
    return m_system;
}

bool FileItem::remove()
{
    throw NotSupportedException();
}

bool FileItem::rename(const string& newName)
{
    throw NotSupportedException();
}

Stream* FileItem::open(int mode, int access)
{
    throw NotSupportedException();
}

void FileItem::setType(int value)
{
    m_type = value;
}

//////////////////////////////////////////////////////////////////////////
FolderItem::FolderItem(FolderItem* parent, FileSystem* system) : FileItem(parent, system)
{
}

bool FolderItem::isFolder()
{
    return true;
}

int FolderItem::permissions()
{
    return AllowList;
}

FileItems FolderItem::enumItems()
{
    return FileItems();
}

FileItem* FolderItem::findItem(const string& name)
{
    return 0;
}

FileItem* FolderItem::createFile(const string& name)
{
    throw NotSupportedException();
}

FolderItem* FolderItem::createFolder(const string& name)
{
    throw NotSupportedException();
}

void FolderItem::moveHere(FileItem* item, FileCopyHandler* callback, FileCopyContext* context)
{
    throw NotSupportedException();
}

void FolderItem::copyHere(FileItem* item, FileCopyHandler* callback, FileCopyContext* context)
{
    if (item->isFolder())
    {
        FolderItem* source = (FolderItem*)item;
        FolderItem* target = 0;
        FileItems   sourceItems = source->enumItems();

        try
        {
            target = createFolder(source->name());
            if (target == 0) return;

            for (FileItems::iterator it = sourceItems.begin(); it != sourceItems.end(); ++it)
            {
                target->copyHere(*it, callback, context);
            }
        }
        catch (...)
        {
        }

        releaseItems(sourceItems);

        if (target) target->release();
    }
    else
    {
        FileItem* dest = findItem(item->name());

        string source = item->path();
        string target = path() + "/" + item->name();

        if (context)
        {
            context->sourceFile = source.c_str();
            context->targetFile = target.c_str();

            context->exists = dest;

            if (context->overwrite != OverwriteCtrl::YesToAll &&
                context->overwrite != OverwriteCtrl::NoToAll)
            {
                context->overwrite = OverwriteCtrl::Unkown;
            }

            if (callback) callback->invoke(context);

            if (dest && context->skipExists()) return;
        }

        if (dest == 0) dest = createFile(item->name());
        
        AutoPtr<Stream> input  (item->open(FileMode::Open, FileAccess::ReadOnly)    );
        AutoPtr<Stream> output (dest->open(FileMode::Create, FileAccess::WriteOnly) );
                        
        const int BUFSIZE = 32768;
        char buffer[BUFSIZE];

        for (;;)
        {
            int num = input->read(buffer, 0, BUFSIZE);

            if (num == 0) break;
            if (num < 0) throw IOException();

            output->writeBytes(buffer, num);

            if (context) context->bytesCopied += num;

            if (callback)
            {
                callback->invoke(context);

                if (context && context->cancel) throw AbortedException();
            }
        }

        if (context) context->filesCopied++;
    }
}

//////////////////////////////////////////////////////////////////////////

LocalFileItem::LocalFileItem(const string& path, FolderItem* owner) : FileItem(owner, LocalFileSystem::instance()), m_path(path)
{
}

string LocalFileItem::name()
{
    return Path::getFileName(m_path);
}

string LocalFileItem::path()
{
    return m_path;
}

DateTime LocalFileItem::date()
{
    return File::modifyTime(m_path);
}

int64 LocalFileItem::size()
{
    return File::length(m_path);
}

int LocalFileItem::permissions()
{
    return AllowRead | AllowWrite | AllowRemove | AllowRename;
}

bool LocalFileItem::isFolder()
{
    return false;
}

bool LocalFileItem::exists()
{
    return File::exists(m_path);
}

bool LocalFileItem::rename(const string& newName)
{
    string newPath = Path::combine(m_path, "..", newName);

    try
    {
        File::move(m_path, newPath, true);
        m_path = newPath;

        return true;
    }
    catch (...)
    {
        return false;
    }
}

bool LocalFileItem::remove()
{
    return File::remove(m_path);
}

Stream* LocalFileItem::open(int mode, int access)
{
    return File::open(m_path, mode, access);
}

//////////////////////////////////////////////////////////////////////////
LocalFolderItem::LocalFolderItem(const string& path, FolderItem* owner): FolderItem(owner, LocalFileSystem::instance()), m_path(path)
{
}

string LocalFolderItem::name()
{
    return Path::getFileName(m_path);
}

int64 LocalFolderItem::size()
{
    return -1;
}

DateTime LocalFolderItem::date()
{
    return File::modifyTime(m_path);
}

string LocalFolderItem::path()
{
    return m_path;
}

int LocalFolderItem::permissions()
{
    return AllowList | AllowRemove | AllowRename;
}

bool LocalFolderItem::isFolder()
{
    return true;
}

bool LocalFolderItem::exists()
{
    return Directory::exists(m_path);
}

bool LocalFolderItem::rename(const string& newName)
{
    string newPath = Path::combine(m_path, "..", newName);

    try
    {
        File::move(m_path, newPath, true);
        m_path = newPath;

        return true;
    }
    catch (...)
    {
        return false;
    }
}

bool LocalFolderItem::remove()
{
    try
    {
        Directory::remove(m_path);
        return true;
    }
    catch (...)
    {
        return false;
    }
}

FileItems LocalFolderItem::enumItems()
{
    FileItems result;

    try
    {
        strings dirs = Directory::getDirectories(m_path);
        for (int n = 0; n < dirs.size(); n++)
        {
            string path = Path::combine(m_path, dirs[n]);
            result.push_back(new LocalFolderItem(path, this));
        }

        strings files = Directory::getFiles(m_path);
        for (int n = 0; n < files.size(); n++)
        {
            string path = Path::combine(m_path, files[n]);
            result.push_back(new LocalFileItem(path, this));
        }
    }
    catch (...)
    {
    }

    return result;
}

FileItem* LocalFolderItem::findItem(const string& name)
{
    string path = Path::combine(m_path, name);

    if (File::exists(path)) return new LocalFileItem(path, this);
    if (Directory::exists(path)) return new LocalFolderItem(path, this);

    return 0;
}

FolderItem* LocalFolderItem::createFolder(const string& name)
{
    string subPath = Path::combine(m_path, name);
    return LocalFileSystem::createFolder(subPath);
}

FileItem* LocalFolderItem::createFile(const string& name)
{
    string subPath = Path::combine(m_path, name);
    return LocalFileSystem::createFile(subPath);
}

//////////////////////////////////////////////////////////////////////////
FileItem* LocalFileSystem::findItem(const string& path, FolderItem* parent)
{
    if (File::exists(path)) return new LocalFileItem(path, parent);
    if (Directory::exists(path)) return new LocalFolderItem(path, parent);
    return 0;
}

FileItem* LocalFileSystem::getFile(const string& path, FolderItem* parent)
{
   return File::exists(path) ? new LocalFileItem(path, parent) : 0;
}

FolderItem* LocalFileSystem::getFolder(const string& path, FolderItem* parent)
{
    return Directory::exists(path) ? new LocalFolderItem(path, parent) : 0;
}

FileItem* LocalFileSystem::createFile(const string& path, FolderItem* parent)
{
    try { File::touch(path); return new LocalFileItem(path, parent); }
    catch (...) { return 0; }
}

FolderItem* LocalFileSystem::createFolder(const string& path, FolderItem* parent)
{
    try { Directory::create(path); return new LocalFolderItem(path, parent); }
    catch (...) { return 0; }
}

FolderItem* LocalFileSystem::rootFolder()
{
    return new LocalFolderItem("/", 0);
}

LocalFileSystem* LocalFileSystem::instance()
{
    if (m_instance == 0)
    {
        m_instance = new LocalFileSystem();
    }

    return m_instance;
}

LocalFileSystem* LocalFileSystem::m_instance = 0;

END_NAMESPACE_LIB