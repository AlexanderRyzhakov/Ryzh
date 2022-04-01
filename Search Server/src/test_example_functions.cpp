#include <cmath>
#include <cstdint>
#include <iterator>
#include <map>
#include <random>
#include <set>
#include <string_view>
#include <tuple>

#include "document.h"
#include "log_duration.h"
#include "process_queries.h"
#include "search_server.h"
#include "test_example_functions.h"

using namespace std;

void AssertImpl(bool value, const string &expr_str, const string &file, const string &func, unsigned line,
    const string &hint) {
    if (!value) {
        cerr << file << "("s << line << "): "s << func << ": "s;
        cerr << "ASSERT("s << expr_str << ") failed."s;
        if (!hint.empty()) {
            cerr << " Hint: "s << hint;
        }
        cerr << endl;
        abort();
    }
}

#define ASSERT_EQUAL(a, b) AssertEqualImpl((a), (b), #a, #b, __FILE__, __FUNCTION__, __LINE__, ""s);
#define ASSERT_EQUAL_HINT(a, b, hint) AssertEqualImpl((a), (b), #a, #b, __FILE__, __FUNCTION__, __LINE__, (hint) );
#define ASSERT(expr) AssertImpl( (expr), #expr, __FILE__, __FUNCTION__, __LINE__, ""s);
#define ASSERT_HINT(expr, hint) AssertImpl( (expr), #expr, __FILE__, __FUNCTION__, __LINE__, (hint));

#define RUN_TEST(func, print_result) RunTestImpl( (func), #func, (print_result))

void AddDocument(SearchServer &search_server, int new_id, const string &document, DocumentStatus status,
    vector<int> ratings) {
    search_server.AddDocument(new_id, document, status, ratings);
}

void TestAddDocument() {
    SearchServer test_class_search_server("in the on"s);

    ASSERT(test_class_search_server.FindTopDocuments("any"s).empty());

    test_class_search_server.AddDocument(42, "some words"s, DocumentStatus::ACTUAL, { 1, 2, 3 });

    const auto &found_docs = test_class_search_server.FindTopDocuments("words"s);
    ASSERT_HINT(!found_docs.empty(), "Doument search failed"s);
    ASSERT_HINT(test_class_search_server.GetDocumentCount() == 1u, "Incorreect documents count"s);
    ASSERT_EQUAL_HINT(found_docs[0].id, 42, "Incorrect ID"s);
}

void TestMinusWords() {
    SearchServer test_class_search_server("and"s);

    test_class_search_server.AddDocument(0, "good-bad and ugly"s, DocumentStatus::ACTUAL, { });
    ASSERT(!test_class_search_server.FindTopDocuments("ugly"s).empty());
    ASSERT(test_class_search_server.FindTopDocuments("-western"s).empty());
    ASSERT(!test_class_search_server.FindTopDocuments("good-bad"s).empty());
    ASSERT_HINT(test_class_search_server.FindTopDocuments("good -ugly"s).empty(), "Minus word not working"s);
}

void TestMatching() {
    SearchServer test_class_search_server("and"s);

    test_class_search_server.AddDocument(0, "city cat flat"s, DocumentStatus::ACTUAL, { });
    vector<string> query = { "city cat"s, "city -cat"s, "city cat city cat cat" };
    string test_str1 = "cat";
    string test_str2 = "city";

    vector<vector<string_view>> expected_words = { { test_str1, test_str2 }, { }, { test_str1, test_str2 } };

    for (int i = 0; i < 2; ++i) {
        const auto &returned_result = test_class_search_server.MatchDocument(query.at(i), 0);
        auto returned_words = get<vector<string_view>>(returned_result);
        DocumentStatus returned_status = get<DocumentStatus>(returned_result);
        ASSERT(returned_status == DocumentStatus::ACTUAL);
        ASSERT_EQUAL_HINT(returned_words, expected_words.at(i), "Incorrect output with minus word"s);
        ASSERT_EQUAL_HINT(returned_words.size(), expected_words.at(i).size(), "Incorrect size - matching"s);
    }
}

