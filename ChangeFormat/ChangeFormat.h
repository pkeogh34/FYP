//
// Created by Patrick Ross Keogh on 01/03/2024.
//
#include "../DataExploration/DataExploration.h"
#include "../Protein/Protein.h"
#include <fstream>
#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>
#include <memory>
#include <algorithm>
#include <random>

#ifndef FYP_CHANGEFORMAT_H
#define FYP_CHANGEFORMAT_H

void outputModifiedProteins(std::vector<std::unique_ptr<Protein>> &proteins, const std::string& filePath);
void splitData(std::vector<std::unique_ptr<Protein>>& data,
               std::vector<std::unique_ptr<Protein>>& train,
               std::vector<std::unique_ptr<Protein>>& test);
std::string decodeSequence(const std::string &encodedSequence);

#endif // FYP_CHANGEFORMAT_H
