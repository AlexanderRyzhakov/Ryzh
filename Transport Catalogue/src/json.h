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
    using Value = variant;

    Node(Value val) : JsonNode(val) {
    }

    bool IsBool() const;
    bool IsInt() const;
    bool IsDouble() const;
    bool IsNull() const;
    bool IsPureDouble() const;

    bool IsArray() const;
    bool IsDict() const;
    bool IsString() const;

    const Array& AsArray() const;
    bool AsBool() const;
    double AsDouble() const;
    int AsInt() const;
    const Dict& AsDict() const;
    const std::string& AsString() const;
    const JsonNode& AsVariant() const;

    void Print(std::ostream &out) const;
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

struct PrintNode {
    void operator()(std::nullptr_t);
    void operator()(const Array&);
    void operator()(const Dict&);
    void operator()(bool);
    void operator()(int);
    void operator()(double);
    void operator()(const std::string&);

    std::ostream &out;
    int indent = 0;
    int indent_step = 2;

private:
    void PrintIndent();
};

Document Load(std::istream &input);

void Print(const Document &doc, std::ostream &output);

} // namespace json