void TestSort() {
    SearchServer test_class_search_server("and"s);
    const DocumentStatus document_status_example = DocumentStatus::ACTUAL;
    double relevance_count_accuracy = 1e-6;

    const string test_query = "mouse brown house"s;
    const vector<string> test_document_text = { "doc withot query words"s,
        "lory was a brown mouse ___ two qurey words"s, "lory was a brown mouse ___ two qurey words"s,
        "mouse in a big brown house ___ three query words"s, "another doc withot query words"s,
        "its a story about mouse in a house ___ two query words"s, "tom cat and mouse jerry ___ one query word"s,
        "big brown wall ___ one query word"s };
    const uint8_t number_of_documents_to_add = test_document_text.size();

    for (uint8_t i = 0; i < number_of_documents_to_add; ++i) {
        test_class_search_server.AddDocument(i, test_document_text[i], document_status_example, { i });
    }

    const auto &found_docs = test_class_search_server.FindTopDocuments(test_query);

    ASSERT(found_docs.size() <= kMaxResultDocumentCount);

    for (uint8_t j = 1; j < found_docs.size(); ++j) {
        if (abs(found_docs[j].relevance - found_docs[j - 1].relevance) >= relevance_count_accuracy) {
            ASSERT_HINT(found_docs[j - 1].relevance > found_docs[j].relevance, "Incorrect sort by relevance"s);
        } else {
            ASSERT_HINT(found_docs[j - 1].rating >= found_docs[j].rating, "Incorrect sort by rating"s);
        }
    }
}

void TestRatingsCount() {
    vector<vector<int>> test_ratings_input = { { }, { 1, 2 }, { 0, 4, 8 }, { -4, 2 }, { -9, -2 } };
    vector<int> expected_rating_output = { 0, 1, 4, -1, -5, };

    if (test_ratings_input.size() != expected_rating_output.size()) {
        cerr << "Test data incorrect ( void TestRatingsCount() )"s;
        abort();
    }

    for (uint8_t i = 0; i < test_ratings_input.size(); ++i) {
        SearchServer test_class_search_server("and"s);
        test_class_search_server.AddDocument(0, "test"s, DocumentStatus::ACTUAL, test_ratings_input[i]);
        ASSERT_EQUAL_HINT(test_class_search_server.FindTopDocuments("test"s).at(0).rating, expected_rating_output[i],
            "Incorrect rating count"s);
    }
}

void TestFindByUserPredicate() {
    string default_test_text = "the greatest novel of all times"s;
    DocumentStatus default_test_status = DocumentStatus::ACTUAL;
    vector<int> default_test_rating = { 1 };

    // test - predicate for document ID
    {
        SearchServer test_class_search_server("and"s);
        for (int i = 0; i < 10; ++i) {
            test_class_search_server.AddDocument(i, default_test_text, default_test_status, default_test_rating);
        }
        const auto &found_docs = test_class_search_server.FindTopDocuments(default_test_text,
            [](int doc_id, DocumentStatus status, int rating) {
                return doc_id == 7;
            });
        ASSERT(found_docs.size() == 1u);
        ASSERT_EQUAL_HINT(found_docs[0].id, 7, "find by ID not correct"s);
    }

    // test - predicate for document status
    {
        SearchServer test_class_search_server("and"s);
        vector<DocumentStatus> test_status = { DocumentStatus::ACTUAL, DocumentStatus::IRRELEVANT,
            DocumentStatus::BANNED, DocumentStatus::REMOVED, DocumentStatus::IRRELEVANT };
        for (uint8_t i = 0; i < test_status.size(); ++i) {
            test_class_search_server.AddDocument(i, default_test_text, test_status[i], default_test_rating);
        }
        const auto &found_docs = test_class_search_server.FindTopDocuments(default_test_text,
            [](int doc_id, DocumentStatus status, int rating) {
                return status == DocumentStatus::IRRELEVANT;
            });
        ASSERT_EQUAL_HINT(found_docs.size(), 2u, "find by status not correct"s);
    }

    // test - predicate for document rating
    {
        SearchServer test_class_search_server("and"s);
        const string test_hint = "find by rating not correct"s;
        vector<vector<int>> test_rating = { { 1 }, { }, { -2, -3 }, { 10, 4, 3 }, { -7 } };
        for (uint8_t i = 0; i < test_rating.size(); ++i) {
            test_class_search_server.AddDocument(i, default_test_text, default_test_status, test_rating[i]);
        }
        const auto &found_docs = test_class_search_server.FindTopDocuments(default_test_text,
            [](int doc_id, DocumentStatus status, int rating) {
                return rating < 0;
            });
        ASSERT_EQUAL_HINT(found_docs.size(), 2u, test_hint);
        ASSERT_EQUAL_HINT(found_docs[0].rating, -2, test_hint);
    }
}

