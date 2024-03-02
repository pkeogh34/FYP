//
// Created by Patrick Ross Keogh on 01/03/2024.
//
#include <unordered_map>
#include <string>
#include <fstream>
#include <iostream>
#include "Protein/Protein.h"
#include "DataExploration/DataExploration.h"

#ifndef FYP_CHANGEFORMAT_H
#define FYP_CHANGEFORMAT_H

std::string oneHotEncode(Protein& protein);
void outputModifiedProtein(std::vector<std::unique_ptr<Protein>>& proteins);
std::string decodeSequence(const std::string& encodedSequence);


#endif //FYP_CHANGEFORMAT_H
