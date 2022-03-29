#include "tests.h"

#include <random>

#include "global_library.h"
#include "library_main_class.h"

using namespace std;


string Tests::GenerateBooks(size_t amount)
{
    string generation_time;
    OPERATION_DURATION("generation time", generation_time);

    const int word_amount_at_lexicon = amount * 100;
    mt19937 generator;
    uniform_int_distribution<int> uniform_dist(0, word_amount_at_lexicon);

    for (size_t i = 0; i < amount; ++i) {
        pseudo_string_.emplace(to_string(uniform_dist(generator)));
    }

    return generation_time;
}

string Tests::AddToLib()
{
    string adding_time;
    OPERATION_DURATION("adding time", adding_time);
    for (const auto& item : pseudo_string_) {
        library.AddBook(item, item);
    }
    return adding_time;
}