void TestFindByStatus() {
    SearchServer test_class_search_server("and"s);
    vector<DocumentStatus> test_statuses = { DocumentStatus::ACTUAL, DocumentStatus::IRRELEVANT,
        DocumentStatus::REMOVED, DocumentStatus::REMOVED, DocumentStatus::IRRELEVANT };
    string test_text = "war and peace"s;

    for (uint8_t i = 0; i < test_statuses.size(); ++i) {
        test_class_search_server.AddDocument(i, test_text, test_statuses[i], { });
    }

    ASSERT_EQUAL(test_class_search_server.FindTopDocuments(test_text, DocumentStatus::ACTUAL).size(), 1u);
    ASSERT_EQUAL(test_class_search_server.FindTopDocuments(test_text, DocumentStatus::REMOVED).size(), 2u);
    ASSERT_EQUAL(test_class_search_server.FindTopDocuments(test_text, DocumentStatus::BANNED).size(), 0u);
}

void TestRelevanceCount() {
    const double acceptable_relevance_mistake = 1e-6;
    const string test_hint = "Incorrect relevance count"s;
    const DocumentStatus document_status_example = DocumentStatus::ACTUAL;
    SearchServer test_class_search_server("and"s);

    test_class_search_server.AddDocument(0, "one two three four"s, document_status_example, { });
    test_class_search_server.AddDocument(1, "two lazy dogs"s, document_status_example, { });
    test_class_search_server.AddDocument(2, "story with many words"s, document_status_example, { });
    double document_count = 3.0;

    string test_query = "one two"s;
    double idf_one = log(document_count / 1.0);
    double idf_two = log(document_count / 2.0);

    double tf_one_in_doc1 = 1.0 / 4.0;
    double tf_one_in_doc2 = 0.0 / 4.0;
    double tf_two_in_doc1 = 1.0 / 4.0;
    double tf_two_in_doc2 = 1.0 / 3.0;

    double expected_relevance_for_doc1 = idf_one * tf_one_in_doc1 + idf_two * tf_two_in_doc1;
    double expected_relevance_for_doc2 = idf_one * tf_one_in_doc2 + idf_two * tf_two_in_doc2;

    const auto &found_docs = test_class_search_server.FindTopDocuments(test_query);
    ASSERT(found_docs.size() == 2u);

    ASSERT_HINT(abs(found_docs[0].relevance - expected_relevance_for_doc1) <= acceptable_relevance_mistake, test_hint);
    ASSERT_HINT(abs(found_docs[1].relevance - expected_relevance_for_doc2) <= acceptable_relevance_mistake, test_hint);
}

void TestExcludeStopWordsFromAddedDocumentContent() {
    const string test_text = "stars in the sky"s;
    const DocumentStatus test_status = DocumentStatus::ACTUAL;
    const vector<int> test_ratings = { 1 };

    {
        SearchServer test_class_search_server("  in the  "s);
        test_class_search_server.AddDocument(0, test_text, test_status, test_ratings);
        ASSERT(!test_class_search_server.FindTopDocuments("stars"s).empty());
        ASSERT_HINT(test_class_search_server.FindTopDocuments("the"s).empty(),
            "Stop words must be excluded from documents"s);
    }

    {
        vector<string> stop_words = { ""s, "in"s, "the"s };
        SearchServer test_class_search_server(stop_words);
        test_class_search_server.AddDocument(0, test_text, test_status, test_ratings);
        ASSERT_HINT(test_class_search_server.FindTopDocuments("in"s).empty(), "Stop words vector addition failed"s);
    }

    {
        set<string> stop_words = { "in"s, "the"s };
        SearchServer test_class_search_server(stop_words);
        test_class_search_server.AddDocument(0, test_text, test_status, test_ratings);
        ASSERT_HINT(test_class_search_server.FindTopDocuments("in"s).empty(), "Stop words set addition failed"s);
    }
}

void TestGetWordsFrequencies() {
    SearchServer test_class_search_server("and"s);
    int document_id = 15;
    test_class_search_server.AddDocument(document_id, "aONE bTWO cTHREE and dFOUR and dFOUR"s, DocumentStatus::ACTUAL, {
        1, 2 });

    auto words_and_frequency = test_class_search_server.GetWordsFrequencies(document_id);
    ASSERT_EQUAL(words_and_frequency.size(), 4u);
    ASSERT(words_and_frequency.count("aONE"s));
    ASSERT_EQUAL(words_and_frequency.at("aONE"s), .2);
    ASSERT(words_and_frequency.count("dFOUR"s));
    ASSERT_EQUAL(words_and_frequency.at("dFOUR"s), .4);
    ASSERT(test_class_search_server.GetWordsFrequencies(11).empty());
}

