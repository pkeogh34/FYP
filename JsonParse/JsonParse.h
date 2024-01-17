//
// Created by Patrick Ross Keogh on 17/01/2024.
//
#ifndef JSONPARSE_H
#define JSONPARSE_H

#include <iostream>
#include <string>
#include <vector>

struct ProteinData {
    std::string id;
    std::string sequence;
    std::string reference;
};

ProteinData parseJson(const std::string& protein);
std::vector<ProteinData> parseJsonArray(const std::string& jsonString);
std::string readFileIntoString(const std::string& path);

#endif // JSONPARSE_H

