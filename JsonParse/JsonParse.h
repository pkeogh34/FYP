//
// Created by Patrick Ross Keogh on 17/01/2024.
//
#ifndef JSONPARSE_H
#define JSONPARSE_H

#include "../Protein/Protein.h"
#include <iostream>
#include <string>
#include <vector>

std::unique_ptr<Protein> parseJson(const std::string &protein);
std::vector<std::unique_ptr<Protein>> parseJsonArray(const std::string &jsonString);
std::string readFileIntoString(const std::string &path);

#endif // JSONPARSE_H
