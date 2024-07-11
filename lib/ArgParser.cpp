#pragma once

#include "ArgParser.h"

ArgumentParser::IntArgument& ArgumentParser::ArgParser::AddIntArgument(char short_name, const std::string& long_name, const std::string& description) {
    return int_arguments.emplace_back(short_name, long_name, description);
}

ArgumentParser::StringArgument& ArgumentParser::ArgParser::AddStringArgument(char short_name, const std::string& long_name, const std::string& description) {
    return string_arguments.emplace_back(short_name, long_name, description);
}

ArgumentParser::Flag& ArgumentParser::ArgParser::AddFlag(char short_name, const std::string& long_name, const std::string& description) {
    return flags.emplace_back(short_name, long_name, description).Default(false);
}

void ArgumentParser::ArgParser::AddHelp(char short_name, const std::string& long_name, const std::string& description) {
    have_help = true;
    help = StringArgument(short_name, long_name, description);
}

bool ArgumentParser::ArgParser::Parse(int argc, char** argv) {
    std::vector<std::string> args;
    for (int i = 1; i < argc; ++i) {
        args.emplace_back(argv[i]);
    }
    return Parse(args);
}

bool ArgumentParser::ArgParser::Parse(const std::vector<std::string>& args) {
    for (int i = 1; i < args.size(); i++) {
        std::string argument = args[i];
        size_t pos = argument.find('=');
        std::string value;
        if (pos != std::string::npos) {
            value = argument.substr(pos + 1);
            argument = argument.substr(0, pos);
        }

        if (argument[0] == '-') {
            if (argument[1] == '-') {
                argument = argument.substr(2);

                if (have_help && argument == help.LongName()) {
                    this->show_help = true;
                    return true;
                }

                for (auto& arg : string_arguments) {
                    if (arg.LongName() == argument) {
                        if (value.empty()) {
                            if (i + 1 < args.size()) {
                                value = args[++i];
                            } else {
                                return false;
                            }
                        }
                        arg.SetValue(value);
                    }
                }
                for (auto& arg : int_arguments) {
                    if (arg.LongName() == argument) {
                        if (value.empty()) {
                            if (i + 1 < args.size()) {
                                value = args[++i];
                            } else {
                                return false;
                            }
                        }
                        arg.SetValue(std::stoi(value));
                    }
                }

                for (auto& arg : flags) {
                    if (arg.LongName() == argument) {
                        arg.SetValue(true);
                    }
                }
            } else {
                argument = argument.substr(1);

                if (have_help && help.ShortName() == argument[0]) {
                    show_help = true;
                    return true;
                }

                for (auto& flag_argument : flags) {
                    for (char c : argument) {
                        if (flag_argument.ShortName() == c) {
                            flag_argument.SetValue(true);
                        }
                    }
                }

                for (auto& int_argument : int_arguments) {
                    if (int_argument.ShortName() == argument[0]) {
                        if (value.empty()) {
                            if (i + 1 < args.size()) {
                                value = args[i + 1];
                                i++;
                            } else {
                                return false;
                            }
                        } else {
                            int_argument.SetValue(std::stoi(value));
                        }
                        break;
                    }
                }

                for (auto& string_argument : string_arguments) {
                    if (string_argument.ShortName() == argument[0]) {
                        if (value.empty()) {
                            if (i + 1 < args.size()) {
                                value = args[i + 1];
                                i++;
                            } else {
                                return false;
                            }
                        } else {
                            string_argument.SetValue(value);
                        }
                        break;
                    }
                }
            }
        } else {
            for (auto& arg : string_arguments) {
                if (arg.IsPositional()) {
                    arg.SetValue(argument);
                }
            }
            for (auto& arg : int_arguments) {
                if (arg.IsPositional()) {
                    arg.SetValue(std::stoi(argument));
                }
            }
        }
    }

    for (auto arg : string_arguments) {
        if (!arg.IsValid()) {
            return false;
        }
    }

    for (auto arg : int_arguments) {
        if (!arg.IsValid()) {
            return false;
        }
    }

    for (auto arg : flags) {
        if (!arg.IsValid()) {
            return false;
        }
    }

    return true;
}

bool ArgumentParser::ArgParser::GetFlag(const std::string& long_name, int index) const {
    for (const auto& flag : flags) {
        if (flag.LongName() == long_name) {
            return flag.GetValue(index);
        }
    }
    return false;
}

int ArgumentParser::ArgParser::GetIntValue(const std::string& long_name, int index) const {
    for (const auto& int_argument : int_arguments) {
        if (int_argument.LongName() == long_name) {
            return int_argument.GetValue(index);
        }
    }
    return 0;
}

std::string ArgumentParser::ArgParser::GetStringValue(const std::string& long_name, int index) const {
    for (const auto& string_argument : string_arguments) {
        if (string_argument.LongName() == long_name) {
            return string_argument.GetValue(index);
        }
    }
    return "";
}

std::string ArgumentParser::ArgParser::HelpDescription() const {
    std::string help_description = parser_description + '\n';
    if (have_help) {
        help_description += help.HelpDescription() + '\n';
    }

    help_description += "String arguments:\n";
    for (const auto& string_argument : string_arguments) {
        help_description += string_argument.HelpDescription() + '\n';
    }
    help_description += "Int arguments:\n";
    for (const auto& int_argument : int_arguments) {
        help_description += int_argument.HelpDescription() + '\n';
    }
    help_description += "Flags:\n";
    for (const auto& flag : flags) {
        help_description += flag.HelpDescription() + '\n';
    }

    return help_description;
}
