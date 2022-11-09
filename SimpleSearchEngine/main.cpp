#include <iostream>
#include <set>
#include <string>
#include <vector>

using namespace std;

/*This is a simple search engine for finding key words in text.

Input example:
//stop words to make our documents and query cleaner and more accurate

a an on the in is has been are with for from have be was

//number of documents
4

//documents where we gonna search info
a small curly guinea pig with grey hair has been found
a young 50 year old crocodile wants to make friends
a strange brown creature was seen in the box of oranges
a strange animal with big ears is building a house for its friends

//our search query
cheburashka with big ears likes oranges

*/

string ReadLine() {
    string s;
    getline(cin, s);
    return s;
}

int ReadLineWithNumber() {
    int result = 0;
    cin >> result;
    ReadLine();
    return result;
}

vector<string> SplitIntoWords(const string& text) {
    vector<string> words;
    string word;
    for (const char c : text) {
        if (c == ' ') {
            if (!word.empty()) {
                words.push_back(word);
                word.clear();
            }
        }
        else {
            word += c;
        }
    }
    if (!word.empty()) {
        words.push_back(word);
    }

    return words;
}

set<string> ParseStopWords(const string& text) {
    set<string> stop_words;
    for (const string& word : SplitIntoWords(text)) {
        stop_words.insert(word);
    }
    return stop_words;
}

vector<string> SplitIntoWordsNoStop(const string& text, const set<string>& stop_words) {
    vector<string> words;
    for (const string& word : SplitIntoWords(text)) {
        if (stop_words.count(word) == 0) {
            words.push_back(word);
        }
    }
    return words;
}

void AddDocument(vector<pair<int, vector<string>>>& documents,
    const set<string>& stop_words,
    int document_id,
    const string& document) {
    const vector<string> words = SplitIntoWordsNoStop(document, stop_words);
    documents.push_back({ document_id, words });
}

set<string> ParseQuery(const string& text, const set<string>& stop_words) {
    set<string> query_words;
    for (const string& word : SplitIntoWordsNoStop(text, stop_words)) {
        query_words.insert(word);
    }
    return query_words;
}

//Checks relevance
int MatchDocument(const pair<int, vector<string>>& content, const set<string>& query_words) {
    if (query_words.empty()) {
        return 0;
    }
    int counter = 0;
    for (const auto& word : content.second) {
        if (query_words.count(word) != 0) {
            ++counter;
        }
    }
    return counter;
}

// Returns id and relevance for each document
vector<pair<int, int>> FindDocuments(const vector<pair<int, vector<string>>>& documents,
    const set<string>& stop_words,
    const string& query) {
    const set<string> query_words = ParseQuery(query, stop_words);
    vector<pair<int, int>> matched_documents;
    for (const auto& document : documents) {
        int counter = MatchDocument(document, query_words);
        if (counter) {
            matched_documents.push_back({ document.first, counter });
        }
    }
    return matched_documents;
}

int main() {
    const string stop_words_joined = ReadLine();
    const set<string> stop_words = ParseStopWords(stop_words_joined);

    // Read documents
    vector<pair<int, vector<string>>> documents;
    const int document_count = ReadLineWithNumber();
    for (int document_id = 0; document_id < document_count; ++document_id) {
        AddDocument(documents, stop_words, document_id, ReadLine());
    }

    const string query = ReadLine();
    for (const auto& [document_id, relevance] : FindDocuments(documents, stop_words, query)) {
        cout << "{ document_id = "s << document_id << ", relevance = "s
            << relevance << " }"s << endl;
    }
}