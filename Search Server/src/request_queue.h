#pragma once

#include <deque>
#include <string>

#include "document.h"
#include "search_server.h"

class RequestQueue {
public:
    explicit RequestQueue(const SearchServer &search_server) :
        server_(search_server), no_result_count_(0), request_count_(0) {
    }

    template<typename DocumentPredicate>
    void AddFindRequest(const std::string &raw_query, DocumentPredicate document_predicate) {
        int found_docs_amount = server_.FindTopDocuments(raw_query, document_predicate).size();
        QueryResult new_query(raw_query, found_docs_amount);
        if (request_count_ < kMaxQueriesAmount) {
            ++request_count_;
        } else {
            if (requests_.front().amount_of_found_docs == 0) {
                --no_result_count_;
            }
            requests_.pop_front();
        }

        requests_.push_back(new_query);
        if (new_query.amount_of_found_docs == 0) {
            ++no_result_count_;
        }
    }

    void AddFindRequest(const std::string &raw_query, DocumentStatus status = DocumentStatus::ACTUAL);

    int GetNoResultRequests() const;

private:
    struct QueryResult {
        explicit QueryResult(const std::string &raw_query, int number_of_docs) :
            query(raw_query), amount_of_found_docs(number_of_docs) {
        }
        std::string query;
        int amount_of_found_docs = 0;
    };

private:
    static const int kMaxQueriesAmount = 1440;

private:
    const SearchServer &server_;
    std::deque<QueryResult> requests_;
    int no_result_count_ = 0;
    int request_count_ = 0;
};
