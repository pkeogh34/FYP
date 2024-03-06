//
// Created by Patrick Ross Keogh on 18/01/2024.
//

#include "DataExploration.h"

float avgProteinLength(const std::vector<std::unique_ptr<Protein>> &proteins) {
	if (proteins.empty())
		return 0.0;

	float avgLength = 0.0;
	for (const auto &protein : proteins) {
		avgLength += protein->sequenceLength();
	}
	return avgLength / proteins.size();
}

float avgDisorder(const std::vector<std::unique_ptr<Protein>> &proteins) {
	if (proteins.empty())
		return 0.0;

	float avgDisorder = 0.0;
	for (const auto &protein : proteins) {
		avgDisorder += protein->percentageDisorder();
	}
	return avgDisorder / proteins.size();
}

int mostDisorderedProtein(const std::vector<std::unique_ptr<Protein>> &proteins) {
	int mostDisorderedProtein = 0;
	for (int i = 0; i < proteins.size(); i++) {
		if (proteins[i]->percentageDisorder() > proteins[mostDisorderedProtein]->percentageDisorder()) {
			mostDisorderedProtein = i;
		}
	}

	return mostDisorderedProtein;
}

std::unordered_map<char, int> disorderFrequencyOfAminoAcids(const std::vector<std::unique_ptr<Protein>> &proteins) {
	std::unordered_map<char, int> frequency;
	for (const auto &protein : proteins) {
		for (int i = 0; i < protein->getSequence().length(); i++) {
			if (protein->getReference()[i] == '1') {
				frequency[protein->getSequence()[i]]++;
			}
		}
	}
	return frequency;
}

int totalNumAminoAcids(const std::vector<std::unique_ptr<Protein>> &proteins) {
	int total = 0;
	for (const auto &protein : proteins) {
		total += protein->sequenceLength();
	}

	return total;
}
