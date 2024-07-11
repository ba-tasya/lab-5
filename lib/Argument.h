#pragma once

#include <string>
#include <utility>
#include <variant>
#include <vector>

namespace ArgumentParser {

template <typename Type>
class Argument {
private:
    char short_name = '\0';
    std::string long_name;
    std::string description;

    Type default_value;
    std::vector<Type> values_list;

    bool multi_value = false;
    size_t min_size = 0;
    bool positional = false;
    bool has_default = false;

    Type* value_ptr = nullptr;
    std::vector<Type>* value_vector_ptr = nullptr;

public:
    Argument() = default;
    Argument(char short_name, std::string long_name, std::string description) : short_name(short_name), long_name(std::move(long_name)), description(std::move(description)) {}

    Argument& MultiValue(int count = 0) { multi_value = true; min_size = count; return *this; }
    Argument& Positional() { positional = true; return *this; }
    Argument& Default(const Type& value) { default_value = value; has_default = true; return *this; }
    Argument& StoreValue(Type& value) { value_ptr = &value; return *this; }
    Argument& StoreValues(std::vector<Type>& values) { value_vector_ptr = &values; return *this; }
    Argument& SetValue(const Type& value);
    Type GetValue(int index = -1) const;
    std::vector<Type> GetValues() const { return values_list.empty() ? std::vector<Type>(1, default_value) : values_list; }

    [[nodiscard]] char ShortName() const { return short_name; }
    [[nodiscard]] const std::string& LongName() const { return long_name; }
    [[nodiscard]] const std::string& Description() const { return description; }
    [[nodiscard]] bool IsMultiValue() const { return multi_value; }
    [[nodiscard]] size_t MinSize() const { return min_size; }
    [[nodiscard]] bool IsPositional() const { return positional; }
    [[nodiscard]] bool HasDefault() const { return has_default; }

    [[nodiscard]] bool IsValid();
    [[nodiscard]] std::string HelpDescription() const;
};

template<typename Type>
Argument<Type>& Argument<Type>::SetValue(const Type& value) {
    if (value_vector_ptr) {
        value_vector_ptr->push_back(value);
    }

    if (value_ptr) {
        *value_ptr = value;
    }

    values_list.push_back(value);
    return *this;
}

template<typename Type>
bool Argument<Type>::IsValid() {
    if (has_default && values_list.empty()) {
        SetValue(default_value);
    }

    if (values_list.size() < min_size) {
        return false;
    }

    if (value_ptr && multi_value || value_vector_ptr && !multi_value) {
        return false;
    }

    if (!multi_value && values_list.size() > 1) {
        return false;
    }

    if (!has_default && values_list.empty()) {
        return false;
    }

    return true;
}

template<typename Type>
std::string Argument<Type>::HelpDescription() const {
    std::string result = "  ";
    if (short_name != '\0') {
        result += "-";
        result += short_name;
    }
    if (!long_name.empty()) {
        if (short_name != '\0') {
            result += ", ";
        }
        result += "--";
        result += long_name;
    }
    if (multi_value) {
        result += " <value>...";
    } else {
        result += " <value>";
    }
    result += "  ";
    result += description;
    return result;
}

template<typename Type>
Type Argument<Type>::GetValue(int index) const {
    if (index == -1) {
        return values_list.empty() ? default_value : values_list[0];
    } else {
        return values_list[index];
    }
}

}