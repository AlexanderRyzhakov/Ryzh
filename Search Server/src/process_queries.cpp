#include "process_queries.h"

#include <execution>
#include <iterator>
#include <list>
#include <string>
#include <string_view>
#include <vector>
#include <utility>

#include "document.h"
#include "search_server.h"

using namespace std;

vector<vector<Document>> ProcessQueries(const SearchServer &search_server,
    const std::vector<std::string> &queries) {

    vector<vector<Document>> result(queries.size());

    transform(execution::par,
        queries.begin(), queries.end(), result.begin(),
        [&search_server](string_view query) {
            return search_server.FindTopDocuments(execution::par, query);
        });

    return result;
}

list<Document> ProcessQueriesJoined(const SearchServer &search_server, const std::vector<std::string> &queries) {
    auto results_for_queries = ProcessQueries(search_server, queries);

    return std::transform_reduce(std::execution::par,
        results_for_queries.begin(), results_for_queries.end(),
        std::list<Document> { },
        [](auto lhs, auto rhs) {
            lhs.splice(lhs.end(), rhs);
            return lhs;
        },
        [](auto &container) {
            return std::list<Document>(std::make_move_iterator(container.begin()),
                std::make_move_iterator(container.end()));
        });
}