void TestIterators() {
    SearchServer test_class_search_server("and"s);
    string text = "Cpp is cool";
    vector<int> ratings = { 5 };
    DocumentStatus status = DocumentStatus::ACTUAL;

    for (int id = 1; id <= 10; ++id) {
        test_class_search_server.AddDocument(id, text, status, ratings);
    }
    ASSERT_EQUAL(*test_class_search_server.begin(), 1);
    ASSERT_EQUAL(distance(test_class_search_server.begin(), test_class_search_server.end()), 10);

    int test_id = 1;
    for (const int it : test_class_search_server) {
        ASSERT_EQUAL(it, test_id++);
    }
}

void ErrorTestInvalidInitialization() {
    bool error_found = false;

    try {
        SearchServer test_search_server(" in and \x11"s);
    } catch (const exception &error) {
        error_found = true;
    }
    ASSERT(error_found);
}

void ErrorTestInvalidAddDocument() {
    SearchServer test_search_server("and"s);
    string text = "test"s;
    DocumentStatus satus = DocumentStatus::ACTUAL;
    vector<int> ratings = { 1 };

    bool error_found = false;

    try {
        test_search_server.AddDocument(-1, text, satus, ratings);
    } catch (const exception &error) {
        error_found = true;
    }
    ASSERT_HINT(error_found, "Check negative ID failed"s);

    error_found = false;
    try {
        test_search_server.AddDocument(1, text, satus, ratings);
        test_search_server.AddDocument(1, "another text"s, satus, ratings);
    } catch (const exception &error) {
        error_found = true;
    }
    ASSERT_HINT(error_found, "Check same ID failed"s);

    error_found = false;
    try {
        test_search_server.AddDocument(1, "uno dos tres \10"s, satus, ratings);
    } catch (const exception &error) {
        error_found = true;
    }
    ASSERT_HINT(error_found, "Check unacceptable symbols failed"s);
}

void ErrorTestInvalidRequestFindTopDocuments() {
    SearchServer test_search_server("and"s);
    test_search_server.AddDocument(0, "war and peace"s, DocumentStatus::ACTUAL, { });

    ASSERT(!test_search_server.FindTopDocuments("war and"s).empty());

    bool error_found = false;
    try {
        test_search_server.FindTopDocuments("war and \x3"s);
    } catch (const exception &error) {
        error_found = true;
    }
    ASSERT_HINT(error_found, "Invalid query error not thrown"s);
}

void ErrorTestInvalidRequestMatchDocument() {
    SearchServer test_search_server("and"s);
    test_search_server.AddDocument(0, "war and peace"s, DocumentStatus::ACTUAL, { });

    bool error_found = false;
    try {
        test_search_server.MatchDocument("war and \x12"s, 0);
    } catch (const exception &error) {
        error_found = true;
    }
    ASSERT_HINT(error_found, "Invalid query error not thrown"s);
}

void ErrorTestMinusInRequest() {
    SearchServer test_search_server("and"s);
    test_search_server.AddDocument(0, "war and peace"s, DocumentStatus::ACTUAL, { });
    ASSERT(!test_search_server.FindTopDocuments("war"s).empty());
    ASSERT(test_search_server.FindTopDocuments("-war"s).empty());

    bool error_found = false;
    try {
        test_search_server.FindTopDocuments("--war"s);
    } catch (const exception &error) {
        error_found = true;
    }
    ASSERT_HINT(error_found, "Double minus error not thrown"s);

    error_found = false;
    try {
        test_search_server.FindTopDocuments("war  - "s);
    } catch (const exception &error) {
        error_found = true;
    }
    ASSERT_HINT(error_found, "Minus without word not thrown"s);

}

void SpeedTest() {
    SearchServer search_server("and with"s);

    const int docs_to_add = 1'000;
    const int word_amount_at_lexicon = 5'000;
    const int words_in_document = 100;
    const int doc_id_to_frequency_report = docs_to_add / 2;
    const int doc_id_to_deleate = docs_to_add / 2;

    mt19937 generator;
    uniform_int_distribution<int> uniform_dist(0, word_amount_at_lexicon);

    {
        string test_name = "Adding documents (" + to_string(docs_to_add) + " documents)";
        OPERATION_DURATION(test_name, cerr);

        for (int i = 1; i <= docs_to_add; ++i) {
            string text = ""s;
            for (int i = 1; i <= words_in_document; ++i) {
                text += to_string(uniform_dist(generator)) + ' ';
            }
            search_server.AddDocument(i, text, DocumentStatus::ACTUAL, { });
        }
    }

    {
        string test_name = "Frequencies by ID search (ID = " + to_string(doc_id_to_frequency_report) + ")";
        OPERATION_DURATION(test_name, cerr);
        auto words_and_frequency = search_server.GetWordsFrequencies(doc_id_to_frequency_report);
    }

    {
        string test_name = "Delate by ID (ID = " + to_string(doc_id_to_deleate) + ")";
        OPERATION_DURATION(test_name, cerr);
        search_server.RemoveDocument(doc_id_to_deleate);
    }
}

