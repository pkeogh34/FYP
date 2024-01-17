//
// Created by Patrick Ross Keogh on 17/01/2024.
//
#include "JsonParse.h"
#include <fstream>
#include <sstream>
using namespace std;

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
