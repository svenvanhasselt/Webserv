# Webserv

## Table of Contents

- [Introduction](#introduction)
- [Features](#features)
- [Getting Started](#getting-started)
  - [Requirements](#requirements)
  - [How to Build](#how-to-build)
  - [Running the Server](#running-the-server)
- [Configuration](#configuration)
- [Testing](#testing)
- [Project Requirements](#project-requirements)
- [Contributors](#contributors)

---

## Introduction

`Webserv` is a simple web server built for the 42 curriculum. It mimics basic functionalities of real HTTP servers like Nginx, following the HTTP/1.1 protocol. The goal is to understand how servers work and improve C++ programming skills.

---

## Features

- Supports basic HTTP methods: `GET`, `POST`, and `DELETE`.
- Serves static files (HTML, CSS, images, etc.).
- Executes scripts (e.g., PHP or Python) via CGI.
- Reads settings from a configuration file.
- Handles multiple client requests at the same time.
- Sends appropriate error responses (e.g., 404, 500).

---

## Getting Started

### Requirements

- C++17-compatible compiler (e.g., `g++` or `clang++`).
- `make` installed on your system.

### How to Build

1. Clone the repository:
   ```bash
   git clone git@github.com:svenvanhasselt/Webserv.git
   cd Webserv
   ```
2. Build the project:
   ```bash
   make
   ```
3. This will create the `webserv` executable.

### Running the Server

Run the server with a configuration file:

```bash
./webserv config/config_file.conf
```

Example:

```bash
./webserv example.conf
```

To stop the server, press `Ctrl+C`.

---

## Configuration

The server uses a `.conf` file for settings. There is an example you can use located at config/config_file.conf.

### Key Settings

- `listen`: Port number for the server to listen on.
- `server_name`: Server name (e.g., localhost).
- `autoindex`: Enables or disables directory listing.
- `location`: Rules for specific paths.
  - `root`: Directory for files.
  - `index`: Default file to serve.
  - `cgi`: Directory for CGI scripts.
  - `extension`: File extension for CGI scripts.
- `error_page`: Custom pages for specific errors.
- `allowed_methods`: Specifies HTTP methods allowed for this location.

---

## Testing

To test:

1. Start the server with a valid configuration file.
2. Send requests using a browser, `curl`, or `Postman`:
   ```bash
   curl -X GET http://localhost:8080
   ```
3. Test different methods (`GET`, `POST`, `DELETE`) and invalid URLs to check error handling.

For performance testing, use tools like `ab` (Apache Benchmark):

```bash
ab -n 100 -c 10 http://localhost:8080/
```

---

## Project Requirements

- Written in C++.
- Supports HTTP/1.1.
- Reads from a configuration file.
- Handles multiple connections.
- Manages memory and errors properly.
- Follows 42 school coding standards.

---

## Contributors

- [Sven van Hasselt](https://github.com/svenvanhasselt)

---

Feel free to ask questions or report issues on the repository. Happy coding!

