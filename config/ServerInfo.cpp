#include "ServerInfo.hpp"

// --- Constructor ---
ServerInfo::ServerInfo() : _port(0), _serverFd(-1), _maxClient(0), _host(""), _root(""), _index(""), _serverName("") {}

// --- Setter ---
void	ServerInfo::setHost(const std::string &newHost)
{
	_host = newHost;
}

void	ServerInfo::setPort(int newPort)
{
	_port = newPort;
}

void	ServerInfo::setRoot(const std::string &newRoot)
{
	_root = newRoot;
}

void	ServerInfo::setMaxClient(int newMaxClient)
{
	_maxClient = newMaxClient;
}

void	ServerInfo::setIndex(const std::string &newIndex)
{
	_index = newIndex;
}

void	ServerInfo::setServerName(const std::string &newServerName)
{
	_serverName = newServerName;
}

void ServerInfo::setLocations(const std::vector<Location> &newLocation)
{
	_locations = newLocation;
}

void ServerInfo::setErrorPage(const std::string &newErrorPage)
{
	// Check if variable is already set
	if (!hasErrorPage(newErrorPage))
		_errorPage.push_back(newErrorPage);
	else
		throw  Exception_ServerInfo("Error_page is duplicated");
}

// --- Getter ---
std::string	ServerInfo::getHost() const
{
	return _host;
}

int ServerInfo::getPort() const
{
	return _port;
}

std::string	ServerInfo::getRoot() const
{
	return _root;
}

int ServerInfo::getServerFd() const
{
	return _serverFd;
}

int	ServerInfo::getMaxClient() const
{
	return _maxClient;
}

std::string	ServerInfo::getIndex() const
{
	return _index;
}

std::string	ServerInfo::getServerName() const
{
	return _serverName;
}

std::vector<Location> ServerInfo::getLocations() const
{
	return _locations;
}

std::vector<std::string> ServerInfo::getErrorPage() const
{
	return _errorPage;
}

// --- Utils ---
// Helper function to check fot doubels in error_page
bool ServerInfo::hasErrorPage(const std::string &errorPage) const
{
    return (std::find(_errorPage.begin(), _errorPage.end(), errorPage) != _errorPage.end());
}