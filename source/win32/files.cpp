#include "../files.h"
#include "../utils.h"
#include "../reader.h"
#include "../writer.h"
#include "../errors.h"

#include <Windows.h>

#define DS "\\"

BEGIN_NAMESPACE_LIB

void FileStream::init(const char* filename, int mode, int access)
{
    DWORD dwCreationDisposition = CREATE_NEW;
    DWORD dwDesiredAccess = GENERIC_READ | GENERIC_WRITE;
    DWORD dwShareMode = 0;

    if      (mode  ==  FileMode::CreateNew    ) dwCreationDisposition = CREATE_NEW;
    else if (mode  ==  FileMode::Create       ) dwCreationDisposition = CREATE_ALWAYS;
    else if (mode  ==  FileMode::OpenOrCreate ) dwCreationDisposition = OPEN_ALWAYS;
    else if (mode  ==  FileMode::Open         ) dwCreationDisposition = OPEN_EXISTING;
    else if (mode  ==  FileMode::Append       ) dwCreationDisposition = OPEN_EXISTING;
    else if (mode  ==  FileMode::Truncate     ) dwCreationDisposition = TRUNCATE_EXISTING;

    if      (access == FileAccess::ReadOnly   ) { dwDesiredAccess = GENERIC_READ; dwShareMode = FILE_SHARE_READ;  }
    else if (access == FileAccess::WriteOnly  ) dwDesiredAccess = GENERIC_WRITE;

    HANDLE h = CreateFileA(filename, dwDesiredAccess, dwShareMode, NULL, dwCreationDisposition, FILE_ATTRIBUTE_NORMAL, NULL);

    if (h == INVALID_HANDLE_VALUE)
    {
        int err = GetLastError();
        throw IOException("File can not be opened");
    }

    if (mode == FileMode::Append) SetFilePointer(h, 0, 0, FILE_END);

    m_handle = h;
}

FileStream::~FileStream ()
{
    try { close(); } catch (...) {}
}

bool FileStream::canRead ()
{
    return hasFlag(m_access, FileAccess::ReadOnly);
}

bool FileStream::canWrite ()
{
    return hasFlag(m_access, FileAccess::WriteOnly);
}

bool FileStream::canSeek ()
{
    return true;
}

int FileStream::read (void* data, int offset, int size)
{
    DWORD bytesRead = 0;
    BOOL success = ReadFile(m_handle, (char*)data + offset, size, &bytesRead, NULL);
    return success ? bytesRead : -1;
}

int FileStream::write (const void* data, int offset, int size)
{
    DWORD bytesWritten = 0;
    BOOL success = WriteFile(m_handle, (const char*)data + offset, size, &bytesWritten, NULL);    
    return success ? bytesWritten : -1;
}

int FileStream::seek (int offset, int origin)
{
    DWORD filePos = SetFilePointer(m_handle, offset, 0, origin);
    return (filePos == INVALID_SET_FILE_POINTER) ? -1 : filePos;
}

int FileStream::position ()
{
    DWORD filePos = SetFilePointer(m_handle, 0, 0, FILE_BEGIN);
    return (filePos == INVALID_SET_FILE_POINTER) ? -1 : filePos;
}

int FileStream::length ()
{
    DWORD fileSize = GetFileSize(m_handle, NULL);
    return (fileSize == INVALID_FILE_SIZE) ? -1 : fileSize;
}

void FileStream::setLength (int value)
{
    DWORD oldPos = SetFilePointer(m_handle, 0, 0, FILE_BEGIN);
    DWORD newPos = SetFilePointer(m_handle, value, 0, FILE_BEGIN);

    SetEndOfFile(m_handle);
    SetFilePointer(m_handle, oldPos, 0, FILE_BEGIN);
}

void FileStream::flush ()
{
    FlushFileBuffers(m_handle);
}

void FileStream::close ()
{
    CloseHandle(m_handle);
    m_handle = INVALID_HANDLE_VALUE;
}

//////////////////////////////////////////////////////////////////////////
FileStream* File::open(const char* path, int mode, int access)
{
    FileStream* file = new FileStream(path, mode, access);
    return file;
}

