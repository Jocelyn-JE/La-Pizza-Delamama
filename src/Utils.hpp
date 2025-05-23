/*
** EPITECH PROJECT, 2025
** La-Pizza-Delamama
** File description:
** Utils
*/

#ifndef SRC_UTILS_HPP_
#define SRC_UTILS_HPP_
#include <string>
#include <vector>

namespace utils {
// Function to split a string by a delimiter
std::vector<std::string> split(
    const std::string &str, const std::string &delimiter);

// Function to validate pizza type
bool isValidPizzaType(const std::string &type);

// Function to validate pizza size
bool isValidPizzaSize(const std::string &size);

// Function to validate pizza count
bool isValidPizzaCount(const std::string &count);

// Function to get the lowercase version of a string
std::string toLower(const std::string &str);

// Function to get the uppercase version of a string
std::string toUpper(const std::string &str);

// Function to check if a string is a positive whole number
bool isNumber(const std::string &str);

}  // namespace utils

#endif  // SRC_UTILS_HPP_
