#include <algorithm>
#include <cassert>
#include <fstream>
#include <vector>
#include <utility>

#include "author.h"
#include "book.h"
#include "file_read_write.h"
#include "global_library.h"
#include "string_process.h"

#define FILE_END "|___FIN___|"

using namespace std;

void SaveLib()
{
    Save_ALI_APP_v_0_1();
    library.Saved();
}

void OpenLib(const std::string& file_name)
{
    ifstream file(file_name, ios::in);
    assert(file);

    string file_version;
    getline(file, file_version);
    if (!count(kVersion.begin(), kVersion.end(), file_version)) {
        file.close();
        throw invalid_argument("unsupporetd version");
    }
    library.Clear();

    if (file_version == "ALI_APP_v_0.2") {
        Open_ALI_APP_v_0_1(file);
    } else if (file_version == "ALI_APP_v_0.1") {
        Open_ALI_APP_v_0_1(file);
    }

    file.close();
    library.Saved();
}

//------------------------------------------------------------- START *** ALI_APP_v_0.1
//format: lib_name.ali - books
//version\n
//Lirary name\n
//book1_name\n
//book1_index\n
//author1,author2 ... \n
//rating\n
// owned (if true owned = true, else wish-list = true)
//comment\n
//book2_name\n
//....
//|___FIN___|


//libarary class:
//std::string library_name_;
//std::map<size_t, Book> books_;    - lib_name.ali
//std::set<Author> authors_;        - lib_name.atr
//std::set<std::string> ganres_;    - lib_name.agr

//book class:
//std::string title_;
//std::set<Author> authors_ = {};
//std::set<std::string> ganres_ = {};
//std::string comment = "";
//int rating = 0;

void Open_ALI_APP_v_0_1(ifstream& file)
{
    string file_lib_name;
    getline(file, file_lib_name);
    library.ChangeLibName(move(file_lib_name));

    string book_title;
    getline(file, book_title);
    while (book_title != FILE_END) {
        string index;
        getline(file, index);

        string authors;
        getline(file, authors);

        Book book(move(book_title), move(authors));

        string rating;
        getline(file, rating);
        book.SetRating(atoi(rating.c_str()));

        string owned;
        getline(file, owned);
        if (owned == "1" || owned == "true") {
            book.SetOwned();
        } else {
            book.SetWished();
        }

        string comment;
        getline(file, comment);
        book.SetComment(move(comment));

        library.AddBook(book, atoi(index.c_str()));

        getline(file, book_title);
    }
}

void Save_ALI_APP_v_0_1()
{
    std::string file_name = CleanString(library.GetLibName());
    if (file_name.size() > 16u) {
        file_name.resize(16u);
    }
    file_name += ".ali";

    ofstream file(file_name, ios::out);
    assert(file);
    file.clear();

    // version
    file << *kVersion.begin() << '\n';
    //library name
    file << library.GetLibName() << "\n";
    //books
    for (const auto& [index, book] : library) {
        //title
        file << book.GetTitle() << '\n';
        //index
        file << index << '\n';
        //authors
        bool first_author = true;
        for (const auto& author_name : book.GetAuthors()) {
            if (!first_author) {
                file << ',';
            }
            file << author_name.GetName();
            first_author = false;
        }
        file << '\n';
        //rating
        file << book.GetRating() << '\n';
        //owned or wish_list
        file << book.IsOwned() << '\n';
        //comment
        std::string comment = book.GetComment();
         RemoveNewLineSymbol(comment);
        file << comment  << '\n';
    }
    file << FILE_END;
    file.close();
}
//------------------------------------------------------------- END *** ALI_APP_v_0.1