FileStream* File::create(const char* path)
{
    FileStream* file = new FileStream(path, FileMode::Create, FileAccess::ReadWrite);
    return file;
}

FileStream* File::openWrite(const char* path)
{
    FileStream* file = new FileStream(path, FileMode::OpenOrCreate, FileAccess::ReadWrite);
    return file;
}

FileStream* File::openRead(const char* path)
{
    FileStream* file = new FileStream(path, FileMode::Open, FileAccess::ReadOnly);
    return file;
}

FileStream* File::openAppend(const char* path)
{
    FileStream* file = new FileStream(path, FileMode::Append, FileAccess::ReadWrite);
    return file;
}

//////////////////////////////////////////////////////////////////////////
void File::copy(const char* sourcePath, const char* destPath, bool overwrite)
{
    if (!overwrite && exists(destPath)) return;
    
    FileStream input (sourcePath, FileMode::Open, FileAccess::ReadOnly);
    FileStream output(destPath, FileMode::Create, FileAccess::WriteOnly);

    const int BufSize = 32768;
    byte buffer[BufSize];

    for (;;)
    {
        int bytes = input.read(buffer, 0, BufSize);
        if (bytes < 0) throw IOException();
        if (bytes == 0) break;

        output.write(buffer, 0, bytes);
    }
}

void File::move(const char* sourcePath, const char* destPath, bool overwrite)
{
    if (!overwrite && exists(destPath)) throw IOException("target file already exists");

    if (rename(sourcePath, destPath) < 0) throw IOException("can not delete file");
}

bool File::remove(const char* path)
{
    return DeleteFileA(path);
}

bool File::exists(const char* path)
{
    DWORD attribs = GetFileAttributesA(path);
    return attribs != INVALID_FILE_ATTRIBUTES;
}

int File::length(const char* path)
{
    return 0;
}

DateTime File::modifyTime(const char* path)
{
    throw NotImplementedException();
}

DateTime File::accessTime(const char* path)
{
    throw NotImplementedException();
}

void File::setModifyTime(const char* path, const DateTime& value)
{
    throw NotImplementedException();
}

void File::setAccessTime(const char* path, const DateTime& value)
{
    throw NotImplementedException();
}

int File::totalLines(const char* path)
{
    int lines = 0;

    StreamReader r(path);

    while(!r.eof())
    {
        r.moveToFirstOf("\r\n", false);

        if (r.read() == '\r')
        {
            int c2 = r.read();
            if (c2 != '\n') r.unread(c2);
        }

        lines++;
    }

    return lines;
}

void File::touch(const char* path)
{
}

int File::append(const char* path, const void* data, int size)
{
    FileStream stream(path, FileMode::OpenOrCreate, FileAccess::ReadWrite);
	
    stream.seek(0, SEEK_END);
    stream.writeBytes(data, size);

    return 0;
}

int File::appendLine(const char* path, const void* data, int size)
{
    FileStream stream(path, FileMode::OpenOrCreate, FileAccess::ReadWrite);

    stream.seek(0, SEEK_END);
    stream.writeBytes(data, size);
    stream.writeBytes("\n", 1);

    return 0;
}

string File::readContent(const char* path)
{
    FileStream file(path, FileMode::Open, FileAccess::ReadOnly);
    return readContent(&file);
}

string File::readContent(Stream* stream)
{
    const int BUFSIZE = 32768;
    char buffer[BUFSIZE];

    string content;
    int num;

    while ((num = stream->read(buffer, 0, BUFSIZE)) > 0)
    {
        content.append(buffer, num);
    }

    return content;
}

strings File::readAllLines(const char* path)
{
    strings lines;
    StreamReader r(path);

    while(!r.eof())
    {
        string line = r.readLine();
        lines.push_back(line);
    }

    return lines;
}

void File::writeAllLines(const char* path, const strings& lines)
{
    StreamWriter w(path);
    strings::const_iterator it = lines.begin();

    while (it != lines.end())
    {
        w.writeLine(*it++);
    }
}

void File::writeContent(const char* path, const string& content)
{
    FileStream file(path, FileMode::Create, FileAccess::ReadWrite);
    file.writeBytes(content.c_str(), content.size());
}

