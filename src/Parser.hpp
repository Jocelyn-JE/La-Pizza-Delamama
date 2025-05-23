/*
** EPITECH PROJECT, 2025
** La-Pizza-Delamama
** File description:
** Parser
*/

#ifndef SRC_PARSER_HPP_
#define SRC_PARSER_HPP_
#include <string>
#include <vector>

class Parser {
 public:
    Parser() = delete;
    Parser(int argc, char **argv);
    ~Parser() = default;
    bool parse();
    double getCookingMultiplier() const;
    unsigned int getCookNb() const;
    unsigned int getRestockTime() const;

 private:
    bool printUsage() const;
    double tryConvertDouble(const std::string &arg);
    unsigned int tryConvertUnsignedInt(const std::string &arg);

    int _argc;
    std::vector<std::string> _argv;
    double _cookingMultiplier;
    unsigned int _cookNb;
    unsigned int _restockTime;

    class ParserException : public std::exception {
     public:
        explicit ParserException(const std::string &errmsg);
        const char *what() const noexcept override;

     private:
        std::string _errmsg = "No error message specified";
    };
};

#endif  // SRC_PARSER_HPP_
