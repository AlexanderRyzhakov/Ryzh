#pragma once

struct Document {
    Document() = default;
    Document(int id_input, double relevance_input, int rating_input) :
        id(id_input), relevance(relevance_input), rating(rating_input) {
    }
    int id = 0;
    double relevance = 0.0;
    int rating = 0;
};

enum class DocumentStatus {
    ACTUAL,
    IRRELEVANT,
    BANNED,
    REMOVED,
};

