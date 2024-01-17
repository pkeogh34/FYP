#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <sstream>
using namespace std;

struct ProteinData {
    string id;
    string sequence;
    string reference;
};

ProteinData parseJson(const string& protein) {
    ProteinData data;
    stringstream ss(protein);
    string temp, key, value;

    while (getline(ss, temp, ',')) {
        stringstream pairStream(temp);
        getline(pairStream, key, ':');
        getline(pairStream, value, ':');
        value.erase(remove(value.begin(), value.end(), '"'), value.end());
        value.erase(remove(value.begin(), value.end(), '}'), value.end());
        value.erase(remove(value.begin(), value.end(), '{'), value.end());
        value.erase(remove(value.begin(), value.end(), ' '), value.end());

        if (key.find("id") != string::npos) data.id = value;
        else if (key.find("sequence") != string::npos) data.sequence = value;
        else if (key.find("reference") != string::npos) data.reference = value;
    }
    return data;
}

vector<ProteinData> parseJsonArray(const string& jsonString) {
    vector<ProteinData> dataArray;
    stringstream ss(jsonString);
    string token;

    while (getline(ss, token, '}')) {
        if (token.find('{') != string::npos) {
            token += '}';
            dataArray.push_back(parseJson(token));
        }
    }
    return dataArray;
}

string readFileIntoString(const string& path) {
    ifstream input_file(path);
    if (!input_file.is_open()) {
        cerr << "Could not open the file - '" << path << "'" << endl;
        return "";
    }
    return {(istreambuf_iterator<char>(input_file)), istreambuf_iterator<char>()};
}

int main() {
    string filePath = "IDRdataset.json";
    string jsonContent = readFileIntoString(filePath);
    if (jsonContent.empty()) {
        cerr << "Failed to read JSON file." << endl;
        return 1;
    }

    vector<ProteinData> data = parseJsonArray(jsonContent);

    for (const auto& d : data) {
        cout << "ID: " << d.id << endl;
        cout << "Sequence: " << d.sequence << endl;
        cout << "Reference: " << d.reference << endl;
        cout << "-------------------" << endl;
    }

    return 0;
}

