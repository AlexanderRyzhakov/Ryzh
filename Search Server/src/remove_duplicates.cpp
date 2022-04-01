#include "remove_duplicates.h"

#include <iostream>
#include <map>
#include <set>
#include <string>

#include "search_server.h"

using namespace std;

void RemoveDuplicates(SearchServer &server) {
    set<int> duplicate_id;
    set<set<string_view>> existing_word_sets;

    for (const auto current_id_aff_all : server) {
        set<string_view> new_word_set;
        for (const auto& [word, _] : server.GetWordsFrequencies(current_id_aff_all)) {
            new_word_set.emplace_hint(new_word_set.end(), word);
        }

        if (!existing_word_sets.emplace(new_word_set).second) {
            duplicate_id.emplace_hint(duplicate_id.end(), current_id_aff_all);
        }
    }

    for (const auto id_to_delete : duplicate_id) {
        cout << "Found duplicate document id " << id_to_delete << '\n';
        server.RemoveDocument(id_to_delete);
    }
}
