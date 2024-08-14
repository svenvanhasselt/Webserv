#ifndef SERVERINFO_HPP
# define SERVERINFO_HPP

# include <iostream>
# include <string.h>
# include <fstream>
# include <vector>

class Config;

class ServerInfo
{
	private:
		std::string			_serverName;	// The name/number of the server.
		std::string			_host;			// The host of the server. 				   
		std::vector<int>	_ports;			// A vector of ports the server listens on.
		std::string			_rootPath;		// The root directory path for the server.  

	public:
		ServerInfo();		// Constructor
		~ServerInfo();		// Destructor

		// Setter
		void setName(const std::string &newName);
		void setHost(const std::string &newHost);
		void setPort(const std::vector<int> &newPort);
		void setRoot(const std::string &newRoot);

		// Getter
		std::string getName() const;
		std::string getHost() const;
		std::vector<int> getPort() const;
		std::string getRoot() const;
};

#endif