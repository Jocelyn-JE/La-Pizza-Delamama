/*
** EPITECH PROJECT, 2025
** La-Pizza-Delamama
** File description:
** Utils
*/

#include "Utils.hpp"

#include <iostream>
#include <string>
#include <vector>

#include "plazza/Pizza.hpp"

bool utils::isValidPizzaType(const std::string &type) {
    return plazza::validPizzaTypes.count(type) > 0;
}

bool utils::isValidPizzaSize(const std::string &size) {
    return plazza::validPizzaSizes.count(utils::toUpper(size)) > 0;
}

bool utils::isValidPizzaCount(const std::string &count) {
    if (count.size() < 2 || count[0] != 'x' || count[1] == '0')
        return false;
    for (size_t i = 1; i < count.size(); ++i)
        if (!std::isdigit(count[i]))
            return false;
    return true;
}

std::vector<std::string> utils::split(
    const std::string &str, const std::string &delimiter) {
    std::vector<std::string> tokens;
    size_t start = 0, end = 0;

    while ((end = str.find(delimiter, start)) != std::string::npos) {
        tokens.push_back(str.substr(start, end - start));
        start = end + delimiter.length();
    }
    tokens.push_back(str.substr(start));
    return tokens;
}

std::string utils::toLower(const std::string &str) {
    std::string lowerStr = str;
    for (char &c : lowerStr)
        c = std::tolower(c);
    return lowerStr;
}

std::string utils::toUpper(const std::string &str) {
    std::string upperStr = str;
    for (char &c : upperStr)
        c = std::toupper(c);
    return upperStr;
}

bool utils::isNumber(const std::string &str) {
    for (char c : str) {
        if (!std::isdigit(c))
            return false;
    }
    return true;
}
