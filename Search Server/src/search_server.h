#pragma once

#include <algorithm>
#include <cmath>
#include <execution>
#include <functional>
#include <future>
#include <map>
#include <mutex>
#include <set>
#include <stdexcept>
#include <string>
#include <string_view>
#include <thread>
#include <tuple>
#include <type_traits>
#include <utility>
#include <vector>

#include "concurrent_map.h"
#include "document.h"

const int kMaxResultDocumentCount = 5;

class SearchServer {
public:
    template<typename StringContainer>
    explicit SearchServer(const StringContainer &stop_words);

    explicit SearchServer(const std::string &stop_words_text);

    explicit SearchServer(std::string_view stop_words_text);

public:
    void AddDocument(int document_id, std::string_view document, DocumentStatus status,
        const std::vector<int> &ratings);

    template<class ExecutionPolicy, typename KeyMapper>
    std::vector<Document> FindTopDocuments(ExecutionPolicy &&policy, std::string_view raw_query,
        KeyMapper user_predicate) const;

    template<class ExecutionPolicy>
    std::vector<Document> FindTopDocuments(ExecutionPolicy &&policy, std::string_view raw_query,
        DocumentStatus requested_status = DocumentStatus::ACTUAL) const;

    template<typename KeyMapper>
    std::vector<Document> FindTopDocuments(std::string_view raw_query, KeyMapper user_predicate) const;

    std::vector<Document> FindTopDocuments(std::string_view raw_query,
        DocumentStatus requested_status = DocumentStatus::ACTUAL) const;

    int GetDocumentCount() const;

    const std::map<std::string_view, double>& GetWordsFrequencies(int document_id) const;

    template<class ExecutionPolicy>
    std::tuple<std::vector<std::string_view>, DocumentStatus> MatchDocument(ExecutionPolicy &&policy,
        std::string_view raw_query, int document_id) const;

    std::tuple<std::vector<std::string_view>, DocumentStatus> MatchDocument(std::string_view raw_query,
        int document_id) const;

    template<class ExecutionPolicy>
    void RemoveDocument(ExecutionPolicy &&policy, int id);

    void RemoveDocument(int id);

    const std::set<int, std::less<int>, std::allocator<int>>::iterator begin() const;

    const std::set<int, std::less<int>, std::allocator<int>>::iterator end() const;

private:
    struct DocumentData {
        int rating = 0;
        DocumentStatus status = DocumentStatus::ACTUAL;
    };

    struct Query {
        std::set<std::string_view> plus_words;
        std::set<std::string_view> minus_words;
    };

    struct QueryWord {
        std::string_view data;
        bool is_minus = false;
        bool is_stop = false;
    };

private:
    static const double constexpr kEpsilon = 1e-6;

private:
    template<class ExecutionPolicy, typename Iterator, typename Func>
    void ExecuteIfWordInRangeExists(ExecutionPolicy &&policy, Iterator left, Iterator right,
        Func heavy_executor) const;

    template<class ExecutionPolicy, typename KeyMapper>
    std::vector<Document> FindAllDocuments(ExecutionPolicy &&policy, const Query &query,
        KeyMapper predicate) const;

    template<typename KeyMapper>
    std::vector<Document> FindAllDocuments(const Query &query, KeyMapper predicate) const;

    template<typename StringContainer>
    std::set<std::string, std::less<>> MakeUniqueNonEmptyStrings(const StringContainer &strings) const;

    int ComputeAverageRating(const std::vector<int> &ratings) const;

    double ComputeWordInverseDocumentFreq(
        const std::map<std::string, std::map<int, double>, std::less<>>::const_iterator word_position) const;

    bool IsStopWord(std::string_view word) const;

    static bool IsValidWord(std::string_view word);

    std::vector<std::string_view> SplitIntoWords(std::string_view text) const;

    std::vector<std::string_view> SplitIntoWordsNoStop(std::string_view text) const;

    std::string StringViewToString(std::string_view str_view) const;

    Query ParseQuery(std::string_view text) const;

    QueryWord ParseQueryWord(std::string_view word) const;

private:
    std::map<int, DocumentData> documents_;
    std::set<int> documents_ids_;

    std::set<std::string, std::less<>> stop_words_;

    std::map<std::string, std::map<int, double>, std::less<>> word_to_document_freqs_;
    std::map<int, std::map<std::string_view, double>> document_to_word_freqs_;
};

//class public-methods implementations:
template<typename StringContainer>
SearchServer::SearchServer(const StringContainer &stop_words) : stop_words_(MakeUniqueNonEmptyStrings(stop_words)) {
    if (std::any_of(std::execution::seq, stop_words_.begin(), stop_words_.end(),
        [](const auto &word) {
            return !IsValidWord(word);
        })) {
        throw std::invalid_argument("Something is wrong with symbols");
    }
}

template<class ExecutionPolicy, typename KeyMapper>
std::vector<Document> SearchServer::FindTopDocuments(ExecutionPolicy &&policy, std::string_view raw_query,
    KeyMapper user_predicate) const {
    Query query = ParseQuery(raw_query);
    auto matched_documents = FindAllDocuments(policy, query, user_predicate);

    std::sort(policy, matched_documents.begin(), matched_documents.end(),
        [](const Document &lhs, const Document &rhs) {
            if (std::abs(lhs.relevance - rhs.relevance) < kEpsilon) {
                return lhs.rating > rhs.rating;
            } else {
                return lhs.relevance > rhs.relevance;
            }
        });

    if (matched_documents.size() > kMaxResultDocumentCount) {
        matched_documents.resize(kMaxResultDocumentCount);
    }
    return matched_documents;
}

