#include "search_server.h"

using namespace std;

//class SearchServer - CONSTRUCTORS:
SearchServer::SearchServer(const std::string &stop_words_text) : SearchServer(SplitIntoWords(stop_words_text)) {
}

SearchServer::SearchServer(std::string_view stop_words_text) : SearchServer(SplitIntoWords(stop_words_text)) {
}

//class - SearchServer - PUBLIC METHODS:
void SearchServer::AddDocument(int document_id, string_view document, DocumentStatus status = DocumentStatus::ACTUAL,
    const vector<int> &ratings = { }) {
    if (document_id < 0 || documents_.count(document_id) != 0) {
        throw invalid_argument("Incorrect document ID"s);
    }
    if (!IsValidWord(document)) {
        throw invalid_argument("Document has unacceptable symbols"s);
    }

    const vector<string_view> words = SplitIntoWordsNoStop(document);
    documents_ids_.insert(document_id);
    const double inv_word_count = 1.0 / words.size();
    for (string_view word : words) {
        word_to_document_freqs_[StringViewToString(word)][document_id] += inv_word_count;
        auto word_insert_pos = word_to_document_freqs_.find(word);
        document_to_word_freqs_[document_id][word_insert_pos->first] += inv_word_count;
    }
    documents_.emplace(document_id, DocumentData { ComputeAverageRating(ratings), status });
}

tuple<vector<string_view>, DocumentStatus> SearchServer::MatchDocument(string_view raw_query, int document_id) const {
    return MatchDocument(execution::seq, raw_query, document_id);
}

int SearchServer::GetDocumentCount() const {
    return documents_.size();
}

vector<Document> SearchServer::FindTopDocuments(std::string_view raw_query, DocumentStatus requested_status) const {
    return FindTopDocuments(execution::seq, raw_query,
        [requested_status](int document_id, DocumentStatus status, int rating) {
            return status == requested_status;
        });
}

const map<string_view, double>& SearchServer::GetWordsFrequencies(int document_id) const {
    static const map<string_view, double> dummy;
    if (!documents_ids_.count(document_id)) {
        return dummy;
    }
    return document_to_word_freqs_.at(document_id);
}

void SearchServer::RemoveDocument(int document_id) {
    RemoveDocument(execution::seq, document_id);
}

const set<int, less<int>, allocator<int>>::iterator SearchServer::begin() const {
    return documents_ids_.begin();
}

const set<int, std::less<int>, allocator<int>>::iterator SearchServer::end() const {
    return documents_ids_.end();
}

//class - SearchServer - PRIVATE METHODS:
int SearchServer::ComputeAverageRating(const vector<int> &ratings) const {
    if (ratings.empty()) {
        return 0;
    }
    int rating_sum = 0;
    for (const int rating : ratings) {
        rating_sum += rating;
    }
    return rating_sum / static_cast<int>(ratings.size());
}

double SearchServer::ComputeWordInverseDocumentFreq(
    const map<string, map<int, double>, less<>>::const_iterator word_position) const {
    return log(GetDocumentCount() * 1.0 / word_position->second.size());
}

bool SearchServer::IsStopWord(string_view word) const {
    return stop_words_.count(word);
}

bool SearchServer::IsValidWord(string_view word) {
    return none_of(execution::par, word.begin(), word.end(), [](char c) {
        return c >= '\0' && c < ' ';
    });
}

SearchServer::Query SearchServer::ParseQuery(string_view text) const {
    if (!IsValidWord(text)) {
        throw std::invalid_argument("Bad request");
    }
    Query query = { };
    for (string_view word : SplitIntoWords(text)) {
        const QueryWord query_word = ParseQueryWord(word);
        if (!query_word.is_stop) {
            if (query_word.is_minus) {
                query.minus_words.insert(query_word.data);
            } else {
                query.plus_words.insert(query_word.data);
            }
        }
    }
    return query;
}

SearchServer::QueryWord SearchServer::ParseQueryWord(string_view word) const {
    bool is_minus = (*word.data() == '-') ? true : false;
    is_minus ? word.remove_prefix(1) : word.remove_prefix(0);
    if (word.empty() || (word[0] == '-')) {
        throw invalid_argument("Invalid use of minus in query"s);
    }
    return {word, is_minus, IsStopWord(word)};
}

vector<string_view> SearchServer::SplitIntoWords(string_view text) const {
    vector<string_view> result;
    size_t space = 0u;
    do {
        text.remove_prefix(std::min(text.find_first_not_of(" "), text.size()));
        space = text.find(' ', 0);
        result.push_back(space == text.npos ? text.substr(0) : text.substr(0, space));
        text.remove_prefix(space + 1u);
    } while (space != text.npos);

    return result;
}

vector<string_view> SearchServer::SplitIntoWordsNoStop(string_view text) const {
    vector<string_view> words;
    for (string_view word : SplitIntoWords(text)) {
        if (!IsStopWord(word)) {
            words.push_back(word);
        }
    }
    return words;
}

string SearchServer::StringViewToString(string_view str_view) const {
    return {str_view.data(), str_view.size()};
}
