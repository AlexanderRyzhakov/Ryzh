#pragma once

#include <iostream>
#include <random>
#include <string>
#include <vector>

#include "log_duration.h"
#include "search_server.h"
#include "string_processing.h"
#include "process_queries.h"

#define SPEED_TEST_PROCESS_QUERIES(processor) SpeedTestProcessQueries(#processor, processor, search_server, queries)
#define SPEED_TEST_MATCHING(policy) TestSpeedMatching(#policy, search_server, query, execution::policy)
#define SPEED_TEST_FIND_TOP(policy) SpeedTestFindTop(#policy, search_server, queries, execution::policy)
#define SPPED_TEST_REMOVE(mode) TestSpeedRemove(#mode, search_server, execution::mode)

template<typename Func>
void RunTestImpl(Func f, std::string test_name, bool print) {
    f();
    if (print) {
        std::cerr << test_name << " OK\n";
    }
}

template<typename T, typename U>
void AssertEqualImpl(const T &t, const U &u, const std::string &t_str, const std::string &u_str,
    const std::string &file, const std::string &func, unsigned line, const std::string &hint) {

    if (t != u) {
        std::cerr << file << "(" << line << "): " << func << ": ";
        std::cerr << "ASSERT_EQUAL(" << t_str << ", " << u_str << ") failed: ";
        std::cerr << t << " != " << u << ".";
        if (!hint.empty()) {
            std::cerr << " Hint: " << hint;
        }
        std::cerr << std::endl;
        abort();
    }
}

void AddDocument(SearchServer &search_server, int new_id, const std::string &document, DocumentStatus status =
    DocumentStatus::ACTUAL, std::vector<int> ratings = { });

std::string GenerateWord(std::mt19937 &generator, int max_length);

std::vector<std::string> GenerateDictionary(std::mt19937 &generator, int word_count, int max_length);

std::string GenerateQuery(std::mt19937 &generator, const std::vector<std::string> &dictionary, int word_count,
    double minus_prob = 0);

std::vector<std::string> GenerateQueries(std::mt19937 &generator, const std::vector<std::string> &dictionary,
    int query_count, int max_word_count);

template<typename QueriesProcessor>
void SpeedTestProcessQueries(std::string_view mark, QueriesProcessor processor, const SearchServer &search_server,
    const std::vector<std::string> &queries) {
    OPERATION_DURATION(mark, std::cerr);
    const auto documents_lists = processor(search_server, queries);
}

template<typename ExecutionPolicy>
void TestSpeedMatching(std::string_view mark, SearchServer &search_server, const std::string &query,
    ExecutionPolicy &&policy) {
    OPERATION_DURATION(mark, std::cerr);
    const int document_count = search_server.GetDocumentCount();
    int word_count = 0;
    for (int id = 0; id < document_count; ++id) {
        const auto [words, status] = search_server.MatchDocument(policy, query, id);
        word_count += words.size();
    }
    std::cerr << "matched words: " << word_count << std::endl;
}

template<typename ExecutionPolicy>
void TestSpeedRemove(std::string_view mark, SearchServer search_server, ExecutionPolicy &&policy) {
    OPERATION_DURATION(mark, std::cerr);
    const int document_count = search_server.GetDocumentCount();
    for (int id = 0; id < document_count; ++id) {
        search_server.RemoveDocument(policy, id);
    }
    std::cerr << "Amount of docs after removing: " << search_server.GetDocumentCount() << std::endl;
}

template<typename ExecutionPolicy>
void SpeedTestFindTop(std::string_view mark, const SearchServer &search_server, const std::vector<std::string> &queries,
    ExecutionPolicy &&policy) {
    OPERATION_DURATION(mark, std::cerr);
    double total_relevance = 0;
    for (const std::string_view query : queries) {
        for (const auto &document : search_server.FindTopDocuments(policy, query)) {
            total_relevance += document.relevance;
        }
    }
    std::cerr << "Total relevance = " << total_relevance << std::endl;
}

void TestSearchServer(bool run_test = false, bool print_test_result = false, bool run_speed_test = false);