template<class ExecutionPolicy>
std::vector<Document> SearchServer::FindTopDocuments(ExecutionPolicy &&policy, std::string_view raw_query,
    DocumentStatus requested_status) const {
    return FindTopDocuments(policy, raw_query,
        [requested_status](int document_id, DocumentStatus status, int rating) {
            return status == requested_status;
        });
}

template<typename KeyMapper>
std::vector<Document> SearchServer::FindTopDocuments(std::string_view raw_query, KeyMapper user_predicate) const {
    return FindTopDocuments(std::execution::seq, raw_query, user_predicate);
}

template<class ExecutionPolicy>
std::tuple<std::vector<std::string_view>, DocumentStatus> SearchServer::MatchDocument(ExecutionPolicy &&policy,
    std::string_view raw_query, int document_id) const {
    const auto *const doc_position = &document_to_word_freqs_.at(document_id); //can throw exception
    std::vector<std::string_view> matched_words;

    Query query = ParseQuery(raw_query);

    bool minus_word_found = std::any_of(policy, query.minus_words.begin(), query.minus_words.end(),
        [document_id, doc_position](const auto &item) {
            return doc_position->find(item) != doc_position->end();
        }
    );

    if (!minus_word_found) {
        matched_words.reserve(query.plus_words.size());
        std::copy_if(policy, query.plus_words.begin(), query.plus_words.end(), std::back_inserter(matched_words),
            [&](const auto &item) {
                return doc_position->find(item) != doc_position->end();
            });
    }
    return {matched_words, documents_.at(document_id).status};
}

template<class ExecutionPolicy>
void SearchServer::RemoveDocument(ExecutionPolicy &&policy, int id) {
    const auto *word_freqs_itr = &document_to_word_freqs_.at(id); // can throw exception
    std::mutex wait_please;
    auto search_and_destroy = [&](const auto &word_freq) {
        auto erase_pos = word_to_document_freqs_.find(word_freq.first);
        auto *other_documents_with_word = &erase_pos->second;
        if (other_documents_with_word->size() == 1) {
            std::lock_guard pause(wait_please);
            word_to_document_freqs_.erase(erase_pos);
        } else {
            other_documents_with_word->erase(id);
        }
    };
    std::for_each(policy, word_freqs_itr->begin(), word_freqs_itr->end(), search_and_destroy);
    document_to_word_freqs_.erase(id);
    documents_.erase(id);
    documents_ids_.erase(id);
}

//class private-methods implementations:
template<class ExecutionPolicy, typename Iterator, typename Func>
void SearchServer::ExecuteIfWordInRangeExists(ExecutionPolicy &&policy, Iterator left, Iterator right,
    Func heavy_executor) const {
    std::vector<std::future<void>> futures { };
    futures.reserve(std::distance(left, right));
    auto IfWordExists = [&](const auto &word) {
        const auto id_freq_at_word_ptr = word_to_document_freqs_.find(word);
        if (id_freq_at_word_ptr != word_to_document_freqs_.end()) {
            futures.push_back(std::async(heavy_executor, id_freq_at_word_ptr));
        }
    };
    std::for_each(policy, left, right, IfWordExists);
}

template<class ExecutionPolicy, typename KeyMapper>
std::vector<Document> SearchServer::FindAllDocuments(ExecutionPolicy &&policy, const Query &query,
    KeyMapper predicate) const {
    unsigned int thread_count = 2u;
    if constexpr (std::is_same_v<std::decay_t<ExecutionPolicy>, std::execution::parallel_policy>) {
        thread_count = std::thread::hardware_concurrency();
    }

    ConcurrentMap<int, double> black_list_parallel(thread_count);
    ConcurrentMap<int, double> white_list_parallel(thread_count);

    auto add_id_to_black_list = [&](const auto id_freq_at_word_ptr) {
        for (const auto& [document_id, _] : id_freq_at_word_ptr->second) {
            black_list_parallel[document_id];
        }
    };
    ExecuteIfWordInRangeExists(policy, query.minus_words.begin(), query.minus_words.end(), add_id_to_black_list);
    const auto black_list = black_list_parallel.BuildOrdinaryMap();

    auto if_predicate = [&](const auto id_freq_at_word_ptr) {
        const double inverse_document_freq = ComputeWordInverseDocumentFreq(id_freq_at_word_ptr);
        for (const auto& [document_id, term_freq] : id_freq_at_word_ptr->second) {
            if (!black_list.count(document_id)
                && predicate(document_id, documents_.at(document_id).status,
                    documents_.at(document_id).rating)) {
                white_list_parallel[document_id].ref_to_value += term_freq * inverse_document_freq;
            }
        }
    };
    ExecuteIfWordInRangeExists(policy, query.plus_words.begin(), query.plus_words.end(), if_predicate);
    const auto document_to_relevance = white_list_parallel.BuildOrdinaryMap();

    std::vector<Document> matched_documents(document_to_relevance.size());
    std::transform(policy, document_to_relevance.begin(), document_to_relevance.end(),
        matched_documents.begin(),
        [&](const auto &match) {
            return Document(match.first, match.second, documents_.at(match.first).rating);
        });

    return matched_documents;
}

template<typename KeyMapper>
std::vector<Document> SearchServer::FindAllDocuments(const Query &query, KeyMapper predicate) const {
    return FindAllDocuments(std::execution::seq, query, predicate);
}

template<typename StringContainer>
std::set<std::string, std::less<>> SearchServer::MakeUniqueNonEmptyStrings(const StringContainer &strings) const {
    std::set<std::string, std::less<>> non_empty_strings;
    for (auto &str : strings) {
        if (str.size() != 0u) {
            non_empty_strings.emplace(StringViewToString(str));
        }
    }
    return non_empty_strings;
}
