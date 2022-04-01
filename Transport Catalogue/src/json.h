#pragma once

#include <iostream>
#include <map>
#include <string>
#include <variant>
#include <vector>

namespace json {

class Node;

using Dict = std::map<std::string, Node>;
using Array = std::vector<Node>;
using JsonNode = std::variant<std::nullptr_t, Array, Dict, bool, int, double, std::string>;

class ParsingError: public std::runtime_error {
public:
    using runtime_error::runtime_error;
};

class Node final : private JsonNode {
public:
    using variant::variant;

    bool IsBool() const;
    bool IsInt() const;
    bool IsDouble() const;
    bool IsNull() const;
    bool IsPureDouble() const;

    bool IsArray() const;
    bool IsMap() const;
    bool IsString() const;

    const Array& AsArray() const;
    bool AsBool() const;
    double AsDouble() const;
    int AsInt() const;
    const Dict& AsMap() const;
    const std::string& AsString() const;
    const JsonNode& AsVariant() const;

    void Print(std::ostream &out, int indent = 0) const;

private:
    void PrintNull(std::ostream &out) const;

    void PrintBool(bool val, std::ostream &out) const;

    template<typename NumberType>
    void PrintNumber(NumberType val, std::ostream &out) const;

    void PrintString(const std::string val, std::ostream &out) const;

    void PrintArray(const Array &arr, std::ostream &out, int indent) const;

    void PrintDict(const Dict &dict, std::ostream &out, int indent) const;

    void PrintIndent(std::ostream &out, int indent) const;
};

class Document {
public:
    explicit Document(Node root);

    bool operator==(const Document &other) const;
    bool operator!=(const Document &other) const;

    const Node& GetRoot() const;

private:
    Node root_;
};

Document Load(std::istream &input);

void Print(const Document &doc, std::ostream &output);

template<typename NumberType>
void Node::PrintNumber(NumberType val, std::ostream &out) const {
    out << val;
}

} // namespace json
