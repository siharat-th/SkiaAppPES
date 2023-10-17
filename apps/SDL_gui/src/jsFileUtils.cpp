#include "jsFileUtils.h"


#ifdef __ANDROID__
#include <jni.h>
#include <android/log.h>
#include <android/asset_manager.h>
#include <android/asset_manager_jni.h>

extern DECLSPEC void * SDLCALL SDL_AndroidGetJNIEnv();
extern "C" jclass Android_JNI_GetActivityClass(void);

extern AAssetManager* pAssetManager;

extern char __directoryList[512][128];
extern int __numFilesInDirectory;
#endif

#ifndef _WIN32
#include <pwd.h>
#include <sys/stat.h>
#else
#if (_MSC_VER) // microsoft visual studio
#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif
#ifndef _WINSOCK_DEPRECATED_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#endif
#ifndef UNICODE
#define UNICODE
#endif
#include <stdint.h>
#include <functional>

#define _WINSOCKAPI_  // Tooh - stops Windows.h including winsock.h to prevent macro redefinition in SDL_net.h 
#include <Windows.h>
#endif
#endif

#if defined( __APPLE_CC__)
#define __ASSERT_MACROS_DEFINE_VERSIONS_WITHOUT_UNDERSCORES 0
#include <TargetConditionals.h>
#if TARGET_OS_IPHONE || TARGET_IPHONE
#include <unistd.h>
#include <sys/time.h>
#endif
#endif

#ifdef __MACOSX__
#include <mach-o/dyld.h>       /* _NSGetExecutablePath */
#include <limits.h>        /* PATH_MAX */
#endif

namespace {
    bool enableDataPath = true;

    //--------------------------------------------------
    string defaultDataPath() {
#if defined __MACOSX__
        //return SDL_GetBasePath() + GUI_GetResourcePath();
        return string("data/");
#elif defined WIN32
        return string("data/");
#elif defined __ANDROID__
        return string("data/");
#elif defined __linux__        
        return string("data/");
#else
        //try {
            return std::filesystem::canonical(jsFilePath::join(jsFilePath::getCurrentExeDir(), "data/")).string();
        //} catch (...) {
        //    return jsFilePath::join(jsFilePath::getCurrentExeDir(), "data/");
        //}
#endif
    }

    //--------------------------------------------------
    std::filesystem::path & defaultWorkingDirectory() {
#ifdef WIN32
        static std::filesystem::path defaultWorkingDirectory = std::filesystem::absolute(std::filesystem::current_path());
        return defaultWorkingDirectory;
#else
        static auto * defaultWorkingDirectory = new std::filesystem::path();
        return *defaultWorkingDirectory;
#endif
    }

    //--------------------------------------------------
    std::filesystem::path & dataPathRoot() {
        static auto * dataPathRoot = new std::filesystem::path(defaultDataPath());
        return *dataPathRoot;
    }
}

template<class T>
void jsSort(vector<T>& values) {
    sort(values.begin(), values.end());
}

template<class T, class BoolFunction>
void jsSort(vector<T>& values, BoolFunction compare) {
    sort(values.begin(), values.end(), compare);
}

//--------------------------------------------------
void jsInitFileUtils() {
    defaultWorkingDirectory() = std::filesystem::absolute(std::filesystem::current_path());
}

//--------------------------------------------------
void jsEnableDataPath() {
    enableDataPath = true;
}

//--------------------------------------------------
void jsDisableDataPath() {
    enableDataPath = false;
}

//--------------------------------------------------
bool jsIsEnableDataPath() {
    return enableDataPath;
}

//--------------------------------------------------
bool jsRestoreWorkingDirectoryToDefault() {
//    try {
        std::filesystem::current_path(defaultWorkingDirectory());
        return true;
//    } catch (...) {
//        return false;
//    }
}

//--------------------------------------------------
void jsSetDataPathRoot(const string& newRoot) {
    dataPathRoot() = newRoot;
}

