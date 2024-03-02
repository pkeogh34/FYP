//
// Created by Patrick Ross Keogh on 01/03/2024.
//

#include "ChangeFormat.h"

void outputModifiedProteins(std::vector<std::unique_ptr<Protein>> &proteins)
{
	std::ofstream outfile("encoded_sequence.txt");
	outfile << proteins.size() << std::endl;
	outfile << "21" << std::endl;
	for (const auto &protein : proteins)
	{
		if (outfile.is_open())
		{
			outfile << protein->getId() << std::endl;
			outfile << protein->sequenceLength() << std::endl;
			outfile << protein->oneHotEncode() << std::endl;
            for(auto &c: protein->getReference()){
                outfile << c << " ";
            }
            outfile << std::endl;
            outfile << std::endl;
		}
		else
		{
			std::cerr << "Unable to open file" << std::endl;
		}
	}
	outfile.close();
}

std::string decodeSequence(const std::string &encodedSequence)
{
	std::unordered_map<std::string, char> aminoAcidDeencodings = {
		{"1 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0", 'A'},
		{"0 1 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0", 'C'},
		{"0 0 1 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0", 'D'},
		{"0 0 0 1 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0", 'E'},
		{"0 0 0 0 1 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0", 'F'},
		{"0 0 0 0 0 1 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0", 'G'},
		{"0 0 0 0 0 0 1 0 0 0 0 0 0 0 0 0 0 0 0 0 0", 'H'},
		{"0 0 0 0 0 0 0 1 0 0 0 0 0 0 0 0 0 0 0 0 0", 'I'},
		{"0 0 0 0 0 0 0 0 1 0 0 0 0 0 0 0 0 0 0 0 0", 'K'},
		{"0 0 0 0 0 0 0 0 0 1 0 0 0 0 0 0 0 0 0 0 0", 'L'},
		{"0 0 0 0 0 0 0 0 0 0 1 0 0 0 0 0 0 0 0 0 0", 'M'},
		{"0 0 0 0 0 0 0 0 0 0 0 1 0 0 0 0 0 0 0 0 0", 'N'},
		{"0 0 0 0 0 0 0 0 0 0 0 0 1 0 0 0 0 0 0 0 0", 'P'},
		{"0 0 0 0 0 0 0 0 0 0 0 0 0 1 0 0 0 0 0 0 0", 'Q'},
		{"0 0 0 0 0 0 0 0 0 0 0 0 0 0 1 0 0 0 0 0 0", 'R'},
		{"0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 1 0 0 0 0 0", 'S'},
		{"0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 1 0 0 0 0", 'T'},
		{"0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 1 0 0 0", 'V'},
		{"0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 1 0 0", 'W'},
		{"0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 1 0", 'Y'},
		{"0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 1", 'X'}};

	int length = encodedSequence.length();
	std::string decodedSequence;
	for (int i = 0; i < length; i += 42)
	{
		std::string substr = encodedSequence.substr(i, 41);
		auto it = aminoAcidDeencodings.find(substr);
		if (it != aminoAcidDeencodings.end())
		{
			decodedSequence += it->second;
		}
		else
		{
			std::cerr << "Error: Invalid encoding found at position " << i << std::endl;
			return "";
		}
	}

	return decodedSequence;
}