void File::writeContent(Stream* stream, const string& content)
{
    stream->writeBytes(content.c_str(), content.size());
}

//////////////////////////////////////////////////////////////////////////

void Directory::create(const char* path, bool recursive)
{
    throw NotImplementedException();
}

void Directory::purge(const char* path)
{

}

strings Directory::getFiles(const char* path, const char* ends, bool trimEnds)
{
    WIN32_FIND_DATAA fd;

    string filePath = path;
    if (!contains(filePath, '*')) filePath = Path::combine(filePath, "*");

    HANDLE hFind = FindFirstFileA(filePath.c_str(), &fd);
    if (hFind == INVALID_HANDLE_VALUE) return strings();

    strings items;

    do 
    {
        if (hasFlag(fd.dwFileAttributes, FILE_ATTRIBUTE_DIRECTORY)) continue;

        string filename = fd.cFileName;

        if (ends == 0 || endWith(filename, ends))
        {
            if (trimEnds) removeEnd(filename, ends);
            items.push_back(Path::combine(path, filename));
        }

    } while (FindNextFileA(hFind, &fd));

    FindClose(hFind);

    std::sort(items.begin(), items.end());
    return items;
}

strings Directory::getDirectories(const char* path)
{
    WIN32_FIND_DATAA fd;

    string filePath = path;
    if (!contains(filePath, '*')) filePath = Path::combine(filePath, "*");

    HANDLE hFind = FindFirstFileA(filePath.c_str(), &fd);
    if (hFind == INVALID_HANDLE_VALUE) return strings();

    strings items;

    do 
    {
        if (hasFlag(fd.dwFileAttributes, FILE_ATTRIBUTE_DIRECTORY))
        {
            string filename = fd.cFileName;
            if (filename == "." || filename == "..") continue;
            items.push_back(Path::combine(path, filename));
        }
    } while (FindNextFileA(hFind, &fd));

    FindClose(hFind);

    std::sort(items.begin(), items.end());
    return items;
}

bool Directory::exists(const char* path)
{
    DWORD attrs = GetFileAttributesA(path);

    if (attrs == INVALID_FILE_ATTRIBUTES) return false;
    return hasFlag(attrs, FILE_ATTRIBUTE_DIRECTORY);
}

void Directory::remove(const char* path)
{
    strings files = getFiles(path);

    for (int n = 0; n < files.size(); n++)
    {
        File::remove(Path::combine(path, files[n]));
    }

    strings dirs = getDirectories(path);
    for (int n = 0; n < dirs.size(); n++)
    {
        Directory::remove(Path::combine(path, dirs[n]));
    }

    throw NotImplementedException();
}

void Directory::move(const char* sourcePath, const char* destPath, bool overwrite)
{
    throw NotImplementedException();
}

void Directory::copy(const char* sourcePath, const char* destPath, bool overwrite)
{

}

string Directory::currentDir()
{
    char buffer[MAX_PATH];
    GetCurrentDirectoryA(MAX_PATH, buffer);
    return string(buffer);
}

void Directory::changeDir(const char* path)
{

}

//////////////////////////////////////////////////////////////////////////
int Path::parseComponents(const char* path, PathComponent* components, int offset)
{
    const char* p = path;
    const char* name = path;
    uint len = 0;
    bool absolute = (offset == 0 && (*p == '/' || *p == '\\'));

    if (absolute)
    {
        components[0].name = 0;
        components[0].length = 0;
        components[0].upper = false;
        offset = 1;
    }
    
    for (; ;)
    {
        if (*p == '/' || *p == '\\' || *p == 0)
        {
            bool dotdot = (len == 2 && name[0] == name[1] && name[1] == '.');
            bool dot    = (len == 1 && name[0] == '.');
            bool empty  = (len == 0);

            if (dot || empty || dotdot && absolute && offset == 1)
            {
                //just skip it
            }
            else if (dotdot && offset && !components[offset - 1].upper)
            {
                offset--;
            }
            else // valid component
            {
                components[offset].upper = dotdot;
                components[offset].name = name;
                components[offset++].length = len;
            }

            if (*p == 0) break;

            len = 0;
            name = ++p;
        }
        else
        {
            len++;
            p++;
        }
    }

    return offset;
}

