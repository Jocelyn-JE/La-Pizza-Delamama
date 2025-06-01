/*
** EPITECH PROJECT, 2025
** La-Pizza-Delamama
** File description:
** NamedPipe
*/

#ifndef SRC_NAMEDPIPE_HPP_
#define SRC_NAMEDPIPE_HPP_
#include <fcntl.h>
#include <sys/select.h>
#include <unistd.h>

#include <cstring>
#include <iostream>
#include <string>
#include <thread>

namespace plazza {
class Pizza;
}

class NamedPipe {
 public:
    explicit NamedPipe(const std::string &pipePath);
    ~NamedPipe() noexcept(false);
    operator const char *() const;
    const std::string &getPipePath() const;

    std::string readString();
    void writeString(const std::string &data);

    template <typename T>
    bool writeData(const T &data) {
        const int MAX_RETRIES = 3;
        for (int attempt = 0; attempt < MAX_RETRIES; attempt++) {
            try {
                int fd = tryOpen(O_WRONLY | O_NONBLOCK);
                ssize_t bytesWritten = ::write(fd, &data, sizeof(T));
                tryClose(fd);

                if (bytesWritten == sizeof(T)) {
                    return true;
                } else if (bytesWritten == -1) {
                    if (errno == EAGAIN || errno == EWOULDBLOCK) {
                        std::this_thread::sleep_for(
                            std::chrono::milliseconds(10));
                        continue;
                    }
                    if (attempt == MAX_RETRIES - 1) {
                        std::cerr << "writeData failed after " << MAX_RETRIES
                                  << " attempts: " << strerror(errno)
                                  << std::endl;
                    }
                } else {
                    if (attempt == MAX_RETRIES - 1) {
                        std::cerr << "Warning: writeData partial write "
                                  << bytesWritten << " bytes instead of "
                                  << sizeof(T) << std::endl;
                    }
                }
            } catch (const std::exception &e) {
                if (attempt == MAX_RETRIES - 1) {
                    std::cerr << "Error in writeData: " << e.what()
                              << std::endl;
                }
            }

            std::this_thread::sleep_for(
                std::chrono::milliseconds(50 * (attempt + 1)));
        }
        return false;
    }

    template <typename T>
    bool readData(T &data) {
        try {
            int fd = tryOpen(O_RDONLY | O_NONBLOCK);

            fd_set readfds;
            FD_ZERO(&readfds);
            FD_SET(fd, &readfds);

            struct timeval timeout;
            timeout.tv_sec = 0;
            timeout.tv_usec = 500000;

            int selectResult =
                select(fd + 1, &readfds, nullptr, nullptr, &timeout);

            if (selectResult <= 0) {
                tryClose(fd);
                return false;
            }

            ssize_t bytesRead = ::read(fd, &data, sizeof(T));
            tryClose(fd);

            if (bytesRead == sizeof(T)) {
                return true;
            } else if (bytesRead == -1) {
                if (errno != EAGAIN && errno != EWOULDBLOCK) {
                    std::cerr << "readData error: " << strerror(errno)
                              << std::endl;
                }
            }
            return false;
        } catch (const std::exception &e) {
            return false;
        }
    }

    bool writePizza(const plazza::Pizza &pizza);
    bool readPizza(plazza::Pizza &pizza);

 protected:
 private:
    std::string _pipePath;
    std::string _readBuffer;
    std::string getLineFromReadBuffer();
    void tryClose(int fd) const;
    int tryOpen(int mode) const;
};

// Use packed structures to ensure consistent data layout
#pragma pack(push, 1)

struct KitchenCommand {
    enum Type {
        STATUS_REQUEST = 1,
        LOAD_REQUEST = 2,
        PIZZA_ORDER = 3,
        SHUTDOWN = 4
    } type;

    struct PizzaData {
        int type;
        int size;
    } pizzaData;

    KitchenCommand() : type(STATUS_REQUEST) {
        pizzaData.type = 0;
        pizzaData.size = 0;
    }
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

#pragma pack(pop)

#endif  // SRC_NAMEDPIPE_HPP_
