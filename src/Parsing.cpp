#include "../includes/Parsing.hpp"
#include <../includes/Client.hpp>


Parsing::Parsing(){};

Parsing::~Parsing(){};

void	Parsing::parseBuffer ( void )
{
	std::string line, key, value;
	bool startBody = false;

	// Storing the (first) request line with the method (GET/POST etc..), path and version in headerMap
	std::istringstream stream(getReadBuffer());
	if (std::getline(stream, line))
	{
		std::istringstream lineStream(line);
		lineStream >> getRequestMap()["Method"] >> getRequestMap()["Path"] >> getRequestMap()["Version"]; // Error Management missing if wrong request line format!
	}
	// Storing the rest of the incoming header in headerMap
	while (std::getline(stream, line, '\n'))
	{
		std::istringstream lineStream(line);
		if (startBody == false && line == "\r")
			startBody = true;
		if (startBody == true)
			getRequestMap()["Body"] += line + '\n';
		else if (std::getline(lineStream, key, ':'))
		{
			if (std::getline(lineStream, value))
			{            
				value = trimWhiteSpace(value);
				getRequestMap()[key] = value;
			}
		}
	}
}

void	Parsing::addToBuffer( std::string bufferNew )
{
	_readBuffer += bufferNew;
}

bool	Parsing::requestComplete( void )
{
	size_t pos = getReadBuffer().find("\r\n\r\n");

	if (pos == std::string::npos)
		return false;
	
	std::string headers = getReadBuffer().substr(0, pos + 4);
	size_t posContent = headers.find("Content-Length:");

	if (posContent == std::string::npos)
		return true;
	
	size_t contentEnd = headers.find("\r\n", posContent);
	std::string content = headers.substr(posContent + 15, contentEnd - posContent - 15);
	int contentLength = std::stoi(content);

	size_t bodyBegin = pos + 4;
	size_t bodyLength = bodyBegin + contentLength;

	return getReadBuffer().size() >= bodyLength;
}

void	Parsing::createResponse(Client &client)
{
	std::string responseMessage;
	int			statusCode = client.getStatusCode();

	if (statusCode == 0)
		client.setStatusCode(200);
	if (statusCode == 301 || statusCode == 302) // Handle redirect responses
		buildRedirectReponse(client);
	else // Handle regular responses
		buildResponse(client);
	client.setState(SENDING);
}

void	Parsing::buildResponse( Client& client )
{
	std::string responseMessage;

	getResponseMap()["Content-Type"] = "text/html";
	responseMessage = getRequestMap().at("Version") + " " + std::to_string(client.getStatusCode()) + " " + client.getStatusMessage(client.getStatusCode()) + "\r\n";
	responseMessage += "Content-Type: " + getResponseMap()["Content-Type"] + "\r\n";
	if (!getFileBuffer().empty())
	{
		responseMessage += "Content-Length: " + std::to_string(getFileBuffer().size()) + "\r\n\r\n";
		responseMessage += getFileBuffer();
	}
	else if (getRequestMap().at("Method") == "GET")
	{
		responseMessage += "Content-Length: 0\r\n\r\n";
	}
	else
		responseMessage += "\r\n";
	setWriteBuffer(responseMessage);
}

void	Parsing::buildRedirectReponse( Client& client )
{
	std::string	responseMessage;

	responseMessage = getRequestMap().at("Version") + " " + std::to_string(client.getStatusCode()) + " " + client.getStatusMessage(client.getStatusCode()) + "\r\n";
	responseMessage += "Location: " + getResponseMap()["Location"] + "\r\n";
	responseMessage += "Content-Length: 0\r\n\r\n";  // Usually no body for redirects
	setWriteBuffer(responseMessage);
}

void	Parsing::detectParsingError( Client& client )
{
	isValidMethod(getRequestMap()["Method"], client);
	isValidPath(getRequestMap()["Path"], client);
	isValidVersion(getRequestMap()["Version"], client); 
}

void	Parsing::printRequestMap( void )
{
    std::unordered_map<std::string, std::string> requestMap = getRequestMap();
    
	std::cout << "------- Content of header map -------\n";
	for (const auto& pair : requestMap)
	{
		std::cout << pair.first << ":" << pair.second << std::endl;
	}
	std::cout << "\n------- Content of header map -------\n";
}

// Getters

std::unordered_map<std::string, std::string>&	Parsing::getResponseMap()
{
	return (_responseMap);
}

std::unordered_map<std::string, std::string>&	Parsing::getRequestMap( void )
{
	return (_requestMap);
}

std::string&	Parsing::getReadBuffer( void )
{
	return (_readBuffer);
}

std::string&	Parsing::getWriteBuffer()
{
	return (_writeBuffer);
}

std::string& Parsing::getFileBuffer()
{
    return (_fileBuffer);
}

size_t	Parsing::getWritePos()
{
	return (_writePos);
}

// Setters

void	Parsing::setWriteBuffer( std::string buffer )
{
	_writeBuffer = buffer;
}

void	Parsing::setFileBuffer(std::string buffer)
{
	_fileBuffer = buffer;
}

void	Parsing::setWritePos( size_t pos )
{
	_writePos = pos;
}
