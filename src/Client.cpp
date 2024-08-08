#include "../includes/Client.hpp"

const std::map<int, std::string> Client::_ErrorMap = {
    {200, "OK"},
    {201, "Created"},
    {204, "No Content"},
    {400, "Bad Formatting"},
    {401, "Unauthorized"},
    {404, "Not Found"},
    {405, "Method Not Allowed"},
    {500, "Internal Server Error"},
    {503, "Service Unavailable"},
};

Client::Client() {}

Client::~Client() { }

Client::Client( int fd ): _fd(fd) {}

Client::Client(const Client& rhs)
{
    _fd = rhs._fd;
    _readBuffer = rhs._readBuffer;
    _writeBuffer = rhs._writeBuffer;
    _writePos = rhs._writePos;
    _time = rhs._time;
}

Client& Client::operator=(const Client& rhs)
{
    if (this != &rhs)
    {
        _fd = rhs._fd;
        _readBuffer = rhs._readBuffer;
        _writeBuffer = rhs._writeBuffer;
        _writePos = rhs._writePos;
        _time = rhs._time;
    }
    return *this;
}

void    Client::addToBuffer( std::string bufferNew )
{
    _readBuffer += bufferNew;
}

std::string    Client::getReadBuffer( void )
{
    return (_readBuffer);
}

void Client::setFd ( int fd )
{
    _fd = fd;
}

int Client::getFd()
{
    return (_fd);
}

size_t      Client::getWritePos()
{
    return (_writePos);
}

void        Client::setWritePos( size_t pos )
{
    _writePos = pos;
}

std::string Client::getWriteBuffer()
{
    return (_writeBuffer);
}

void        Client::setWriteBuffer( std::string buffer )
{
    _writeBuffer = buffer;
}

void        Client::setFileBuffer(std::string buffer)
{
    _fileBuffer = buffer;
}

int Client::getState()
{
    return (_state);
}

void Client::setState (const int state)
{
    _state = state;
}

void Client::setStatusCode( const int statusCode )
{
    std::vector<int>     statusCheck = {400, 401, 404, 405, 500, 503};
    if (std::find(statusCheck.begin(), statusCheck.end(), statusCode) != statusCheck.end()){
        setState(ERROR);
    }
    _statusCode = statusCode;
}

bool    Client::requestComplete()
{
    size_t pos = _readBuffer.find("\r\n\r\n");

    if (pos == std::string::npos)
        return false;
    
    std::string headers = _readBuffer.substr(0, pos + 4);
    size_t posContent = headers.find("Content-Length:");

    if (posContent == std::string::npos)
        return true;
    
    size_t contentEnd = headers.find("\r\n", posContent);
    std::string content = headers.substr(posContent + 15, contentEnd - posContent - 15);
    int contentLength = std::stoi(content);

    size_t bodyBegin = pos + 4;
    size_t bodyLength = bodyBegin + contentLength;

    return  _readBuffer.size() >= bodyLength;
}

void    Client::updateTime()
{
    _time = std::time(nullptr);
}

std::time_t Client::getTime()
{
    return _time;
}

void    Client::parseBuffer ( void )
{
    std::string line, key, value;
    bool startBody = false;

    // Storing the (first) request line with the method (GET/POST etc..), path and version in headerMap
    std::istringstream stream(_readBuffer);
    if (std::getline(stream, line))
    {
        std::istringstream lineStream(line);
        lineStream >> _requestMap["Method"] >> _requestMap["Path"] >> _requestMap["Version"]; // Error Management missing if wrong request line format!
    }
    // Storing the rest of the incoming header in headerMap
    while (std::getline(stream, line, '\n'))
    {
        std::istringstream lineStream(line);
        if (startBody == false && line == "\r"){
            startBody = true;
        }
        if (startBody == true){
            _requestMap["Body"] += line + '\n';
        }
        else if (std::getline(lineStream, key, ':'))
        {
            if (std::getline(lineStream, value))
            {            
                value = trimWhiteSpace(value);
                _requestMap[key] = value;
            }
        }
    }
    isValidMethod(_requestMap["Method"]);
    isValidPath(_requestMap["Path"]);
    isValidVersion(_requestMap["Version"]); 
}

void    Client::printRequestMap( void )
{
    // Printing header map
    std::cout << "------- Content of header map -------\n";
    for (const auto& pair : _requestMap)
    {
        std::cout << pair.first << ":" << pair.second << std::endl;
    }
    std::cout << "\n------- Content of header map -------\n";
}

std::map<std::string, std::string> Client::getRequestMap( void )
{
    return (_requestMap);
}

void   Client::isValidMethod( std::string method )
{
    std::vector<std::string> validMethods = {"POST", "GET", "DELETE"};

    if (method.empty()){
        setStatusCode(400);
    }
    else if (std::find(validMethods.begin(), validMethods.end(), method) == validMethods.end()){
        setStatusCode(405);
    }
}

