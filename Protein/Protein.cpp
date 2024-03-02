//
// Created by Patrick Ross Keogh on 18/01/2024.
//

#include "Protein.h"

std::string Protein::getId() const
{
	return id;
}

void Protein::setId(const std::string &newId)
{
	id = newId;
}

std::string Protein::getSequence() const
{
	return sequence;
}

void Protein::setSequence(const std::string &newSequence)
{
	sequence = newSequence;
}

std::string Protein::getReference() const
{
	return reference;
}

void Protein::setReference(const std::string &newReference)
{
	reference = newReference;
}

int Protein::sequenceLength() const
{
	return sequence.length();
}

std::unordered_map<char, int> Protein::aminoAcidFrequency() const
{
	std::unordered_map<char, int> frequency;
	for (char aa : sequence)
	{
		frequency[aa]++;
	}
	return frequency;
}

double Protein::percentageDisorder() const
{
	int disorderCount = std::count(reference.begin(), reference.end(), '1');
	return 100.0 * disorderCount / sequenceLength();
}

std::string Protein::oneHotEncode()
{
	std::string encodedSequence;
	std::unordered_map<char, std::string> aminoAcidEncodings = {
		{'A', "1 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0"},
		{'C', "0 1 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0"},
		{'D', "0 0 1 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0"},
		{'E', "0 0 0 1 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0"},
		{'F', "0 0 0 0 1 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0"},
		{'G', "0 0 0 0 0 1 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0"},
		{'H', "0 0 0 0 0 0 1 0 0 0 0 0 0 0 0 0 0 0 0 0 0"},
		{'I', "0 0 0 0 0 0 0 1 0 0 0 0 0 0 0 0 0 0 0 0 0"},
		{'K', "0 0 0 0 0 0 0 0 1 0 0 0 0 0 0 0 0 0 0 0 0"},
		{'L', "0 0 0 0 0 0 0 0 0 1 0 0 0 0 0 0 0 0 0 0 0"},
		{'M', "0 0 0 0 0 0 0 0 0 0 1 0 0 0 0 0 0 0 0 0 0"},
		{'N', "0 0 0 0 0 0 0 0 0 0 0 1 0 0 0 0 0 0 0 0 0"},
		{'P', "0 0 0 0 0 0 0 0 0 0 0 0 1 0 0 0 0 0 0 0 0"},
		{'Q', "0 0 0 0 0 0 0 0 0 0 0 0 0 1 0 0 0 0 0 0 0"},
		{'R', "0 0 0 0 0 0 0 0 0 0 0 0 0 0 1 0 0 0 0 0 0"},
		{'S', "0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 1 0 0 0 0 0"},
		{'T', "0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 1 0 0 0 0"},
		{'V', "0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 1 0 0 0"},
		{'W', "0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 1 0 0"},
		{'Y', "0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 1 0"},
		{'X', "0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 1"},
		{'B', "0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 1"},
		{'J', "0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 1"},
		{'O', "0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 1"},
		{'U', "0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 1"},
		{'Z', "0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 1"}};
	for (const auto &aa : sequence)
	{
        encodedSequence += aminoAcidEncodings[aa];
        encodedSequence += " ";
	}

	return encodedSequence;
}

void Protein::toString() const
{
	std::cout << "ID: " << id << std::endl;
	std::cout << "Sequence: " << sequence << std::endl;
	std::cout << "Reference: " << reference << std::endl;
}
