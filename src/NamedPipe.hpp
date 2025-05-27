/*
** EPITECH PROJECT, 2025
** La-Pizza-Delamama
** File description:
** NamedPipe
*/

#ifndef SRC_NAMEDPIPE_HPP_
#define SRC_NAMEDPIPE_HPP_
#include <string>

class NamedPipe {
 public:
    explicit NamedPipe(const std::string &pipePath);
    ~NamedPipe() noexcept(false);
    operator const char *() const;
    const std::string &getPipePath() const;
    std::string readString();
    void writeString(const std::string &data);

 protected:
 private:
    std::string _pipePath;
    std::string _readBuffer;
    std::string getLineFromReadBuffer();
    void tryClose(int fd) const;
    int tryOpen(int mode) const;
};

#endif  // SRC_NAMEDPIPE_HPP_
