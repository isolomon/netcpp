#include "files.h"

#include "reader.h"
#include "writer.h"
#include "utils.h"
#include "errors.h"

#include <fcntl.h>
#include <unistd.h>
#include <dirent.h>
#include <stdio.h>
#include <sys/stat.h>
#include <utime.h>
#include <errno.h>

#define DS "/"

BEGIN_NAMESPACE_LIB

void FileStream::init(const char* filename, int mode, int access)
{
    int flags = O_NOCTTY;

    if      (mode  ==  FileMode::Create       ) flags |= O_CREAT | O_TRUNC;
    else if (mode  ==  FileMode::CreateNew    ) flags |= O_CREAT | O_EXCL;    
    else if (mode  ==  FileMode::OpenOrCreate ) flags |= O_CREAT;
    else if (mode  ==  FileMode::Open         ) flags |= 0;
    else if (mode  ==  FileMode::Append       ) flags |= O_APPEND;
    else if (mode  ==  FileMode::Truncate     ) flags |= O_TRUNC;

    if      (access == FileAccess::ReadOnly   ) flags |= O_RDONLY;
    else if (access == FileAccess::WriteOnly  ) flags |= O_WRONLY;
    else flags |= O_RDWR;

    int perms = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH; // rw-r--r--

    m_handle = ::open(filename, flags, perms);

    if (m_handle < 0) throw IOException("File could not be opened");
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
    if (size <= 0) return 0;

    int bytesRead = ::read(m_handle, (char*)data + offset, size);

    if (bytesRead < 0) throw IOException();

    return bytesRead;
}

int FileStream::write (const void* data, int offset, int size)
{
    if (size <= 0) return 0;

    int written = ::write(m_handle, (const char*)data + offset, size);

    if (written < 0) throw IOException();

    return written;
}

int FileStream::seek (int offset, int origin)
{
    int lowOrigin = SEEK_SET;

    if (origin == SeekCurrent)  lowOrigin = SEEK_CUR;
    else if (origin == SeekEnd) lowOrigin = SEEK_END;

    return lseek(m_handle, offset, lowOrigin);
}

int FileStream::position ()
{
    return lseek(m_handle, 0, SEEK_CUR);
}

int FileStream::length ()
{
    int current = lseek(m_handle, 0, SEEK_CUR);
    int length = lseek(m_handle, (size_t)0, SEEK_END);
    lseek(m_handle, current, SEEK_SET);

    return length;
}

void FileStream::setLength (int value)
{
    ftruncate(m_handle, value);
}

void FileStream::flush ()
{
    ::fdatasync(m_handle);
}

void FileStream::close ()
{
    int fd = m_handle;
    m_handle = 0;

    if (fd) ::close(fd);
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
    FileStream* file = new FileStream(path, FileMode::OpenOrCreate, FileAccess::ReadWrite);
    file->seek(0, SeekEnd);
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
    return (::remove(path) == 0);
}

bool File::exists(const char* path)
{
    struct stat st;
    if (::stat(path, &st) < 0) return false;
    return S_ISREG(st.st_mode);
}

int File::length(const char* path)
{
    struct stat st;
    if (::stat(path, &st) < 0) throw IOException("can not get file info");
    return st.st_size;
}

DateTime File::modifyTime(const char* path)
{
    struct stat buf;
    int code = stat(path, &buf);
    if (code < 0) throw IOException();

    return DateTime(buf.st_mtime);
}

DateTime File::accessTime(const char* path)
{
    struct stat buf;
    int code = stat(path, &buf);
    if (code < 0) throw IOException();

    return DateTime(buf.st_atime);
}

void File::setModifyTime(const char* path, const DateTime& value)
{
    struct stat buf;
    int code = stat(path, &buf);
    if (code < 0) throw IOException();

    utimbuf ubuf = { buf.st_atime, value.epochSeconds() };
    code = utime (path, &ubuf);
    if (code < 0) throw IOException();
}

void File::setAccessTime(const char* path, const DateTime& value)
{
    struct stat buf;
    int code = stat(path, &buf);
    if (code < 0) throw IOException();

    utimbuf ubuf = { value.epochSeconds(), buf.st_mtime};
    code = utime(path, &ubuf);
    if (code < 0) throw IOException();
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
    int perms = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH; // rw-r--r--

    int fd = ::open(path, O_CREAT, perms);

    if (fd < 0 && errno == ENOENT)
    {
        string dir = Path::getDirName(path);
        Directory::create(dir);
        fd = ::open(path, O_CREAT, perms);
    }

    if (fd < 0) throw IOException("Cannot touch file");
    else ::close(fd);
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

    if (stream->canSeek())
    {
        int len = stream->length();
        content.reserve(len > 0 ? len + 64: 0);
    }

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
    if (exists(path)) return;

    int code = mkdir(path, S_IRWXU | S_IRWXG |S_IRWXO);
    if (code == 0) return;

    if (recursive)
    {
        create(Path::getDirName(path), recursive);
        code = mkdir(path, S_IRWXU | S_IRWXG |S_IRWXO);
    }

    if (code) throw IOException("Unable to create directory");
}

void Directory::purge(const char* path)
{
    throw NotImplementedException();
}

strings Directory::getFiles(const char* path, const char* ends, bool trimEnds)
{
    strings items;

    DIR* dir = opendir(path);

    if (dir == 0)
    {
        if (errno == ENOENT) return items;
        throw IOException();
    }

    while (dirent* ent = readdir(dir))
    {
        if (ent->d_type & DT_REG)
        {
            string filename = ent->d_name;

            if (ends == 0 || endWith(filename, ends))
            {
                if (trimEnds) removeEnd(filename, ends);
                items.push_back(filename);
            }
        }
    }

    closedir(dir);

    std::sort(items.begin(), items.end());
    return items;
}

strings Directory::getDirectories(const char* path)
{
    strings items;

    DIR* dir = opendir(path);

    if (dir == 0)
    {
        if (errno == ENOENT) return items;
        throw FileNotFoundException();
    }

    while (dirent* ent = readdir(dir))
    {
        if (ent->d_type & DT_DIR)
        {
            if (strcmp(ent->d_name, ".") == 0 || strcmp(ent->d_name, "..") == 0) continue;
            items.push_back(ent->d_name);
        }
    }

    closedir(dir);

    std::sort(items.begin(), items.end());
    return items;
}

bool Directory::exists(const char* path)
{
    struct stat st;
    if (::stat(path, &st) < 0) return false;
    return S_ISDIR(st.st_mode);
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

    if (rmdir(path) < 0) throw IOException("Can not delete dir");
}

void Directory::move(const char* sourcePath, const char* destPath, bool overwrite)
{
    if (!overwrite && exists(destPath)) throw IOException("target file already exists");

    if (rename(sourcePath, destPath) < 0) throw IOException("can not rename file");
}

void Directory::copy(const char* sourcePath, const char* destPath, bool overwrite)
{

}

string Directory::currentDir()
{
    char buffer[PATH_MAX];
    char* result = getcwd(buffer, PATH_MAX);
    return string(buffer);
}

void Directory::changeDir(const char* path)
{
    if (chdir(path) < 0) throw IOException("Unable to change directory");
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
