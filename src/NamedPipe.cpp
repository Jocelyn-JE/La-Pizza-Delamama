/*
** EPITECH PROJECT, 2025
** NamedPipe.cpp
** File description:
** _
*/

#include "NamedPipe.hpp"

#include <iostream>
#include <stdexcept>

namespace plazza {
namespace ipc {

NamedPipe::NamedPipe(const std::string &name, Mode mode)
    : _name(name), _fd(-1), _mode(mode), _isOpen(false) {
    std::cout << "Creating named pipe: " << _name << std::endl;
    if (mkfifo(_name.c_str(), 0666) == -1 && errno != EEXIST) {
        throw std::runtime_error("Failed to create named pipe: " + _name);
    }
}

NamedPipe::~NamedPipe() {
    close();
    if (_mode == WRITE) {
        unlink(_name.c_str());
    }
}

void NamedPipe::openPipe() {
    if (_isOpen) {
        std::cerr << "Named pipe is already open: " << _name << std::endl;
        return;
    }

    int flags = (_mode == READ) ? O_RDONLY : O_WRONLY;
    std::cout << "Opening named pipe: " << _name << std::endl;
    _fd = ::open(_name.c_str(), flags);
    std::cout << "Opening named pipe???: " << _name << std::endl;
    if (_fd == -1) {
        throw std::runtime_error("Failed to open named pipe: " + _name);
    }

    _isOpen = true;
}

void NamedPipe::close() {
    if (_isOpen) {
        ::close(_fd);
        _isOpen = false;
    }
}

}  // namespace ipc
}  // namespace plazza
