#pragma once

#include "Argument.h"

#include <string>
#include <vector>
#include <unordered_map>

namespace ArgumentParser {

using StringArgument = Argument<std::string>;
using IntArgument = Argument<int>;
using Flag = Argument<bool>;

class ArgParser {
private:
    std::vector<StringArgument> string_arguments;
    std::vector<IntArgument> int_arguments;
    std::vector<Flag> flags;
    
    std::string parser_description;
    bool show_help = false;
    bool have_help = false;
    
    StringArgument help;
    
public:
    ArgParser() = default;
    explicit ArgParser(std::string description) : parser_description(std::move(description)) {}
    
    IntArgument& AddIntArgument(char short_name, const std::string& long_name, const std::string& description);
    StringArgument& AddStringArgument(char short_name, const std::string& long_name, const std::string& description);
    Flag& AddFlag(char short_name, const std::string& long_name, const std::string& description);

    IntArgument& AddIntArgument(const std::string& long_name, const std::string& description) { return AddIntArgument('\0', long_name, description); }
    StringArgument& AddStringArgument(const std::string& long_name, const std::string& description) { return AddStringArgument('\0', long_name, description); }
    Flag& AddFlag(const std::string& long_name, const std::string& description) { return AddFlag('\0', long_name, description); }

    IntArgument& AddIntArgument(const std::string& long_name) { return AddIntArgument('\0', long_name, ""); }
    StringArgument& AddStringArgument(const std::string& long_name) { return AddStringArgument('\0', long_name, ""); }
    Flag& AddFlag(const std::string& long_name) { return AddFlag('\0', long_name, ""); }

    IntArgument& AddIntArgument(char short_name, const std::string& long_name) { return AddIntArgument(short_name, long_name, ""); }
    StringArgument& AddStringArgument(char short_name, const std::string& long_name) { return AddStringArgument(short_name, long_name, ""); }
    Flag& AddFlag(char short_name, const std::string& long_name) { return AddFlag(short_name, long_name, ""); }

    void AddHelp(char short_name, const std::string& long_name, const std::string& description);
    void AddHelp(const std::string& long_name, const std::string& description) { AddHelp('\0', long_name, description); }
    void AddHelp(const std::string& long_name) { AddHelp('\0', long_name, ""); }

    bool Parse(int argc, char* argv[]);
    bool Parse(const std::vector<std::string>& args);

    [[nodiscard]] bool GetFlag(const std::string& long_name, int index = -1) const;
    [[nodiscard]] int GetIntValue(const std::string& long_name, int index = -1) const;
    [[nodiscard]] std::string GetStringValue(const std::string& long_name, int index = -1) const;

    [[nodiscard]] std::string HelpDescription() const;
    [[nodiscard]] bool Help() const { return show_help; }
};

}