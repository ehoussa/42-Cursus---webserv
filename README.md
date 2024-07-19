# Webserv Project

The Webserv project at 42 Network involves creating a simplified HTTP web server capable of handling HTTP requests, serving static files, and supporting basic CGI scripts.

## Table of Contents

- [Project Overview](#project-overview)
- [Project Structure](#project-structure)
- [Usage](#usage)
- [Features](#features)
- [Configuration](#configuration)
- [Example Usage](#example-usage)
- [Contributing](#contributing)
- [License](#license)

## Project Overview

The main goal of the project is to implement a basic HTTP web server that can handle incoming HTTP requests, serve static content (HTML, CSS, images), and execute CGI scripts for dynamic content generation.

## Project Structure

The project typically consists of the following files:

- **webserv**: Executable file for the web server.
- **main.cpp**: Contains the main logic for initializing the server, accepting connections, and handling requests.
- **Server.hpp/Server.cpp**: Classes for configuring and managing server instances.
- **Request.hpp/Request.cpp**: Classes for parsing incoming HTTP requests.
- **Response.hpp/Response.cpp**: Classes for generating HTTP responses.
- **Config.hpp/Config.cpp**: Classes for parsing and managing server configuration (e.g., `webserv.conf` file).
- **cgi-bin/**: Directory for storing CGI scripts and executable files.
- **htdocs/**: Directory for storing static content to be served by the web server.
- **Makefile**: Defines compilation rules and dependencies for building the `webserv` executable.

## Usage

To compile and run the `webserv` program, follow these steps:

1. **Clone the repository:**

   ```bash
   git clone https://github.com/ehoussa/42-Cursus---webserv/
   cd 42-Cursus---webserv
   make
   ./webserv path_to_configuration_file.conf

Replace path_to_configuration_file.conf with the path to your server configuration file (webserv.conf) containing server settings and rules.

# Features

The webserv program typically supports the following features:

    HTTP/1.1 Protocol: Handles HTTP requests and responses conforming to the HTTP/1.1 specification.
    Static File Serving: Serves static files (HTML, CSS, images) from a specified directory (htdocs).
    CGI Support: Executes CGI scripts located in the cgi-bin directory for dynamic content generation.
    Virtual Hosting: Supports multiple virtual hosts (domains) with separate configurations.
    Logging: Logs server activity, requests, and errors to a specified log file.
    Error Handling: Handles common HTTP errors (404 Not Found, 500 Internal Server Error, etc.).

# Contributing

Contributions are welcome! Please follow these steps to contribute:

    Fork the repository
    Create a new branch (git checkout -b feature/YourFeature)
    Commit your changes (git commit -am 'Add new feature')
    Push to the branch (git push origin feature/YourFeature)
    Create a new Pull Request

# License

This project is licensed under the MIT License. See the LICENSE file for details.

   ```bash
   This Markdown template provides a structured overview of the "webserv" project, including its purpose, structure, usage instructions, features, configuration details, example usage, contribution guidelines, and licensing information. It is formatted to be easily readable and navigable on platforms like GitHub. Adjust the placeholders (`yourusername`, `path_to_configuration_file.conf`, etc.) with your actual details and paths before using it in your repository.