void    Client::isValidPath( std::string path )
{
    if (path.empty()){
        setStatusCode(400);
    }
}

void    Client::isValidVersion( std::string version )
{
    std::regex versionRegex(R"(HTTP\/\d\.\d)");

    if (version.empty()){
        setStatusCode(400);
    }
    else if (!std::regex_match(version, versionRegex)){
        setStatusCode(505);
    }
}

std::string trimWhiteSpace(std::string& string)
{
    size_t start = string.find_first_not_of(" \n\t\r");
    size_t end = string.find_last_not_of(" \n\t\r");

    return string.substr(start, end - start + 1);
}

std::string Client::readFile ( std::string file )
{
    int     fileFd;
    int     bytesRead;
    int     bufferSize = 2000;
    char    buffer[bufferSize];

    fileFd = open(file.c_str(), O_RDONLY);
    if (fileFd == -1)
    {
        perror("file open");
        exit (1);
    }
    bytesRead = read(fileFd, buffer, bufferSize - 1);
    if (bytesRead == -1){
        perror("read");
        exit (1);
    }
    buffer[bytesRead] = '\0';
    close(fileFd);
    return (buffer);
}

std::string Client::createErrorResponse( void )
{
    std::string errorResponse;
    std::string file = "./config/error_page/" + std::to_string(_statusCode) + ".html";
    std::string errorPage = readFile(file);
    
    _responseMap["Content-Type"] = "text/html";
    if (!_requestMap.count("Version"))
        _requestMap["Version"] = "HTTP/1.1";
    errorResponse = _requestMap.at("Version") + " " + std::to_string(_statusCode) + " " + _ErrorMap.at(_statusCode) + "\r\n";
    errorResponse += "Content-Type: " + _responseMap.at("Content-Type") + "\r\n";
    errorResponse += "Content-Length: " + std::to_string(errorPage.size()) + "\r\n\r\n";
    errorResponse += errorPage;
    return (errorResponse);
}

void Client::createResponse ( void )
{
    std::string responseMessage;

    if (_statusCode == 0)
        setStatusCode(200);
    if (getState() == ERROR)
    {
        _writeBuffer = createErrorResponse();
        setState(READY);
    }
    else {
        _responseMap["Content-Type"] = "text/html";
        responseMessage = _requestMap.at("Version") + " " + std::to_string(_statusCode) + " " + _ErrorMap.at(_statusCode) + "\r\n";
        responseMessage += "Content-Type: " + _responseMap.at("Content-Type") + "\r\n";
        if (!_fileBuffer.empty()){
            responseMessage += "Content-Length: " + std::to_string(_fileBuffer.size()) + "\r\n\r\n";
            responseMessage += _fileBuffer;
        }
        else{
            responseMessage += "\r\n";
        }
        _writeBuffer = responseMessage;
    }
}

void Client::readNextChunk()
{
    char buffer[BUFFER_SIZE];
    int bytesRead = read(_fileFd, buffer, BUFFER_SIZE);

    if (bytesRead < 0)
    {
        std::cerr << "Failed to read file: " << strerror(errno) << std::endl;
        setStatusCode(500);
        close(_fileFd);
        _fd = -1;
        setState(READY);
        return;
    }
    else if (bytesRead == 0)
    {
        close(_fileFd);
        setState(READY);
        createResponse();
        return;
    }

    _fileBuffer.append(buffer, bytesRead);

}

void Client::writeNextChunk()
{
    std::string buffer;
    int bytesWritten;

    buffer = getWriteBuffer().substr(0,BUFFER_SIZE);
    bytesWritten = write(getFd(), buffer.c_str(), buffer.length());
    if (bytesWritten < 0)
    {
        std::cerr << "Failed to write to fd: " << strerror(errno) << std::endl;
        setState(500);
        close(_fileFd);
        _fd = -1;
        return ;
    }
    getWriteBuffer().erase(0, BUFFER_SIZE);

    if (getWriteBuffer().empty())
    {
        close(_fileFd);
        setState(READY);
    }
}

void    Client::resetClientData( void )
{
    _readBuffer.clear();
    _writeBuffer.clear();
    _fileBuffer.clear();
    _writePos = 0;
    _requestMap.clear();
    _responseMap.clear();
    _statusCode = 0;
    _fd = -1;
    _state = START;
    _fileFd = -1;
    // Is all added?? -- Sven
}


std::string Client::getFileBuffer()
{
    return _fileBuffer;
}

void Client::setFileFd(int fd)
{
    _fileFd = fd;
}

int Client::getFileFd()
{
    return _fileFd;
}

int* Client::getRequestPipe()
{
    return (_requestPipe);
}

int* Client::getReponsePipe()
{
    return (_responsePipe);
}