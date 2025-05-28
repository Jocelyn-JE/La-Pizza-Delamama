/*
** EPITECH PROJECT, 2025
** La-Pizza-Delamama
** File description:
** NamedPipe
*/

#ifndef SRC_NAMEDPIPE_HPP_
#define SRC_NAMEDPIPE_HPP_
#include <fcntl.h>

#include <cstring>
#include <string>

class NamedPipe {
 public:
    explicit NamedPipe(const std::string &pipePath);
    ~NamedPipe() noexcept(false);
    operator const char *() const;
    const std::string &getPipePath() const;

    std::string readString();
    void writeString(const std::string &data);

    template <typename T>
    bool write(const T &data) {
        int fd = tryOpen(O_WRONLY);
        ssize_t bytesWritten = write(fd, &data, sizeof(T));
        tryClose(fd);
        return bytesWritten == sizeof(T);
    }

    template <typename T>
    bool read(T &data) {
        int fd = tryOpen(O_RDONLY);
        ssize_t bytesRead = read(fd, &data, sizeof(T));
        tryClose(fd);
        return bytesRead == sizeof(T);
    }

    bool writePizza(const class Pizza &pizza);
    bool readPizza(class Pizza &pizza);

 protected:
 private:
    std::string _pipePath;
    std::string _readBuffer;
    std::string getLineFromReadBuffer();
    void tryClose(int fd) const;
    int tryOpen(int mode) const;
};

struct KitchenCommand {
    enum Type {
        STATUS_REQUEST = 1,
        LOAD_REQUEST = 2,
        PIZZA_ORDER = 3,
        SHUTDOWN = 4
    } type;
};

struct KitchenStatus {
    unsigned int busyCooks;
    unsigned int queueSize;

    struct {
        unsigned int dough;
        unsigned int tomato;
        unsigned int gruyere;
        unsigned int ham;
        unsigned int mushrooms;
        unsigned int steak;
        unsigned int eggplant;
        unsigned int goatCheese;
        unsigned int chiefLove;
    } ingredients;
};

struct LoadResponse {
    unsigned int currentLoad;
};

#endif  // SRC_NAMEDPIPE_HPP_
