#include "../includes/Server.hpp"

Server::Server() {}
Server::~Server() {}
Server::Server(const Server &rhs) {}
Server& Server::operator=(const Server& rhs) {}

/*
    Some things to keep in mind
    - More server sockets might mean a complete change in socket handling(please no T_T)
      this depends on config parsing
    - creating the server socket(s) and running poll could be combined in one function.
      something like "run server"
    - Maybe a function that build the server based on the config parsing
      Another that runs the server.
      Both part of the constructor
*/

void    Server::createServerSocket()
{
    int addrLen = sizeof(_address);
    struct pollfd serverFd;

    // Creating socket file descriptor
    if ((_serverSocket = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        std::cerr << RED << "Socket failed: " << strerror(errno) << RESET << std::endl;
        exit(EXIT_FAILURE);
    }

    // Configuring the address struct of the socket
    _address.sin_family = AF_INET; // address family
    _address.sin_addr.s_addr = INADDR_ANY; // accepts connections from any IP on the host
    _address.sin_port = htons(PORT); // ensures the port number is correctly formatted

    // Binding the socket to the address and the port
    if (bind(_serverSocket, reinterpret_cast<struct sockaddr*>(&_address), addrLen) < 0)
    {
        std::cerr << RED << "Bind failed: " << strerror(errno) << RESET << std::endl;
        close(_serverSocket);
        exit(EXIT_FAILURE);
    }

    // Socket starts listening mode
    if (listen(_serverSocket, MAX_CLIENTS) < 0)
    {
        std::cerr << RED << "Listen failed: " << strerror(errno) << RESET << std::endl;
        close(_serverSocket);
        exit(EXIT_FAILURE);
    }

    // Add the server socket to the poll file descriptor set
    serverFd.fd = _serverSocket;
    serverFd.events = POLLIN;
    _pollFds.push_back(serverFd);
}

void    Server::createPollLoop()
{
        while (true)
        {
            int pollSize = poll(_pollFds.data(), _pollFds.size(), -1);

            if (pollSize == -1)
            {
                std::cerr << RED << "Poll failed: " << strerror(errno) << RESET << std::endl;
                close(_serverSocket);
                exit(EXIT_FAILURE);
            }

            for (size_t i = 0; i < _pollFds.size(); ++i)
            {
                if (_pollFds[i].revents & POLLIN)
                {
                    if (_pollFds[i].fd == _serverSocket)
                        acceptConnection();
                    else
                        handleClientData(i);
                }
                else if (_pollFds[i].revents & POLLOUT)
                    sendClientData(i);
        }
    }
}

void    Server::acceptConnection()
{
    int newSocket;
    int addrLen = sizeof(_address);

    // Accept new connection
    if ((newSocket = accept(_serverSocket, reinterpret_cast<struct sockaddr*>(&_address), reinterpret_cast<socklen_t*>(&addrLen))) < 0)
        std::cerr << RED << "Accept failed: " << strerror(errno) << RESET << std::endl;
    else
    {
        std::cout << GREEN << "New connection, socket fd is " << newSocket << ", ip is : " << inet_ntoa(_address.sin_addr) << ", port : " << ntohs(_address.sin_port) << RESET << std::endl;

        // Add new client socket to the poll file descriptor set
        struct pollfd clientFd;
        clientFd.fd = newSocket;
        clientFd.events = POLLIN & POLLOUT;
        _pollFds.push_back(clientFd);

        // Initialize client
        addClient(newSocket);
    }
}

void    Server::handleClientData(size_t index)
{
    char buffer[BUFFER_SIZE];
    int bytesRead = read(_pollFds[index].fd, buffer, BUFFER_SIZE);

    if (bytesRead <= 0)
    {
        if (bytesRead < 0)
            std::cerr << RED << "Error reading from client socket: " << strerror(errno) << RESET << std::endl;
        else
            std::cout << YELLOW << "Client disconnected, socket fd is " << _pollFds[index].fd << RESET << std::endl;
        // close(_pollFds[*index].fd);
        // removeClient(_pollFds[*index].fd);
        // _pollFds.erase(_pollFds.begin() + (*index));
        // --(*index);
    }
    else
    {
        getClient(_pollFds[index].fd).addToBuffer(buffer);
        // How to check if all the reading is done and where?

    }

}

void    Server::sendClientData(size_t index)
{
    
}


void Server::addClient(int fd)
{
    _clients.emplace(fd, Client(fd));
}

Client& Server::getClient(int fd)
{
    return (_clients.at(fd));
}

void Server::removeClient(int fd)
{
    _clients.erase(fd);
}

