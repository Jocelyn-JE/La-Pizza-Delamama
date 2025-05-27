#ifndef SRC_PLAZZA_IPC_NAMEDPIPE_HPP_
#define SRC_PLAZZA_IPC_NAMEDPIPE_HPP_

#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>

#include <string>

namespace plazza {
namespace ipc {

class NamedPipe {
 public:
    enum Mode {
        READ,
        WRITE
    };

    NamedPipe() = delete;
    NamedPipe(const std::string &name, Mode mode);
    ~NamedPipe();

    template <typename T>
    bool write(const T &data);

    template <typename T>
    bool read(T &data);

    template <typename T>
    NamedPipe &operator<<(const T &data);

    template <typename T>
    NamedPipe &operator>>(T &data);

    void openPipe();

    void close();

    bool isOpen() const {
        return _isOpen;
    }

 private:
    std::string _name;
    int _fd;
    Mode _mode;
    bool _isOpen;
};

template <typename T>
bool NamedPipe::write(const T &data) {
    if (_mode != WRITE || !_isOpen) {
        return false;
    }
    return ::write(_fd, &data, sizeof(T)) == sizeof(T);
}

template <typename T>
bool NamedPipe::read(T &data) {
    if (_mode != READ || !_isOpen) {
        return false;
    }
    return ::read(_fd, &data, sizeof(T)) == sizeof(T);
}

template <typename T>
NamedPipe &NamedPipe::operator<<(const T &data) {
    write(data);
    return *this;
}

template <typename T>
NamedPipe &NamedPipe::operator>>(T &data) {
    read(data);
    return *this;
}

}  // namespace ipc
}  // namespace plazza

#endif  // SRC_PLAZZA_IPC_NAMEDPIPE_HPP_
