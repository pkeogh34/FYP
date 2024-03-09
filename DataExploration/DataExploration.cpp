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

int totalNumAminoAcids(const std::vector<std::unique_ptr<Protein>> &proteins) {
	int total = 0;
	for (const auto &protein : proteins) {
		total += protein->sequenceLength();
	}

	return total;
}