string Path::componentsToPath(PathComponent* components, int count)
{
    if (count == 1 && components[0].name == 0)
    {
        return string(DS);
    }
    
    string result;

    for (int n = 0; n < count;)
    {
        if (components[n].name) result.append(components[n].name, components[n].length);
        if (++n < count) result.append(DS);
    }

    return result;
}

string Path::combine(const char* path1, const char* path2)
{
    PathComponent comps[512];

    int offset = parseComponents(path1, comps, 0);
    offset = parseComponents(path2, comps, offset);
    return componentsToPath(comps, offset);
}

string Path::combine(const char* path1, const char* path2, const char* path3)
{
    PathComponent comps[512];

    int offset = parseComponents(path1, comps, 0);
    offset = parseComponents(path2, comps, offset);
    offset = parseComponents(path3, comps, offset);
    return componentsToPath(comps, offset);
}

string Path::combine(const char* path1, const char* path2, const char* path3, const char* path4)
{
    PathComponent comps[512];

    int offset = parseComponents(path1, comps, 0);
    offset = parseComponents(path2, comps, offset);
    offset = parseComponents(path3, comps, offset);
    offset = parseComponents(path4, comps, offset);
    return componentsToPath(comps, offset);
}

string Path::toUnix(const string& path)
{
    return replace(path, "\\", "/");
}

string Path::toWindows(const string& path)
{
    return replace(path, "/", "\\");
}

string Path::normalize(const char* path)
{
    PathComponent comps[512];
    int offset = 0;

    if (path && (*path == '/' || *path == '\\'))
    {
        offset = parseComponents(path, comps, 0);
        return componentsToPath(comps, offset);
    }
    else
    {
        string curdir = Directory::currentDir();
        offset = parseComponents(curdir.c_str(), comps, 0);
        offset = parseComponents(path, comps, offset);
        return componentsToPath(comps, offset);
    }
}

//////////////////////////////////////////////////////////////////////////

string Path::getDirName(const char* path)
{
    PathComponent comps[512];

    int count = parseComponents(path, comps, 0);
    return componentsToPath(comps, count - 1);
}

string Path::getExtention(const char* path)
{
    const char* ext = strrchr(path, '.');
    const char* name = strrchr(path, '/');

    return (ext > name ? ext : "");
}

string Path::getExtention(const string& path)
{
    size_t dot = path.find_last_of('.');

    if (dot != string::npos)
    {
        size_t slash = path.find_last_of('/') + 1;
        if (dot > slash) return path.substr(dot);
    }

    return string();
}

string Path::getFileName(const string& path)
{
    size_t from = path.find_last_of('/') + 1;
    return path.substr(from);
}

string Path::getFileNameWithoutExtention(const string& path)
{
    size_t from = path.find_last_of("/\\") + 1;
    size_t to = path.find_last_of('.');
    return path.substr(from, to - from);
}

bool Path::isRooted(const string& path)
{
    if (startWith(path, "/") || startWith(path, "\\")) return true;

#ifdef WIN32
    if (path.length() >= 2 && isalpha(path[0]) && path[1] == ':') return true;
#endif

    return false;
}

string Path::freeDirName(const string& path, const string& name, const char* suffix)
{
    string newDir = Path::combine(path, name);

    if (!Directory::exists(newDir)) return newDir;

    for (int n = 2; n < 999999; n++)
    {
        newDir = Path::combine(path, name + format(suffix, n));
        if (!Directory::exists(newDir)) break;
    }

    return newDir;
}

string Path::freeFileName(const string& path, const string& name, const char* suffix)
{
    string newFile = Path::combine(path, name);

    if (!File::exists(newFile)) return newFile;

    string baseName = getFileNameWithoutExtention(name);
    string extName  = getExtention(name);
    
    for (int n = 2; n < 999999; n++)
    {
        newFile = Path::combine(path, baseName + format(suffix, n) + extName);
        if (!File::exists(newFile)) break;
    }

    return newFile;
}

END_NAMESPACE_LIB
