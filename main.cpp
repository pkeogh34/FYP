#include "ChangeFormat/ChangeFormat.h"
#include "DataExploration/DataExploration.h"
#include "JsonParse/JsonParse.h"

int main() {
	std::string filePath = "IDRdataset.json";
	std::string jsonContent = readFileIntoString(filePath);

	if (jsonContent.empty()) {
		std::cerr << "Failed to read JSON file." << std::endl;
		return 1;
	}

	std::vector<std::unique_ptr<Protein>> data = parseJsonArray(jsonContent);

	std::cout << data.at(0)->getReference().length() / data.at(0)->sequenceLength() << std::endl;

	outputModifiedProteins(data);
}
