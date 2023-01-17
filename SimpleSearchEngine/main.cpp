#include <algorithm>
#include <cmath>
#include <iostream>
#include <map>
#include <set>
#include <string>
#include <utility>
#include <vector>

using namespace std;

/*This is a simple search engine for finding key words in text. Here we will use stop words, minus words, TF-IDF
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
//our example search query
cheburashka with big ears likes -oranges
*/

const int MAX_RESULT_DOCUMENT_COUNT = 5;

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

struct Document {
    int id;
    double relevance;
};

class SearchServer {
public:
    void SetDocumentsCount(int num) {
        document_count_ = num;
    }

    int GetDocumentsCount() {
        return document_count_;
    }

    void SetStopWords(const string& text) {
        for (const string& word : SplitIntoWords(text)) {
            stop_words_.insert(word);
        }
    }

    void AddDocument(int document_id, const string& document) {
        const vector<string> words = SplitIntoWordsNoStop(document);
        for (string word : words) {
            int w_count = count(words.begin(), words.end(), word);
            if (!word_to_document_freqs_.count(word)) {
                word_to_document_freqs_[word][document_id] = ((double)w_count / words.size());
            }
            else if (!word_to_document_freqs_[word].count(document_id)) {
                word_to_document_freqs_[word][document_id] = ((double)w_count / words.size());
            }
        }
    }

    vector<Document> FindTopDocuments(const string& raw_query) const {
        const Query query_words = ParseQuery(raw_query);
        auto matched_documents = FindAllDocuments(query_words);

        sort(matched_documents.begin(), matched_documents.end(),
            [](const Document& lhs, const Document& rhs) {
                return lhs.relevance > rhs.relevance;
            });
        if (matched_documents.size() > MAX_RESULT_DOCUMENT_COUNT) {
            matched_documents.resize(MAX_RESULT_DOCUMENT_COUNT);
        }
        return matched_documents;
    }

private:

    struct Query {
        set<string> plusWords;
        set<string> minusWords;
    };

    map<string, map<int, double>> word_to_document_freqs_;
    set<string> stop_words_;
    int document_count_;

    bool IsStopWord(const string& word) const {
        return stop_words_.count(word) > 0;
    }

    vector<string> SplitIntoWordsNoStop(const string& text) const {
        vector<string> words;
        for (const string& word : SplitIntoWords(text)) {
            if (!IsStopWord(word)) {
                words.push_back(word);
            }
        }
        return words;
    }

    Query ParseQuery(const string& text) const {
        Query query_words;
        for (const string& word : SplitIntoWordsNoStop(text)) {
            word[0] == '-' ? query_words.minusWords.insert(word.substr(1))
                : query_words.plusWords.insert(word);
        }
        return query_words;
    }

    vector<Document> FindAllDocuments(const Query& query_words) const {
        vector<Document> matched_documents;
        map<int, double> document_to_relevance;

        for (int id = 0; id < document_count_; ++id) { //проходим по всем id
            for (auto& word : query_words.plusWords) { //проходим по всем плюс словам               
                if (word_to_document_freqs_.count(word)) { //есть ли такое слово в БД
                    double idf = log((double)document_count_ / word_to_document_freqs_.at(word).size());
                    if (word_to_document_freqs_.at(word).count(id)) { //есть ли по слову документ с текущим id
                        document_to_relevance[id] += idf * word_to_document_freqs_.at(word).at(id);
                    }
                }
            }
        }
        for (auto& word : query_words.minusWords) {
            if (word_to_document_freqs_.count(word)) {
                for (auto& [key, val] : word_to_document_freqs_.at(word)) {
                    document_to_relevance.erase(key);
                }
            }
        }
        for (const auto& [id, relevance] : document_to_relevance) {
            matched_documents.push_back({ id, relevance });
        }
        return matched_documents;
    }
};

SearchServer CreateSearchServer() {
    SearchServer search_server;
    search_server.SetStopWords(ReadLine());
    search_server.SetDocumentsCount(ReadLineWithNumber());
    for (int document_id = 0; document_id < search_server.GetDocumentsCount(); ++document_id) {
        search_server.AddDocument(document_id, ReadLine());
    }

    return search_server;
}

int main() {
    const SearchServer search_server = CreateSearchServer();

    const string query = ReadLine();
    for (const auto& [document_id, relevance] : search_server.FindTopDocuments(query)) {
        cout << "{ document_id = "s << document_id << ", "
            << "relevance = "s << relevance << " }"s << endl;
    }
}