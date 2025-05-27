/*
** EPITECH PROJECT, 2025
** La-Pizza-Delamama
** File description:
** NamedPipe
*/

#include "NamedPipe.hpp"
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include <cstring>
#include <filesystem>
#include <stdexcept>
#include <string>

NamedPipe::NamedPipe(const std::string &pipePath) : _pipePath(pipePath) {
    if (!std::filesystem::is_fifo(_pipePath.c_str()) &&
        !std::filesystem::exists(_pipePath.c_str())) {
    if (mkfifo(_pipePath.c_str(), 0666) == -1)
            throw std::runtime_error(
                "Failed to create named pipe: " + _pipePath + "\n" +
                strerror(errno) + "\n");
    }
}

NamedPipe::~NamedPipe() noexcept(false) {
    if (std::filesystem::is_fifo(_pipePath.c_str()) &&
        unlink(_pipePath.c_str()) == -1) {
        throw std::runtime_error("Failed to remove named pipe: " + _pipePath +
                                 "\n" + strerror(errno) + "\n");
    }
}

NamedPipe::operator const char *() const {
    return _pipePath.c_str();
}

const std::string &NamedPipe::getPipePath() const {
    return _pipePath;
}

std::string NamedPipe::readString() {
    int fd = tryOpen(O_RDONLY);
    std::string lineBuffer;
    char buffer[BUFSIZ];
    ssize_t bytesRead;

    do {
        bytesRead = read(fd, buffer, sizeof(buffer) - 1);
        if (bytesRead == -1) {
            tryClose(fd);
            throw std::runtime_error(
                "Failed to read from named pipe: " + _pipePath + "\n" +
                strerror(errno) + "\n");
        }
        buffer[bytesRead] = '\0';
        lineBuffer += buffer;
    } while (bytesRead > 0 && lineBuffer.find('\n') == std::string::npos);
    tryClose(fd);
    return lineBuffer;
}

void NamedPipe::writeString(const std::string &data) {
    int fd = tryOpen(O_WRONLY);
    ssize_t bytesWritten;

    bytesWritten = write(fd, data.c_str(), data.size());
    tryClose(fd);
    if (bytesWritten == -1) {
        throw std::runtime_error("Failed to write to named pipe: " +
                                 _pipePath + "\n" + strerror(errno) + "\n");
    }
}

void NamedPipe::tryClose(int fd) const {
    if (close(fd) == -1) {
        throw std::runtime_error(
            "Failed to close file descriptor: " + std::to_string(fd) + "\n" +
            strerror(errno) + "\n");
    }
}

int NamedPipe::tryOpen(int mode) const {
    int fd = open(_pipePath.c_str(), mode);

    if (fd == -1) {
        throw std::runtime_error("Failed to open named pipe: " + _pipePath +
                                 "\n" + strerror(errno) + "\n");
    }
    return fd;
}
