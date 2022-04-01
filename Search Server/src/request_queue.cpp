#include "request_queue.h"

#include <string>

#include "document.h"

using namespace std;

void RequestQueue::AddFindRequest(const string &raw_query, DocumentStatus status) {
    AddFindRequest(raw_query, [status](int document_id, DocumentStatus doc_status, int rating) {
        return doc_status == status;
    });
}

int RequestQueue::GetNoResultRequests() const {
    return no_result_count_;
}

