#include "json.h"

#include <utility>

using namespace std;

namespace json {

namespace {

void CheckCommand(std::istream &input, const std::string &expected) {
    for (const char c_expected : expected) {
        if (!input) {
            throw ParsingError("invalid operand: " + expected);
        }
        char c_input = 0;
        input >> c_input;
        if (c_input != c_expected) {
            throw ParsingError("invalid operand: " + expected);
        }
    }
}

Node LoadNode(istream &input);

Node LoadArray(istream &input) {
    Array result;
    char c;
    while (input >> c) {
        if (c == ']') {
            return Node(move(result));
        }
        if (c != ',') {
            input.putback(c);
        }
        result.push_back(LoadNode(input));
    }
    throw json::ParsingError("load array fail"s);
    return {};
}

Node LoadOperand(std::istream &input) {
    if (input.peek() == 'n') {
        CheckCommand(input, "null"s);
        return Node { };
    } else if (input.peek() == 't') {
        CheckCommand(input, "true"s);
        return Node { true };
    } else if (input.peek() == 'f') {
        CheckCommand(input, "false"s);
        return Node { false };
    } else {
        throw ParsingError("Unknown operand"s);
    }
    return {};
}

Node LoadNumber(std::istream &input) {
    using namespace std::literals;

    std::string parsed_num;

    auto read_char = [&parsed_num, &input] {
        parsed_num += static_cast<char>(input.get());
        if (!input) {
            throw ParsingError("Failed to read number from stream"s);
        }
    };

    auto read_digits = [&input, read_char] {
        if (!std::isdigit(input.peek())) {
            throw ParsingError("A digit is expected"s);
        }
        while (std::isdigit(input.peek())) {
            read_char();
        }
    };

    if (input.peek() == '-') {
        read_char();
    }
    if (input.peek() == '0') {
        read_char();
    } else {
        read_digits();
    }

    bool is_int = true;
    if (input.peek() == '.') {
        read_char();
        read_digits();
        is_int = false;
    }

    if (int ch = input.peek(); ch == 'e' || ch == 'E') {
        read_char();
        if (ch = input.peek(); ch == '+' || ch == '-') {
            read_char();
        }
        read_digits();
        is_int = false;
    }

    try {
        if (is_int) {
            try {
                return Node(std::stoi(parsed_num));
            } catch (...) {
            }
        }
        return Node(std::stod(parsed_num));
    } catch (...) {
        throw ParsingError("Failed to convert "s + parsed_num + " to number"s);
    }
}

Node LoadString(istream &input) {

    std::noskipws(input);
    string line;
    char c = 0;
    while (input >> c) {
        if (c == '"') {
            std::skipws(input);
            return Node(move(line));
        }
        if (c == '\\') {
            char next = input.peek();
            if (next == '"' || next == '\\') {
                line.push_back(next);
            } else if (next == 'n') {
                line.push_back('\n');
            } else if (next == 'r') {
                line.push_back('\r');
            } else if (next == 't') {
                line.push_back('\t');
            }
            input.get();
        } else {
            line.push_back(c);
        }
    }

    throw ParsingError("string parsing failed - no ending \" - func LoadString():node"s);
    return {};
}

Node LoadDict(istream &input) {
    Dict result;
    std::skipws(input);
    char c = 0;
    while (input >> c) {
        if (c == '}') {
            return Node(move(result));
        }
        if (c == ',') {
            input >> c;
        }
        string key = LoadString(input).AsString();
        input >> c;
        result.insert( { move(key), LoadNode(input) });
    }

    throw json::ParsingError("load map fail"s);
    return {};
}

Node LoadNode(istream &input) {
    char c;
    input >> c;

    if (c == '[') {
        return LoadArray(input);
    } else if (c == '{') {
        return LoadDict(input);
    } else if (c == '"') {
        return LoadString(input);
    } else if (std::isdigit(c) || (c == '-' && std::isdigit(input.peek()))) {
        input.putback(c);
        return LoadNumber(input);
    } else if (c == 'n' || c == 't' || c == 'f') { //none | true | false
        input.putback(c);
        return LoadOperand(input);
    }
    else {
        throw ParsingError("unknown format - func LoadNode"s);
    }
}

}  // namespace

Document Load(istream &input) {
    return Document { LoadNode(input) };
}

bool Node::IsArray() const {
    return holds_alternative<Array>(*this);
}

bool Node::IsBool() const {
    return holds_alternative<bool>(*this);
}
bool Node::IsDouble() const {
    return holds_alternative<double>(*this) || IsInt();
}

bool Node::IsInt() const {
    return holds_alternative<int>(*this);
}

bool Node::IsDict() const {
    return holds_alternative<Dict>(*this);
}

bool Node::IsNull() const {
    return holds_alternative<nullptr_t>(*this);
}

bool Node::IsPureDouble() const {
    return IsDouble() && !IsInt();
}

bool Node::IsString() const {
    return holds_alternative<std::string>(*this);
}

const Array& Node::AsArray() const {
    if (!IsArray()) {
        throw std::logic_error("not array");
    }
    return std::get<Array>(*this);
}

const Dict& Node::AsDict() const {
    if (!IsDict()) {
        throw std::logic_error("not map");
    }
    return std::get<Dict>(*this);
}

bool Node::AsBool() const {
    if (!IsBool()) {
        throw std::logic_error("not bool");
    }
    return std::get<bool>(*this);
}

double Node::AsDouble() const {
    if (IsInt()) {
        return static_cast<double>(std::get<int>(*this));
    }
    if (!IsDouble()) {
        throw std::logic_error("not double");
    }
    return std::get<double>(*this);
}

int Node::AsInt() const {
    if (!IsInt()) {
        throw std::logic_error("not int");
    }
    return std::get<int>(*this);
}

const string& Node::AsString() const {
    if (!IsString()) {
        throw std::logic_error("not string");
    }
    return std::get<std::string>(*this);
}

const JsonNode& Node::AsVariant() const {
    return *this;
}

void Node::Print(ostream &out) const {
    visit(PrintNode { out }, static_cast<JsonNode>(*this));
}

void PrintNode::operator()(std::nullptr_t) {
    out << "null";
}

void PrintNode::operator()(const Array &arr) {
    bool indent_increased = false;
    if (!arr.empty() && arr.begin()->IsDict()) {
        out << '\n';
        PrintIndent();
        indent += indent_step;
        indent_increased = true;
    }

    bool first_element = true;
    out << "["s;
    for (const auto &element : arr) {
        if (!first_element) {
            out << ", "s;
        }
        std::visit(PrintNode { out, indent }, element.AsVariant());
        first_element = false;
    }

    if (indent_increased) {
        indent -= indent_step;
    }
    if (!arr.empty() && arr.rbegin()->IsDict()) {
        out << '\n';
        PrintIndent();
    }

    out << "]"s;
}

void PrintNode::operator()(const Dict &dict) {
    out << '\n';
    PrintIndent();
    out << "{\n"s;
    int comma_count = dict.size() - 1;
    indent += indent_step;
    for (const auto &element : dict) {
        PrintNode printer { out };
        PrintIndent();
        printer(element.first);
        out << ": "s;
        std::visit(PrintNode { out, indent + indent_step }, element.second.AsVariant());
        if (comma_count--) {
            out << ',';
        }
        out << '\n';
    }
    indent -= indent_step;
    PrintIndent();
    out << "}";
}

void PrintNode::operator()(bool value) {
    std::boolalpha(out);
    out << value;
}

void PrintNode::operator()(int print) {
    out << print;
}

void PrintNode::operator()(double print) {
    out << print;
}

void PrintNode::operator()(const std::string &print) {
    out << '\"';
    for (const char c : print) {
        if (c == '\"') {
            out << "\\\""s;
        } else if (c == '\n') {
            out << "\\n"s;
        } else if (c == '\r') {
            out << "\\r"s;
        } else if (c == '\t') {
            out << "\\t"s;
        } else if (c == '\\') {
            out << "\\\\"s;
        } else {
            out << c;
        }
    }
    out << '\"';
}

void PrintNode::PrintIndent() {
    for (int i = 0; i < indent; ++i) {
        out << ' ';
    }
}

Document::Document(Node root) : root_(move(root)) {
}

const Node& Document::GetRoot() const {
    return root_;
}

void Print(const Document &doc, std::ostream &output) {
    doc.GetRoot().Print(output);
}

} // namespace json