string GenerateWord(mt19937 &generator, int max_length) {
    const int length = uniform_int_distribution(1, max_length)(generator);
    string word;
    word.reserve(length);
    for (int i = 0; i < length; ++i) {
        word.push_back(uniform_int_distribution('a', 'z')(generator));
    }
    return word;
}

vector<string> GenerateDictionary(mt19937 &generator, int word_count, int max_length) {
    vector<string> words;
    words.reserve(word_count);
    for (int i = 0; i < word_count; ++i) {
        words.push_back(GenerateWord(generator, max_length));
    }
    sort(words.begin(), words.end());
    words.erase(unique(words.begin(), words.end()), words.end());
    return words;
}

string GenerateQuery(mt19937 &generator, const vector<string> &dictionary, int word_count, double minus_prob) {
    string query;
    for (int i = 0; i < word_count; ++i) {
        if (!query.empty()) {
            query.push_back(' ');
        }
        if (uniform_real_distribution<>(0, 1)(generator) < minus_prob) {
            query.push_back('-');
        }
        query += dictionary[uniform_int_distribution<int>(0, dictionary.size() - 1)(generator)];
    }
    return query;
}

vector<string> GenerateQueries(mt19937 &generator, const vector<string> &dictionary, int query_count,
    int max_word_count) {
    vector<string> queries;
    queries.reserve(query_count);
    for (int i = 0; i < query_count; ++i) {
        queries.push_back(GenerateQuery(generator, dictionary, max_word_count, 0.4));
    }
    return queries;
}

void TestSearchServer(bool run_test, bool print_test_result, bool run_speed_test) {
    if (!run_test) {
        return;
    }

    RUN_TEST(TestAddDocument, print_test_result);
    RUN_TEST(TestExcludeStopWordsFromAddedDocumentContent, print_test_result);
    RUN_TEST(TestMinusWords, print_test_result);
    RUN_TEST(TestMatching, print_test_result);
    RUN_TEST(TestSort, print_test_result);
    RUN_TEST(TestRatingsCount, print_test_result);
    RUN_TEST(TestFindByUserPredicate, print_test_result);
    RUN_TEST(TestFindByStatus, print_test_result);
    RUN_TEST(TestRelevanceCount, print_test_result);
    RUN_TEST(TestGetWordsFrequencies, print_test_result);
    RUN_TEST(TestIterators, print_test_result);

    RUN_TEST(ErrorTestInvalidInitialization, print_test_result);
    RUN_TEST(ErrorTestInvalidAddDocument, print_test_result);
    RUN_TEST(ErrorTestInvalidRequestFindTopDocuments, print_test_result);
    RUN_TEST(ErrorTestInvalidRequestMatchDocument, print_test_result);
    RUN_TEST(ErrorTestMinusInRequest, print_test_result);

    if (run_speed_test) {
        RUN_TEST(SpeedTest, print_test_result);

        mt19937 generator;
        const auto dictionary = GenerateDictionary(generator, 1'000, 10);
        const auto documents = GenerateQueries(generator, dictionary, 10'000, 70);

        SearchServer search_server(dictionary[0]);
        for (size_t i = 0; i < documents.size(); ++i) {
            search_server.AddDocument(i, documents[i], DocumentStatus::ACTUAL, { 1, 2, 3 });
        }

        const auto queries = GenerateQueries(generator, dictionary, 100, 70);
        const string query = GenerateQuery(generator, dictionary, 500, 0.1);

        SPEED_TEST_PROCESS_QUERIES(ProcessQueries);

        cerr << "speed test - matching" << endl;
        SPEED_TEST_MATCHING(seq);
        SPEED_TEST_MATCHING(par);

        cerr << "speed test - remove" << endl;
        SPPED_TEST_REMOVE(seq);
        SPPED_TEST_REMOVE(par);

        cerr << "speed test - find top documents" << endl;
        SPEED_TEST_FIND_TOP(seq);
        SPEED_TEST_FIND_TOP(par);
    }

    if (print_test_result) {
        cerr << "Search server testing finished"s << endl;
    }
}

