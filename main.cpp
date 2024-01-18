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

    for (const auto& d : data) {
        d.toString();
        std::cout << "-------------------" << std::endl;
    }

    return 0;
}


