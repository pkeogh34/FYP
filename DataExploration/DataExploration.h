//
// Created by Patrick Ross Keogh on 18/01/2024.
//

#ifndef FYP_DATAEXPLORATION_H
#define FYP_DATAEXPLORATION_H

#include <vector>
#include "../Protein/Protein.h"

float avgProteinLength(const std::vector<Protein>& proteins);
float avgDisorder(const std::vector<Protein>& proteins);
Protein mostDisorderedProtein(const std::vector<Protein>& proteins);
std::unordered_map<char, int> disorderFrequencyOfAminoAcids(const std::vector<Protein>& proteins);
int totalNumAminoAcids(const std::vector<Protein>& proteins);

#endif //FYP_DATAEXPLORATION_H
