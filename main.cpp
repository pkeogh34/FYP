#include "JsonParse/JsonParse.h"
#include "DataExploration/DataExploration.h"

int main() {
    std::string filePath = "IDRdataset.json";
    std::string jsonContent = readFileIntoString(filePath);
    if (jsonContent.empty()) {
        std::cerr << "Failed to read JSON file." << std::endl;
        return 1;
    }

    std::vector<Protein> data = parseJsonArray(jsonContent);

    for (const auto& protein : data) {
        protein.toString();
        std::cout << "Percentage disorder: " << protein.percentageDisorder() << std::endl;
        std::cout << "-------------------" << std::endl;
    }

    std::cout << "Average protein length: " << avgProteinLength(data) << std::endl;
    std::cout << "Average protein disorder: " << avgDisorder(data) << "\n" << std::endl;
    std::cout << "Most disordered protein" << std::endl;
    mostDisorderedProtein(data).toString();
    std::cout << "Percentage disorder: " << mostDisorderedProtein(data).percentageDisorder() << "\n" << std::endl;
    std::unordered_map<char, int> frequency = disorderFrequencyOfAminoAcids(data);
    int total = totalNumAminoAcids(data);
    std::cout << "Disorder probability for each amino acid" << std::endl;
    for(const auto& aa : frequency){
        double probability = static_cast<double>(aa.second) / total * 100.0;
        std::cout << aa.first << ": " << probability << std::endl;
    }


    return 0;
}