//--------------------------------------------------
string jsToDataPath(const string& path, bool makeAbsolute) {
    if (!enableDataPath)
        return path;

#ifdef __ANDROID__
    return path;
#endif

    bool hasTrailingSlash = !path.empty() && std::filesystem::path(path).generic_string().back() == '/';



    // if our Current Working Directory has changed (e.g. file open dialog)
#ifdef _WIN32
    if (defaultWorkingDirectory() != std::filesystem::current_path()) {
        // change our cwd back to where it was on app load
        bool ret = jsRestoreWorkingDirectoryToDefault();
        if (!ret) {
            GUI_Log("jsToDataPath: error while trying to change back to default working directory %s\n", defaultWorkingDirectory().c_str());
        }
    }
#endif

    // this could be performed here, or wherever we might think we accidentally change the cwd, e.g. after file dialogs on windows
    const auto  & dataPath = dataPathRoot();
    std::filesystem::path inputPath(path);
    std::filesystem::path outputPath;

    // if path is already absolute, just return it
    if (inputPath.is_absolute()) {
        if( std::filesystem::exists( inputPath ) == false ) { // Jimmy *** canonical will except if path not exists
            return inputPath.string();
        }
//        try {
            auto outpath = std::filesystem::canonical(inputPath);
#if defined(_MSC_VER) || defined(__linux__)
            // msc: outpath no-trailingslash after call canonical()
            if (std::filesystem::is_directory(outpath) && hasTrailingSlash) {  // noom win require ***
#else
            if (std::filesystem::is_directory(outpath) && !hasTrailingSlash) {  // jimmy ***
#endif
                return jsFilePath::addTrailingSlash(outpath.string());
            } else {
                return outpath.string();
            }
//        } catch (...) {
//            return inputPath.string();
//        }
    }

    // here we check whether path already refers to the data folder by looking for common elements
    // if the path begins with the full contents of dataPathRoot then the data path has already been added
    // we compare inputPath.toString() rather that the input var path to ensure common formatting against dataPath.toString()
    auto dirDataPath = dataPath.string();

    // also, we strip the trailing slash from dataPath since `path` may be input as a file formatted path even if it is a folder (i.e. missing trailing slash)
    dirDataPath = jsFilePath::addTrailingSlash(dirDataPath);

    auto relativeDirDataPath = jsFilePath::makeRelative(std::filesystem::current_path().string(), dataPath.string());
    relativeDirDataPath = jsFilePath::addTrailingSlash(relativeDirDataPath);

    if (inputPath.string().find(dirDataPath) != 0 && inputPath.string().find(relativeDirDataPath) != 0) {
        // inputPath doesn't contain data path already, so we build the output path as the inputPath relative to the dataPath
        if (makeAbsolute) {
            outputPath = dirDataPath / inputPath;
        } else {
            outputPath = relativeDirDataPath / inputPath;
        }
    } else {
        // inputPath already contains data path, so no need to change
        outputPath = inputPath;
    }

    // finally, if we do want an absolute path and we don't already have one
    if (makeAbsolute) {
        // then we return the absolute form of the path
//        try {
            auto outpath = std::filesystem::canonical(std::filesystem::absolute(outputPath));

            if (std::filesystem::is_directory(outpath) && hasTrailingSlash) {
                return jsFilePath::addTrailingSlash(outpath.string());
            } else {
                return outpath.string();
            }
//        } catch (std::exception &) {
//            return std::filesystem::absolute(outputPath).string();
//        }
    } else {
        // or output the relative path
        return outputPath.string();
    }
}

//------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------
// -- jsBuffer
//------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------

//--------------------------------------------------
jsBuffer::jsBuffer()
    :currentLine(end(), end()) {
}

//--------------------------------------------------
jsBuffer::jsBuffer(const char * _buffer, std::size_t size)
    : buffer(_buffer, _buffer + size)
    , currentLine(end(), end()) {
}

//--------------------------------------------------
jsBuffer::jsBuffer(const string & text)
    : buffer(text.begin(), text.end())
    , currentLine(end(), end()) {
}

//--------------------------------------------------
jsBuffer::jsBuffer(istream & stream, size_t ioBlockSize)
    : currentLine(end(), end()) {
    set(stream, ioBlockSize);
}

//--------------------------------------------------
bool jsBuffer::set(istream & stream, size_t ioBlockSize) {
    if (stream.bad()) {
        clear();
        return false;
    } else {
        buffer.clear();
    }

    vector<char> aux_buffer(ioBlockSize);
    while (stream.good()) {
        stream.read(&aux_buffer[0], ioBlockSize);
        append(aux_buffer.data(), (size_t)stream.gcount());
    }
    return true;
}

//--------------------------------------------------
void jsBuffer::setall(char mem) {
    buffer.assign(buffer.size(), mem);
}

//--------------------------------------------------
bool jsBuffer::writeTo(ostream & stream) const {
    if (stream.bad()) {
        return false;
    }
    stream.write(buffer.data(), buffer.size());
    return stream.good();
}

//--------------------------------------------------
void jsBuffer::set(const char * _buffer, std::size_t _size) {
    buffer.assign(_buffer, _buffer + _size);
}

//--------------------------------------------------
void jsBuffer::set(const string & text) {
    set(text.c_str(), text.size());
}

//--------------------------------------------------
void jsBuffer::append(const string& _buffer) {
    append(_buffer.c_str(), _buffer.size());
}

//--------------------------------------------------
void jsBuffer::append(const char * _buffer, std::size_t _size) {
    buffer.insert(buffer.end(), _buffer, _buffer + _size);
}

void jsBuffer::append(int _ivalue) {
    const char* _buffer = (const char*)&_ivalue;
    append(_buffer, 4);
}

void jsBuffer::append(unsigned int _ivalue) {
    const char* _buffer = (const char*)&_ivalue;
    append(_buffer, 4);
}

void jsBuffer::append(short _svalue) {
    const char* _buffer = (const char*)&_svalue;
    append(_buffer, 2);
}

void jsBuffer::append(unsigned short _svalue) {
    const char* _buffer = (const char*)&_svalue;
    append(_buffer, 2);
}

void jsBuffer::append(float _fvalue) {
    union {
        float f32;
        unsigned int u32;
    } float_int_u;
    float_int_u.f32 = _fvalue;
    append(float_int_u.u32);
}

//--------------------------------------------------
void jsBuffer::appendU32(uint32_t u32) {
    uint8_t buf[4];
    buf[0] = u32 & 0xFF;
    buf[1] = (u32 & 0xFF00) >> 8;
    buf[2] = (u32 & 0xFF0000) >> 16;
    buf[3] = (u32 & 0xFF000000) >> 24;

    append((const char *)buf, 4);
}

void jsBuffer::appendU16(uint16_t u16) {
    uint8_t buf[2];
    buf[0] = u16 & 0xFF;
    buf[1] = (u16 & 0xFF00) >> 8;

    append((const char *)&buf[0], 1);
    append((const char *)&buf[1], 1);
}

void jsBuffer::appendS16(int16_t s16) {
    int16_t buf[2];
    buf[0] = s16 & 0xFF;
    buf[1] = (s16 & 0xFF00) >> 8;

    append((const char *)&buf[0], 1);
    append((const char *)&buf[1], 1);
}

void jsBuffer::appendU8(uint8_t u8) {
    append((const char *)&u8, 1);
}

void jsBuffer::appendU8(uint8_t u8, int num) {
    for (int i = 0; i < num; i++) {
        append((const char *)&u8, 1);
    }
}

void jsBuffer::appendString(const char *str) {
    int len = (int)strlen(str);

    append((const char *)str, len);
}

void jsBuffer::appendPESString(const char *str) {
    unsigned char buf[257];

    int len = (int)strlen(str);
    if (len > 255)
        len = 255;

    buf[0] = len;
    memcpy(&buf[1], str, len);

    append((const char *)buf, len + 1);
}

void jsBuffer::appendPESString2(const char *str) {
    unsigned char buf[258];

    int len = (int)strlen(str);
    if (len > 255)
        len = 255;

    buf[0] = len & 0xFF;
    buf[1] = (len & 0xFF00) >> 8;
    memcpy(&buf[2], str, len);

    append((const char *)buf, len + 2);
}

//--------------------------------------------------
void jsBuffer::reserve(size_t size) {
    buffer.reserve(size);
}

//--------------------------------------------------
void jsBuffer::clear() {
    buffer.clear();
}

//--------------------------------------------------
void jsBuffer::allocate(std::size_t _size) {
    resize(_size);
}

//--------------------------------------------------
void jsBuffer::resize(std::size_t _size) {
    buffer.resize(_size);
}

//--------------------------------------------------
char * jsBuffer::getData() {
    return buffer.data();
}

//--------------------------------------------------
const char * jsBuffer::getData() const {
    return buffer.data();
}

//--------------------------------------------------
string jsBuffer::getText() const {
    if (buffer.empty()) {
        return "";
    }
    return std::string(buffer.begin(), buffer.end());
}

//--------------------------------------------------
jsBuffer::operator string() const {
    return getText();
}

//--------------------------------------------------
jsBuffer & jsBuffer::operator=(const string & text) {
    set(text);
    return *this;
}

//--------------------------------------------------
std::size_t jsBuffer::size() const {
    return buffer.size();
}

//--------------------------------------------------
vector<char>::iterator jsBuffer::begin() {
    return buffer.begin();
}

//--------------------------------------------------
vector<char>::iterator jsBuffer::end() {
    return buffer.end();
}

//--------------------------------------------------
vector<char>::const_iterator jsBuffer::begin() const {
    return buffer.begin();
}

//--------------------------------------------------
vector<char>::const_iterator jsBuffer::end() const {
    return buffer.end();
}

//--------------------------------------------------
vector<char>::reverse_iterator jsBuffer::rbegin() {
    return buffer.rbegin();
}

//--------------------------------------------------
vector<char>::reverse_iterator jsBuffer::rend() {
    return buffer.rend();
}

//--------------------------------------------------
vector<char>::const_reverse_iterator jsBuffer::rbegin() const {
    return buffer.rbegin();
}

//--------------------------------------------------
vector<char>::const_reverse_iterator jsBuffer::rend() const {
    return buffer.rend();
}

//--------------------------------------------------
jsBuffer::Line::Line(vector<char>::iterator _begin, vector<char>::iterator _end)
    :_current(_begin)
    , _begin(_begin)
    , _end(_end) {
    if (_begin == _end) {
        line = "";
        return;
    }

    bool lineEndWasCR = false;

    while (_current != _end && *_current != '\n') {
        if (*_current == '\r') {
            lineEndWasCR = true;
            break;
        } else if (*_current == 0 && _current + 1 == _end) {
            break;
        } else {
            _current++;
        }
    }

    line = string(_begin, _current);

    if (_current != _end) {
        _current++;
    }

    if (lineEndWasCR && _current != _end && *_current == '\n') {
        _current++;
    }
}

//--------------------------------------------------
const string & jsBuffer::Line::operator*() const {
    return line;
}

//--------------------------------------------------
const string * jsBuffer::Line::operator->() const {
    return &line;
}

//--------------------------------------------------
const string & jsBuffer::Line::asString() const {
    return line;
}

//--------------------------------------------------
jsBuffer::Line & jsBuffer::Line::operator++() {
    *this = Line(_current, _end);
    return *this;
}

//--------------------------------------------------
jsBuffer::Line jsBuffer::Line::operator++(int) {
    Line tmp(*this);
    operator++();
    return tmp;
}

//--------------------------------------------------
bool jsBuffer::Line::operator!=(Line const& rhs) const {
    return rhs._begin != _begin || rhs._end != _end;
}

//--------------------------------------------------
bool jsBuffer::Line::operator==(Line const& rhs) const {
    return rhs._begin == _begin && rhs._end == _end;
}

bool jsBuffer::Line::empty() const {
    return _begin == _end;
}

//--------------------------------------------------
jsBuffer::Lines::Lines(vector<char>::iterator begin, vector<char>::iterator end)
    :_begin(begin)
    , _end(end) {
}

//--------------------------------------------------
jsBuffer::Line jsBuffer::Lines::begin() {
    return Line(_begin, _end);
}

//--------------------------------------------------
jsBuffer::Line jsBuffer::Lines::end() {
    return Line(_end, _end);
}

//--------------------------------------------------
jsBuffer::Lines jsBuffer::getLines() {
    return jsBuffer::Lines(begin(), end());
}

//--------------------------------------------------
ostream & operator<<(ostream & ostr, const jsBuffer & buf) {
    buf.writeTo(ostr);
    return ostr;
}

//--------------------------------------------------
istream & operator>>(istream & istr, jsBuffer & buf) {
    buf.set(istr);
    return istr;
}

//--------------------------------------------------
jsBuffer jsBufferFromFile(const string & path, bool binary) {
    // Jimmy - to do *Android* Assets load

#ifdef __ANDROID__
    
    jsFile f(path, jsFile::ReadOnly, binary);
    return jsBuffer(f);

#else

    jsFile f(path, jsFile::ReadOnly, binary);
    return jsBuffer(f);
#endif
}

//--------------------------------------------------
bool jsBufferToFile(const string & path, jsBuffer & buffer, bool binary) {
    jsFile f(path, jsFile::WriteOnly, binary);
    return buffer.writeTo(f);
}

//------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------
// -- jsFile
//------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------

//------------------------------------------------------------------------------------------------------------
jsFile::jsFile()
    :mode(Reference)
    , binary(true) {
}

jsFile::jsFile(const std::filesystem::path & path, Mode mode, bool binary)
    : mode(mode)
    , binary(true) {
    open(path, mode, binary);
}

jsFile::jsFile(std::string path, Mode mode, bool binary)
    : mode(mode)
    , binary(true) {
    jsFileOpen(path, mode, binary);
}

bool jsFile::jsFileOpen(const std::filesystem::path & path, Mode mode, bool binary) {
    return open(path, mode, binary);
}

//-------------------------------------------------------------------------------------------------------------
jsFile::~jsFile() {
    //close();
}

//-------------------------------------------------------------------------------------------------------------
jsFile::jsFile(const jsFile & mom)
    :basic_ios()
    , fstream()
    , mode(Reference)
    , binary(true) {
    copyFrom(mom);
}

//-------------------------------------------------------------------------------------------------------------
jsFile & jsFile::operator=(const jsFile & mom) {
    copyFrom(mom);
    return *this;
}

//-------------------------------------------------------------------------------------------------------------
void jsFile::copyFrom(const jsFile & mom) {
    if (&mom != this) {
        Mode new_mode = mom.mode;

        if (new_mode != Reference && new_mode != ReadOnly) {
            new_mode = ReadOnly;
            GUI_Log("sfFile::copyFrom(): copying a writable file, opening new copy as read only\n");
        }

        open(mom.myFile.string(), new_mode, mom.binary);
    }
}

//------------------------------------------------------------------------------------------------------------
bool jsFile::openStream(Mode _mode, bool _binary) {
    mode = _mode;
    binary = _binary;
    ios_base::openmode binary_mode = binary ? ios::binary : (ios_base::openmode)0;

    switch (_mode) {
        case WriteOnly:
        case ReadWrite:
        case Append:
            if (!jsDirectory(jsFilePath::getEnclosingDirectory(path())).exists()) {
                jsFilePath::createEnclosingDirectory(path());
            }
            break;

        case Reference:
        case ReadOnly:
            break;
    }

    switch (_mode) {
        case Reference:
            return true;
            break;

        case ReadOnly:
            if (exists() && isFile()) {
                fstream::open(path().c_str(), ios::in | binary_mode);
            }
            break;

        case WriteOnly:
            fstream::open(path().c_str(), ios::out | binary_mode);
            break;

        case ReadWrite:
            fstream::open(path().c_str(), ios_base::in | ios_base::out | binary_mode);
            break;

        case Append:
            fstream::open(path().c_str(), ios::out | ios::app | binary_mode);
            break;
    }

    return fstream::good();
}

//------------------------------------------------------------------------------------------------------------
bool jsFile::open(const std::filesystem::path & _path, Mode _mode, bool binary) {
    close();

    myFile = std::filesystem::path(jsToDataPath(_path.string()));
    return openStream(_mode, binary);
}

//-------------------------------------------------------------------------------------------------------------
bool jsFile::changeMode(Mode _mode, bool binary) {
    if (_mode != mode) {
        string _path = path();
        close();
        myFile = std::filesystem::path(_path);
        return openStream(_mode, binary);
    } else {
        return true;
    }
}

//-------------------------------------------------------------------------------------------------------------
bool jsFile::isWriteMode() {
    return mode != ReadOnly;
}

//-------------------------------------------------------------------------------------------------------------
void jsFile::close() {
    myFile = std::filesystem::path();
    if (mode != Reference) fstream::close();
}

//------------------------------------------------------------------------------------------------------------
bool jsFile::create() {
    bool success = false;

    if (!myFile.string().empty()) {
        auto oldmode = this->mode;
        auto oldpath = path();
        success = open(path(), jsFile::WriteOnly, binary);
        close();
        open(oldpath, oldmode, binary);
    }

    return success;
}

//------------------------------------------------------------------------------------------------------------
jsBuffer jsFile::readToBuffer() {
    if (myFile.string().empty() || !std::filesystem::exists(myFile)) {
        return jsBuffer();
    }

    return jsBuffer(*this);
}

//------------------------------------------------------------------------------------------------------------
bool jsFile::writeFromBuffer(const jsBuffer & buffer) {
    if (myFile.string().empty()) {
        return false;
    }

    if (!isWriteMode()) {
        GUI_Log("jsFile::writeFromBuffer() : trying to write to read only file %s\n", myFile.string().c_str());
    }

    return buffer.writeTo(*this);
}

//------------------------------------------------------------------------------------------------------------
filebuf *jsFile::getFileBuffer() const {
    return rdbuf();
}

//------------------------------------------------------------------------------------------------------------
bool jsFile::exists() const {
    if (path().empty()) {
        return false;
    }

    return std::filesystem::exists(myFile);
}

//------------------------------------------------------------------------------------------------------------
string jsFile::path() const {
    return myFile.string();
}

//------------------------------------------------------------------------------------------------------------
string jsFile::getExtension() const {
    auto dotext = myFile.extension().string();

    if (!dotext.empty() && dotext.front() == '.') {
        return std::string(dotext.begin() + 1, dotext.end());
    } else {
        return dotext;
    }
}

//------------------------------------------------------------------------------------------------------------
string jsFile::getFileName() const {
    return myFile.filename().string();
}

//------------------------------------------------------------------------------------------------------------
string jsFile::getBaseName() const {
    return myFile.stem().string();
}

//------------------------------------------------------------------------------------------------------------
string jsFile::getEnclosingDirectory() const {
    return jsFilePath::getEnclosingDirectory(path());
}

//------------------------------------------------------------------------------------------------------------
string jsFile::getAbsolutePath() const {
    return jsFilePath::getAbsolutePath(path());
}

//------------------------------------------------------------------------------------------------------------
bool jsFile::canRead() const {

#ifdef __ANDROID__
    // Jimmy - to do - need proper manage here
    return true;
#endif

    auto perm = std::filesystem::status(myFile).permissions();
#ifdef _WIN32
    DWORD attr = GetFileAttributes(myFile.native().c_str());

    if (attr == INVALID_FILE_ATTRIBUTES) {
        return false;
    }

    return true;
#else
    struct stat info;
    //printf( "CanRead: %s\n", path().c_str());
    /*
    stat(path().c_str(), &info);  // Error check omitted

    if (geteuid() == info.st_uid) {
        return perm & std::filesystem::owner_read;
    } else if (getegid() == info.st_gid) {
        return perm & std::filesystem::group_read;
    } else {
        return perm & std::filesystem::others_read;
    }
    */
    return true;
#endif
}

//------------------------------------------------------------------------------------------------------------
bool jsFile::canWrite() const {
    auto perm = std::filesystem::status(myFile).permissions();
#ifdef _WIN32
    DWORD attr = GetFileAttributes(myFile.native().c_str());

    if (attr == INVALID_FILE_ATTRIBUTES) {
        return false;
    } else {
        return (attr & FILE_ATTRIBUTE_READONLY) == 0;
    }
#else
    struct stat info;
    stat(path().c_str(), &info);  // Error check omitted

//    if (geteuid() == info.st_uid) {
//        return perm & std::filesystem::owner_write;
//    } else if (getegid() == info.st_gid) {
//        return perm & std::filesystem::group_write;
//    } else {
//        return perm & std::filesystem::others_write;
//    }
#endif
}

//------------------------------------------------------------------------------------------------------------
bool jsFile::canExecute() const {
    auto perm = std::filesystem::status(myFile).permissions();
#ifdef _WIN32
    return getExtension() == "exe";
#else
    struct stat info;
    stat(path().c_str(), &info);  // Error check omitted

//    if (geteuid() == info.st_uid) {
//        return perm & std::filesystem::owner_exe;
//    } else if (getegid() == info.st_gid) {
//        return perm & std::filesystem::group_exe;
//    } else {
//        return perm & std::filesystem::others_exe;
//    }
#endif
}

//------------------------------------------------------------------------------------------------------------
bool jsFile::isFile() const {
    bool ret = false;
#ifdef __ANDROID__
    if (myFile.string().find(".") != std::string::npos)
        ret = true;
#else
    ret = std::filesystem::is_regular_file(myFile);
#endif
    return ret;
}

//------------------------------------------------------------------------------------------------------------
bool jsFile::isLink() const {
    return std::filesystem::is_symlink(myFile);
}

//------------------------------------------------------------------------------------------------------------
bool jsFile::isDirectory() const {
#ifdef __ANDROID__
    bool ret = false;

    size_t pos = myFile.string().find_last_of( "/" );
    if (myFile.string().find(".", pos) == std::string::npos)
        ret = true;

    GUI_Log("isDirectory: %s (%i)\n", myFile.c_str(), (int)ret);
#else
    bool ret = std::filesystem::is_directory(myFile);
#endif
    return ret;
}

//------------------------------------------------------------------------------------------------------------
bool jsFile::isDevice() const {
#ifdef TARGET_WIN32
    return false;
#else
//    return std::filesystem::status(myFile).type() == std::filesystem::block_file;
#endif
}

//------------------------------------------------------------------------------------------------------------
bool jsFile::isHidden() const {
#ifdef TARGET_WIN32
    return false;
#else
    return myFile.filename() != "." && myFile.filename() != ".." && myFile.filename().string()[0] == '.';
#endif
}

//------------------------------------------------------------------------------------------------------------
void jsFile::setWriteable(bool flag) {
    setReadOnly(!flag);
}

//------------------------------------------------------------------------------------------------------------
void jsFile::setReadOnly(bool flag) {
//    try {
        if (flag) {
//            std::filesystem::permissions(myFile, std::filesystem::perms::owner_write | std::filesystem::perms::remove_perms);
//            std::filesystem::permissions(myFile, std::filesystem::perms::owner_write | std::filesystem::perms::remove_perms);
//            std::filesystem::permissions(myFile, std::filesystem::perms::owner_write | std::filesystem::perms::remove_perms);
        } else {
//            std::filesystem::permissions(myFile, std::filesystem::perms::owner_write | std::filesystem::perms::add_perms);
        }
//    } catch (std::exception & e) {
//        GUI_Log("Couldn't set write permission on %s:%s\n", myFile.string().c_str(), e.what());
//    }
}

//------------------------------------------------------------------------------------------------------------
void jsFile::setExecutable(bool flag) {
//    try {
//        std::filesystem::permissions(myFile, std::filesystem::perms::owner_exe | std::filesystem::perms::add_perms);
//    } catch (std::exception & e) {
//        //ofLogError() << "Couldn't set write permission on " << myFile << ": " << e.what();
//        GUI_Log("Couldn't set write permission on %s:%s\n", myFile.string().c_str(), e.what());
//    }
}

//------------------------------------------------------------------------------------------------------------
bool jsFile::copyTo(const string& _path, bool bRelativeToData, bool overwrite) const {
    std::string path = _path;

    if (isDirectory()) {
        return jsDirectory(myFile).copyTo(path, bRelativeToData, overwrite);
    }
    if (path.empty()) {
        GUI_Log("jsFile::copyTo(): destination path %s is empty\n", _path.c_str());
        return false;
    }
    if (!exists()) {
        GUI_Log("jsFile::copyTo(): source path %s is does not exist\n", this->path().c_str());
        return false;
    }

    if (bRelativeToData) {
        path = jsToDataPath(path);
    }

    if (jsFile::doesFileExist(path, bRelativeToData)) {
        if (isFile() && jsFile(path, jsFile::Reference).isDirectory()) {
            path = jsFilePath::join(path, getFileName());
        }

        if (jsFile::doesFileExist(path, bRelativeToData)) {
            if (overwrite) {
                jsFile::removeFile(path, bRelativeToData);
            }
        }
    }

//    try {
        if (!jsDirectory(jsFilePath::getEnclosingDirectory(path, bRelativeToData)).exists()) {
            jsFilePath::createEnclosingDirectory(path, bRelativeToData);
        }

        std::filesystem::copy_file(myFile, path);
//    } catch (...) {
//        return false;
//    }

    return true;
}

//------------------------------------------------------------------------------------------------------------
bool jsFile::moveTo(const string& _path, bool bRelativeToData, bool overwrite) {
    std::string path = _path;

    if (path.empty()) {
        return false;
    }

    if (!exists()) {
        return false;
    }

    if (bRelativeToData) {
        path = jsToDataPath(path);
    }

    if (jsFile::doesFileExist(path, bRelativeToData)) {
        if (isFile() && jsFile(path, jsFile::Reference).isDirectory()) {
            path = jsFilePath::join(path, getFileName());
        }

        if (jsFile::doesFileExist(path, bRelativeToData)) {
            if (overwrite) {
                jsFile::removeFile(path, bRelativeToData);
            }
        }
    }

//    try {
        auto mode = this->mode;

        if (mode != jsFile::Reference) {
            changeMode(jsFile::Reference, binary);
        }

        if (!jsDirectory(jsFilePath::getEnclosingDirectory(path, bRelativeToData)).exists()) {
            jsFilePath::createEnclosingDirectory(path, bRelativeToData);
        }

        std::filesystem::rename(myFile, path);
        myFile = path;

        if (mode != jsFile::Reference) {
            changeMode(mode, binary);
        }
//    } catch (...) {
//        return false;
//    }

    return true;
}

//------------------------------------------------------------------------------------------------------------
bool jsFile::renameTo(const string& path, bool bRelativeToData, bool overwrite) {
    return moveTo(path, bRelativeToData, overwrite);
}

//------------------------------------------------------------------------------------------------------------
bool jsFile::remove(bool recursive) {
    if (myFile.string().empty()) {
        return false;
    }

    if (!exists()) {
        return false;
    }

//    try {
        if (mode != Reference) {
            open(path(), Reference, binary);
        }

        if (recursive) {
            std::filesystem::remove_all(myFile);
        } else {
            std::filesystem::remove(myFile);
        }
//    } catch (...) {
//        return false;
//    }

    return true;
}

//------------------------------------------------------------------------------------------------------------
uint64_t jsFile::getSize() const {
//    try {
        return std::filesystem::file_size(myFile);
//    } catch (...) {
//        return 0;
//    }
}

//------------------------------------------------------------------------------------------------------------
bool jsFile::operator==(const jsFile & file) const {
    return getAbsolutePath() == file.getAbsolutePath();
}

//------------------------------------------------------------------------------------------------------------
bool jsFile::operator!=(const jsFile & file) const {
    return getAbsolutePath() != file.getAbsolutePath();
}

//------------------------------------------------------------------------------------------------------------
bool jsFile::operator<(const jsFile & file) const {
    return getAbsolutePath() < file.getAbsolutePath();
}

//------------------------------------------------------------------------------------------------------------
bool jsFile::operator<=(const jsFile & file) const {
    return getAbsolutePath() <= file.getAbsolutePath();
}

//------------------------------------------------------------------------------------------------------------
bool jsFile::operator>(const jsFile & file) const {
    return getAbsolutePath() > file.getAbsolutePath();
}

//------------------------------------------------------------------------------------------------------------
bool jsFile::operator>=(const jsFile & file) const {
    return getAbsolutePath() >= file.getAbsolutePath();
}

//------------------------------------------------------------------------------------------------------------
// ofFile Static Methods
//------------------------------------------------------------------------------------------------------------

bool jsFile::copyFromTo(const std::string& pathSrc, const std::string& pathDst, bool bRelativeToData, bool overwrite) {
    return jsFile(pathSrc, jsFile::Reference).copyTo(pathDst, bRelativeToData, overwrite);
}

//be careful with slashes here - appending a slash when moving a folder will causes mad headaches
//------------------------------------------------------------------------------------------------------------
bool jsFile::moveFromTo(const std::string& pathSrc, const std::string& pathDst, bool bRelativeToData, bool overwrite) {
    return jsFile(pathSrc, jsFile::Reference).moveTo(pathDst, bRelativeToData, overwrite);
}

//------------------------------------------------------------------------------------------------------------
bool jsFile::doesFileExist(const std::string& _fPath, bool bRelativeToData) {
    std::string fPath = _fPath;

    if (bRelativeToData) {
        fPath = jsToDataPath(fPath);
    }

    jsFile file(fPath, jsFile::Reference);
    return !fPath.empty() && file.exists();
}

//------------------------------------------------------------------------------------------------------------
bool jsFile::removeFile(const std::string& _path, bool bRelativeToData) {
    std::string path = _path;

    if (bRelativeToData) {
        path = jsToDataPath(path);
    }

    return jsFile(path, jsFile::Reference).remove();
}

//------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------
// -- jsDirectory
//------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------

//------------------------------------------------------------------------------------------------------------
jsDirectory::jsDirectory() {
    showHidden = false;
}

//------------------------------------------------------------------------------------------------------------
jsDirectory::jsDirectory(const std::filesystem::path & path) {
    showHidden = false;
    open(path);
}

//------------------------------------------------------------------------------------------------------------
void jsDirectory::open(const std::filesystem::path & path) {
    originalDirectory = jsFilePath::getPathForDirectory(path.string());
    myDir = std::filesystem::path(jsToDataPath(originalDirectory)); // Jimmy ***
    //printf( "     path: %s\n", path.string().c_str() );
    //printf( " original: %s\n", originalDirectory.c_str() );
    //printf( "    jsDir: %s\n", myDir.string().c_str() );
    files.clear();
    dirs.clear();
}

//------------------------------------------------------------------------------------------------------------
void jsDirectory::close() {
    myDir = std::filesystem::path();
}

//------------------------------------------------------------------------------------------------------------
bool jsDirectory::create(bool recursive) {
    if (!myDir.string().empty()) {
//        try {
            if (recursive) {
                std::filesystem::create_directories(myDir);
            } else {
                std::filesystem::create_directory(myDir);
            }
//        } catch (...) {
//            return false;
//        }
    }

    return true;
}

//------------------------------------------------------------------------------------------------------------
bool jsDirectory::exists() const {
    return std::filesystem::exists(myDir);
}

//------------------------------------------------------------------------------------------------------------
string jsDirectory::path() const {
    return myDir.string();
}

//------------------------------------------------------------------------------------------------------------
string jsDirectory::getAbsolutePath() const {
//    try {
        return std::filesystem::canonical(std::filesystem::absolute(myDir)).string();
//    } catch (...) {
//        return std::filesystem::absolute(myDir).string();
//    }
}

//------------------------------------------------------------------------------------------------------------
bool jsDirectory::canRead() const {
    return jsFile(myDir, jsFile::Reference).canRead();
}

//------------------------------------------------------------------------------------------------------------
bool jsDirectory::canWrite() const {
    return jsFile(myDir, jsFile::Reference).canWrite();
}

//------------------------------------------------------------------------------------------------------------
bool jsDirectory::canExecute() const {
    return jsFile(myDir, jsFile::Reference).canExecute();
}

//------------------------------------------------------------------------------------------------------------
bool jsDirectory::isHidden() const {
    return jsFile(myDir, jsFile::Reference).isHidden();
}

//------------------------------------------------------------------------------------------------------------
void jsDirectory::setWriteable(bool flag) {
    return jsFile(myDir, jsFile::Reference).setWriteable(flag);
}

//------------------------------------------------------------------------------------------------------------
void jsDirectory::setReadOnly(bool flag) {
    return jsFile(myDir, jsFile::Reference).setReadOnly(flag);
}

//------------------------------------------------------------------------------------------------------------
void jsDirectory::setExecutable(bool flag) {
    return jsFile(myDir, jsFile::Reference).setExecutable(flag);
}

//------------------------------------------------------------------------------------------------------------
void jsDirectory::setShowHidden(bool showHidden) {
    this->showHidden = showHidden;
}

//------------------------------------------------------------------------------------------------------------
bool jsDirectory::isDirectory() const {
    return std::filesystem::is_directory(myDir);
}

//------------------------------------------------------------------------------------------------------------
bool jsDirectory::copyTo(const std::string& _path, bool bRelativeToData, bool overwrite) {
    std::string path = _path;

    if (myDir.string().empty()) {
        return false;
    }

    if (!std::filesystem::exists(myDir)) {
        return false;
    }

    if (!std::filesystem::is_directory(myDir)) {
        return false;
    }

    if (bRelativeToData) {
        path = jsToDataPath(path, bRelativeToData);
    }

    if (jsDirectory::doesDirectoryExist(path, bRelativeToData)) {
        if (overwrite) {
            jsDirectory::removeDirectory(path, true, bRelativeToData);
        } else {
            return false;
        }
    }

    jsDirectory(path).create(true);

    // Iterate through the source directory
    for (std::filesystem::directory_iterator file(myDir); file != std::filesystem::directory_iterator(); ++file) {
        auto currentPath = std::filesystem::absolute(file->path());
        auto dst = std::filesystem::path(path) / currentPath.filename();
        if (std::filesystem::is_directory(currentPath)) {
            jsDirectory current(currentPath);

            // Found directory: Recursion
            if (!current.copyTo(dst.string(), false)) {
                return false;
            }
        } else {
            jsFile(file->path(), jsFile::Reference).copyTo(dst.string(), false);
        }
    }

    return true;
}

//------------------------------------------------------------------------------------------------------------
bool jsDirectory::moveTo(const std::string& path, bool bRelativeToData, bool overwrite) {
    if (copyTo(path, bRelativeToData, overwrite)) {
        return remove(true);
    } else {
        return false;
    }
}

//------------------------------------------------------------------------------------------------------------
bool jsDirectory::renameTo(const std::string& path, bool bRelativeToData, bool overwrite) {
    return moveTo(path, bRelativeToData, overwrite);
}

//------------------------------------------------------------------------------------------------------------
bool jsDirectory::remove(bool recursive) {
    if (path().empty() || !std::filesystem::exists(myDir)) {
        return false;
    }

//    try {
        if (recursive) {
            std::filesystem::remove_all(std::filesystem::canonical(myDir));
        } else {
            std::filesystem::remove(std::filesystem::canonical(myDir));
        }
//    } catch (...) {
//        return false;
//    }

    return true;
}

//------------------------------------------------------------------------------------------------------------
void jsDirectory::allowExt(const std::string& extension) {
    extensions.push_back(guiToLower(extension));
}

//------------------------------------------------------------------------------------------------------------
std::size_t jsDirectory::listDir(const std::string& directory) {
    open(directory);
    return listDir();
}

template<class T, class BoolFunction>
void jsRemove(vector<T>& values, BoolFunction shouldErase) {
    values.erase(remove_if(values.begin(), values.end(), shouldErase), values.end());
}

template <class T>
std::size_t jsFind(const vector<T>& values, const T& target) {
    return distance(values.begin(), find(values.begin(), values.end(), target));
}

template <class T>
bool jsContains(const vector<T>& values, const T& target) {
    return jsFind(values, target) != values.size();
}

// pom
//------------------------------------------------------------------------------------------------------------
std::size_t jsDirectory::listDir() {
    files.clear();
    dirs.clear();

#ifdef __ANDROID__
    GUI_Log("In ListDir()\n");
    GUI_Log("%s\n", path().c_str());

    char sz[1024];
    GUI_Log("_ASSET_LIST_: %s\n", path().c_str());

    if (path().substr(0, 1) != "/" && path().substr(0, 5) != "data/") {
        sprintf(sz, "data/%s", path().c_str());
    } else {
        sprintf(sz, "%s", path().c_str());
    }

    if (sz[strlen(sz) - 1] == '/') {
        sz[strlen(sz) - 1] = 0;
    }

    GUI_Log("_ASSET_LIST_Dir: %s\n", sz);

    if (sz[0] != '/') {
        
        return 0;
    }
#endif

    if (path().empty()) {
#ifdef __ANDROID__
        GUI_Log("jsDirectory::listDir(): directory path is empty\n");
#else
        GUI_LogError("ofDirectory") << "listDir(): directory path is empty";
#endif
        return 0;
    }

    if (!std::filesystem::exists(myDir)) {
#ifdef __ANDROID__
        GUI_Log("ofDirectory::listDir:() source directory does not exist: %s\n", myDir.string().c_str());
#else
        GUI_LogError("ofDirectory") << "listDir:() source directory does not exist: \"" << myDir << "\"";
#endif
        return 0;
    }

    std::filesystem::directory_iterator end_iter;

    if (std::filesystem::exists(myDir) && std::filesystem::is_directory(myDir) && !std::filesystem::is_symlink(myDir)) {
#ifdef __ANDROID__
        GUI_Log("dir ok -- 1\n");

#endif

//        try {
            // Android crashed in iteration loop
            for (std::filesystem::directory_iterator dir_iter(myDir); dir_iter != end_iter; ++dir_iter) {
#ifdef __ANDROID__
                GUI_Log("%s\n", dir_iter->path().string().c_str());
#endif
                if( std::filesystem::is_directory(dir_iter->path().string()) ) {
                    // files.emplace_back(dir_iter->path().string(), jsFile::Reference);
                    //printf( "Path: %s\n", path().c_str());
                    //printf( "Dirs: %s\n", dir_iter->path().string().c_str() );
                    std::string _p = path();
                    int l = _p.length();
#ifdef __WINDOWS__
                    if( _p.c_str()[_p.length()-1] != '\\' ) {
#else
                    if( _p.c_str()[_p.length()-1] != '/' ) {
#endif
                        l += 1;
                    }
                    std::string dir_name = dir_iter->path().string().substr(l);
                    //printf( "dir_name: %s\n", dir_name.c_str());
                    
                    if (dir_name.c_str()[0] != '.' && dir_name.c_str()[0] != '$' ) {
                        dirs.push_back( dir_name );
                    }
                    //files.emplace_back(dir_iter->path().string(), jsFile::Reference);
                    continue;
                }
                //printf( "%s\n", dir_iter->path().string().c_str() );
                files.emplace_back(dir_iter->path().string(), jsFile::Reference);
            }
            jsSort(dirs);
//        } catch (...) {
//            GUI_Log("Catched error\n");
//        }
    } else {
#ifdef __ANDROID__
        GUI_Log("ofDirectory::listDir:() source directory does not exist: %s\n", myDir.string().c_str());
#else
        GUI_LogError("ofDirectory") << "listDir:() source directory does not exist: \"" << myDir << "\"";
#endif
        return 0;
    }

#ifdef __ANDROID__
    GUI_Log("listed");
#endif

    if (!showHidden) {
        jsRemove(files, [](jsFile & file) {
            return file.isHidden();
        });
    }
#ifdef __ANDROID__
    GUI_Log("removed hidden\n");
#endif

    if (!extensions.empty() && !jsContains(extensions, (string)"*")) {
        jsRemove(files, [&](jsFile & file) {
            return ((std::find(extensions.begin(), extensions.end(), guiToLower(file.getExtension())) == extensions.end()) && (!file.isDirectory()));
        });
    }
#ifdef __ANDROID__
    GUI_Log("OK\n");
#endif

    if (GUI_GetLogLevel() == GUI_LogLevel::GUI_LOG_VERBOSE) {
        for (int i = 0; i < (int)size(); i++) {
            GUI_LogVerbose() << "\t" << getName(i);
        }
        GUI_LogVerbose() << "listed " << size() << " files in \"" << originalDirectory << "\"";
    }

#ifdef __ANDROID__
    GUI_Log("about to return\n");
#endif
    return size();
}

//------------------------------------------------------------------------------------------------------------
string jsDirectory::getOriginalDirectory() const {
    return originalDirectory;
}

//------------------------------------------------------------------------------------------------------------
string jsDirectory::getName(std::size_t position) const {
    return files.at(position).getFileName();
}

//------------------------------------------------------------------------------------------------------------
string jsDirectory::getPath(std::size_t position) const {
    return originalDirectory + getName(position);
}

//------------------------------------------------------------------------------------------------------------
jsFile jsDirectory::getFile(std::size_t position, jsFile::Mode mode, bool binary) const {
    jsFile file = files[position];
    file.changeMode(mode, binary);
    return file;
}

jsFile jsDirectory::operator[](std::size_t position) const {
    return getFile(position);
}

//------------------------------------------------------------------------------------------------------------
const vector<jsFile> & jsDirectory::getFiles() const {
    if (files.empty() && !myDir.empty()) {
        const_cast<jsDirectory*>(this)->listDir();
    }

    return files;
}

//------------------------------------------------------------------------------------------------------------
bool jsDirectory::getShowHidden() const {
    return showHidden;
}

//------------------------------------------------------------------------------------------------------------
void jsDirectory::reset() {
    close();
}

//------------------------------------------------------------------------------------------------------------
static bool natural(const jsFile& a, const jsFile& b) {
    string aname = a.getBaseName(), bname = b.getBaseName();
    int aint = atoi(aname.c_str()), bint = atoi(bname.c_str());

    if (jsToString(aint) == aname && jsToString(bint) == bname) {
        return aint < bint;
    } else {
        return a < b;
    }
}



//------------------------------------------------------------------------------------------------------------
void jsDirectory::sort() {
    if (files.empty() && !myDir.empty()) {
        listDir();
    }
    jsSort(files, natural);
}

//------------------------------------------------------------------------------------------------------------
jsDirectory jsDirectory::getSorted() {
    jsDirectory sorted(*this);
    sorted.listDir();
    sorted.sort();
    return sorted;
}

//------------------------------------------------------------------------------------------------------------
std::size_t jsDirectory::size() const {
    return files.size();
}

//------------------------------------------------------------------------------------------------------------
// jsDirectory Static Methods
//------------------------------------------------------------------------------------------------------------

//------------------------------------------------------------------------------------------------------------
bool jsDirectory::removeDirectory(const std::string& _path, bool deleteIfNotEmpty, bool bRelativeToData) {
    std::string path = _path;

    if (bRelativeToData) {
        path = jsToDataPath(path);
    }

    return jsFile(path, jsFile::Reference).remove(deleteIfNotEmpty);
}

//------------------------------------------------------------------------------------------------------------
bool jsDirectory::createDirectory(const std::string& _dirPath, bool bRelativeToData, bool recursive) {
    std::string dirPath = _dirPath;

    if (bRelativeToData) {
        dirPath = jsToDataPath(dirPath);
    }


    // on OSX,std::filesystem::create_directories seems to return false *if* the path has folders that already exist
    // and true if it doesn't
    // so to avoid unnecessary warnings on OSX, we check if it exists here:
    bool bDoesExistAlready = jsDirectory::doesDirectoryExist(dirPath);

    if (!bDoesExistAlready) {
        bool success = false;

//        try {
            if (!recursive) {
                success = std::filesystem::create_directory(dirPath);
            } else {
                success = std::filesystem::create_directories(dirPath);
            }
//        } catch (...) {
//            return false;
//        }

        return success;
    } else {
        // no need to create it - it already exists.
        return true;
    }
}

//------------------------------------------------------------------------------------------------------------
bool jsDirectory::doesDirectoryExist(const std::string& _dirPath, bool bRelativeToData) {
    std::string dirPath = _dirPath;

    if (bRelativeToData) {
        dirPath = jsToDataPath(dirPath);
    }

    return std::filesystem::exists(dirPath) && std::filesystem::is_directory(dirPath);
}

//------------------------------------------------------------------------------------------------------------
bool jsDirectory::isDirectoryEmpty(const std::string& _dirPath, bool bRelativeToData) {
    std::string dirPath = _dirPath;

    if (bRelativeToData) {
        dirPath = jsToDataPath(dirPath);
    }

    if (!dirPath.empty() && std::filesystem::exists(dirPath) && std::filesystem::is_directory(dirPath)) {
        return std::filesystem::directory_iterator(dirPath) == std::filesystem::directory_iterator();
    }

    return false;
}

//------------------------------------------------------------------------------------------------------------
bool jsDirectory::operator==(const jsDirectory & dir) const {
    return getAbsolutePath() == dir.getAbsolutePath();
}

//------------------------------------------------------------------------------------------------------------
bool jsDirectory::operator!=(const jsDirectory & dir) const {
    return getAbsolutePath() != dir.getAbsolutePath();
}

//------------------------------------------------------------------------------------------------------------
bool jsDirectory::operator<(const jsDirectory & dir) const {
    return getAbsolutePath() < dir.getAbsolutePath();
}

//------------------------------------------------------------------------------------------------------------
bool jsDirectory::operator<=(const jsDirectory & dir) const {
    return getAbsolutePath() <= dir.getAbsolutePath();
}

//------------------------------------------------------------------------------------------------------------
bool jsDirectory::operator>(const jsDirectory & dir) const {
    return getAbsolutePath() > dir.getAbsolutePath();
}

//------------------------------------------------------------------------------------------------------------
bool jsDirectory::operator>=(const jsDirectory & dir) const {
    return getAbsolutePath() >= dir.getAbsolutePath();
}

//------------------------------------------------------------------------------------------------------------
vector<jsFile>::const_iterator jsDirectory::begin() const {
    return getFiles().begin();
}

//------------------------------------------------------------------------------------------------------------
vector<jsFile>::const_iterator jsDirectory::end() const {
    return files.end();
}

//------------------------------------------------------------------------------------------------------------
vector<jsFile>::const_reverse_iterator jsDirectory::rbegin() const {
    return getFiles().rbegin();
}

//------------------------------------------------------------------------------------------------------------
vector<jsFile>::const_reverse_iterator jsDirectory::rend() const {
    return files.rend();
}

//------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------
// -- ofFilePath
//------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------

//------------------------------------------------------------------------------------------------------------
string jsFilePath::addLeadingSlash(const std::string& _path) {
    std::string path = _path;
    auto sep = std::filesystem::path("/").make_preferred();

    if (!path.empty()) {
        if (jsToString(path[0]) != sep.string()) {
            path = (sep / path).string();
        }
    }

    return path;
}

//------------------------------------------------------------------------------------------------------------
string jsFilePath::addTrailingSlash(const std::string& _path) {    
#if defined(_MSC_VER) || defined(__linux__)
    if (_path.empty()) {
      return "\\";
    }
    
    // C++ Filesystem library: how to add trailing separators
    // see: 
    // - https://en.cppreference.com/w/cpp/filesystem/path/append
    // - https://en.cppreference.com/w/cpp/filesystem/canonical
		//
    // PoC
    // 1. if _path already has trailing separators ('/' or '\'), it will removes before append others.
    // 2. then appends preferred separator.
    // 3. then appends empty string, 
    // note: if second string start with separator ('/' or '\') it will replaces first with second instead.
    //
    // [tests]
    // std::string test;
    // std::cout << "\n";
    // test = "obj/data/Emoticon/PES/Small////";
    // std::cout << "input:\n'" << test << "'\noutput:\n'" << ( std::filesystem::weakly_canonical(test) / "" ).make_preferred().string() << "'\n";
    // test = "obj/////data/Emoticon/PES/Small";
    // std::cout << "input:\n'" << test << "'\noutput:\n'" << ( std::filesystem::weakly_canonical(test) / "" ).make_preferred().string() << "'\n";
    // test = "obj\\./data/.\\Emoticon\\PES/..\\.\\PES///Small\\";
    // std::cout << "input:\n'" << test << "'\noutput:\n'" << ( std::filesystem::weakly_canonical(test) / "" ).make_preferred().string() << "'\n";
    // std::cout << "\n";
    // 
    // std::cout << "\n";
    // test = "/obj/data/Emoticon/PES/Small////";
    // std::cout << "input:\n'" << test << "'\noutput:\n'" << ( std::filesystem::weakly_canonical(test) / "" ).make_preferred().string() << "'\n";
    // test = "\\obj/////data/Emoticon/PES/Small";
    // std::cout << "input:\n'" << test << "'\noutput:\n'" << ( std::filesystem::weakly_canonical(test) / "" ).make_preferred().string() << "'\n";
    // test = "/obj\\./data/.\\Emoticon\\PES/..\\.\\PES///Small\\";
    // std::cout << "input:\n'" << test << "'\noutput:\n'" << ( std::filesystem::weakly_canonical(test) / "" ).make_preferred().string() << "'\n";
    // std::cout << "\n";
    
    // inputs
    // 1. "obj/data/Emoticon/PES/Small////"
    // 2. "obj/////data/Emoticon/PES/Small"
    // 3. "obj\\./data/.\\Emoticon\\PES/..\\.\\PES///Small\\"
    //
    // all-outputs
    // 'C:\-full-path-of-cwd-\obj\data\Emoticon\PES\Small\'
    
    // inputs
    // 1. "/obj/data/Emoticon/PES/Small////"
    // 2. "\\obj/////data/Emoticon/PES/Small"
    // 3. "/obj\\./data/.\\Emoticon\\PES/..\\.\\PES///Small\\"
    //
    // all-outputs
    // '\obj\data\Emoticon\PES\Small\'

    // !!use canonical in tests only, but this is 'addTrailingSlash'
    return ( std::filesystem::path(_path) / "" ).make_preferred().string();
#else
    std::string path = _path;
    path = std::filesystem::path(path).make_preferred().string();
    auto sep = std::filesystem::path("/").make_preferred();
    if (!path.empty()) {
        if (jsToString(path.back()) != sep.string()) {
            path = (std::filesystem::path(std::string(_path)+sep.string())).string();
        }
    }
    return path;
#endif    
}

//------------------------------------------------------------------------------------------------------------
string jsFilePath::getFileExt(const std::string& filename) {
    return jsFile(filename, jsFile::Reference).getExtension();
}

//------------------------------------------------------------------------------------------------------------
string jsFilePath::removeExt(const std::string& filename) {
    return jsFilePath::join(getEnclosingDirectory(filename, false), jsFile(filename, jsFile::Reference).getBaseName());
}

//------------------------------------------------------------------------------------------------------------
string jsFilePath::getPathForDirectory(const std::string& path) {
    // if a trailing slash is missing from a path, this will clean it up
    // if it's a windows-style "\" path it will add a "\"
    // if it's a unix-style "/" path it will add a "/"
    // noom
    //auto sep = std::filesystem::path("/").make_preferred();
    //
    //if (!path.empty() && jsToString(path.back()) != sep.string()) {
    //    //return (std::filesystem::path(path) / sep).string(); // Jimmy *** sonething wrong
    //    return path + sep.string(); // Jimmy
    if (!path.empty()) {
        return (std::filesystem::path(path) / std::filesystem::path("")).make_preferred().string();
    } else {
        return path;
    }
}

//------------------------------------------------------------------------------------------------------------
string jsFilePath::removeTrailingSlash(const std::string& _path) {
    std::string path = _path;

    if (path.length() > 0 && (path[path.length() - 1] == '/' || path[path.length() - 1] == '\\')) {
        path = path.substr(0, path.length() - 1);
    }

    return path;
}


//------------------------------------------------------------------------------------------------------------
string jsFilePath::getFileName(const std::string& _filePath, bool bRelativeToData) {
    std::string filePath = _filePath;

    if (bRelativeToData) {
        filePath = jsToDataPath(filePath);
    }

    return std::filesystem::path(filePath).filename().string();
}

//------------------------------------------------------------------------------------------------------------
string jsFilePath::getBaseName(const std::string& filePath) {
    return jsFile(filePath, jsFile::Reference).getBaseName();
}

//------------------------------------------------------------------------------------------------------------
string jsFilePath::getEnclosingDirectory(const std::string& _filePath, bool bRelativeToData) {
    std::string filePath = _filePath;

    if (bRelativeToData) {
        filePath = jsToDataPath(filePath);
    }

    return addTrailingSlash(std::filesystem::path(filePath).parent_path().string());
}

//------------------------------------------------------------------------------------------------------------
bool jsFilePath::createEnclosingDirectory(const std::string& filePath, bool bRelativeToData, bool bRecursive) {
    return jsDirectory::createDirectory(jsFilePath::getEnclosingDirectory(filePath), bRelativeToData, bRecursive);
}

//------------------------------------------------------------------------------------------------------------
string jsFilePath::getAbsolutePath(const std::string& path, bool bRelativeToData) {
    if (bRelativeToData) {
        return jsToDataPath(path, true);
    } else {
//        try {
            return std::filesystem::canonical(std::filesystem::absolute(path)).string();
//        } catch (...) {
//            return std::filesystem::absolute(path).string();
//        }
    }
}

//------------------------------------------------------------------------------------------------------------
bool jsFilePath::isAbsolute(const std::string& path) {
    return std::filesystem::path(path).is_absolute();
}

//------------------------------------------------------------------------------------------------------------
string jsFilePath::getCurrentWorkingDirectory() {
    return std::filesystem::current_path().string();
}

//------------------------------------------------------------------------------------------------------------
string jsFilePath::join(const std::string& path1, const std::string& path2) {
    return (std::filesystem::path(path1) / std::filesystem::path(path2)).string();
}

//------------------------------------------------------------------------------------------------------------
string jsFilePath::getCurrentExePath() {
#if defined(__LINUX__) 
    return "./";
#elif defined(__ANDROID__)
    char buff[FILENAME_MAX];
    ssize_t size = readlink("/proc/self/exe", buff, sizeof(buff) - 1);

    if (size == -1) {
        GUI_Log("getCurrentExePath(): readlink failed with error");
    } else {
        buff[size] = '\0';
        return buff;
    }
#elif defined(__MACOSX__)
    char path[FILENAME_MAX];
    uint32_t size = sizeof(path);

    if (_NSGetExecutablePath(path, &size) != 0) {
        GUI_LogError("ofFilePath") << "getCurrentExePath(): path buffer too small, need size " << size;
    }

    return path;

#elif defined(_WIN32)
    vector<char> executablePath(MAX_PATH);
    DWORD result = ::GetModuleFileNameA(nullptr, &executablePath[0], static_cast<DWORD>(executablePath.size()));

    if (result != 0) {
        return string(executablePath.begin(), executablePath.begin() + result);
}
#endif
    return "";
}

//------------------------------------------------------------------------------------------------------------
string jsFilePath::getCurrentExeDir() {
    return getEnclosingDirectory(getCurrentExePath(), false);
}

std::string jsGetEnv(const std::string & var) {
#ifdef _WIN32
    const size_t BUFSIZE = 4096;
    std::vector<char> pszOldVal(BUFSIZE, 0);
    auto size = GetEnvironmentVariableA(var.c_str(), pszOldVal.data(), BUFSIZE);

    if (size > 0) {
        return std::string(pszOldVal.begin(), pszOldVal.begin() + size);
    } else {
        return "";
    }
#else
    auto value = getenv(var.c_str());

    if (value) {
        return value;
    } else {
        return "";
    }
#endif
}

//------------------------------------------------------------------------------------------------------------
string jsFilePath::getUserHomeDir() {
#ifdef _WIN32
    // getenv will return any Environent Variable on Windows
    // USERPROFILE is the key on Windows 7 but it might be HOME
    // in other flavours of windows...need to check XP and NT...
    return jsGetEnv("USERPROFILE");
#elif !defined(__EMSCRIPTEN__)
//    struct passwd * pw = getpwuid(getuid());
//    return pw->pw_dir;
    return "";
#else
    return "";
#endif
}

string jsFilePath::makeRelative(const std::string & from, const std::string & to) {
    auto pathFrom = std::filesystem::absolute(from);
    auto pathTo = std::filesystem::absolute(to);
    std::filesystem::path ret;
    std::filesystem::path::const_iterator itrFrom(pathFrom.begin()), itrTo(pathTo.begin());

    // Find common base
    for (std::filesystem::path::const_iterator toEnd(pathTo.end()), fromEnd(pathFrom.end()); itrFrom != fromEnd && itrTo != toEnd && *itrFrom == *itrTo; ++itrFrom, ++itrTo);
    // Navigate backwards in directory to reach previously found base

    for (std::filesystem::path::const_iterator fromEnd(pathFrom.end()); itrFrom != fromEnd; ++itrFrom) {
        if ((*itrFrom) != ".") {
            ret /= "..";
        }
    }

    // Now navigate down the directory branch
    for (; itrTo != pathTo.end(); ++itrTo) {
        if (itrTo->string() != ".") {
            ret /= *itrTo;
        }
    }

    return ret.string();
}
