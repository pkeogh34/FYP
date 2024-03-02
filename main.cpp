#include "ChangeFormat.h"
#include "DataExploration/DataExploration.h"
#include "JsonParse/JsonParse.h"

int main()
{
	std::string filePath = "IDRdataset.json";
	std::string jsonContent = readFileIntoString(filePath);

	if (jsonContent.empty())
	{
		std::cerr << "Failed to read JSON file." << std::endl;
		return 1;
	}

	std::vector<std::unique_ptr<Protein>> data = parseJsonArray(jsonContent);

    std::cout << data.at(0)->getReference().length()/data.at(0)->sequenceLength() << std::endl;

    outputModifiedProteins(data);

	/* for (const auto &protein : data)
	{
		protein->toString();
		std::cout << "Percentage disorder: " << protein->percentageDisorder() << std::endl;
		std::cout << "-------------------" << std::endl;
	}

	std::cout << "Average protein length: " << avgProteinLength(data) << std::endl;
	std::cout << "Average protein disorder: " << avgDisorder(data) << "\n"
			  << std::endl;
	std::cout << "Most disordered protein" << std::endl;
	Protein *protein = data.at(mostDisorderedProtein(data)).get();
	protein->toString();
	std::cout << "Percentage disorder: " << protein->percentageDisorder() << "\n"
			  << std::endl;
	std::unordered_map<char, int> frequency = disorderFrequencyOfAminoAcids(data);
	int total = totalNumAminoAcids(data);
	std::cout << "Disorder probability for each amino acid" << std::endl;
	for (const auto &aa : frequency)
	{
		double probability = static_cast<double>(aa.second) / total * 100.0;
		std::cout << aa.first << ": " << probability << std::endl;
	}*/
}
