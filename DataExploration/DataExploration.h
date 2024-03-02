//
// Created by Patrick Ross Keogh on 18/01/2024.
//

#ifndef FYP_DATAEXPLORATION_H
#define FYP_DATAEXPLORATION_H

#include "../Protein/Protein.h"
#include <vector>

float avgProteinLength(const std::vector<std::unique_ptr<Protein>> &proteins);
float avgDisorder(const std::vector<std::unique_ptr<Protein>> &proteins);
int mostDisorderedProtein(const std::vector<std::unique_ptr<Protein>> &proteins);
std::unordered_map<char, int> disorderFrequencyOfAminoAcids(const std::vector<std::unique_ptr<Protein>> &proteins);
int totalNumAminoAcids(const std::vector<std::unique_ptr<Protein>> &proteins);

#endif // FYP_DATAEXPLORATION_H
