//
// Created by Patrick Ross Keogh on 17/01/2024.
//
#ifndef JSONPARSE_H
#define JSONPARSE_H

#include <iostream>
#include <string>
#include <vector>
#include "../Protein/Protein.h"

Protein parseJson(const std::string& protein);
std::vector<Protein> parseJsonArray(const std::string& jsonString);
std::string readFileIntoString(const std::string& path);

#endif // JSONPARSE_H

