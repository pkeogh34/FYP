#include "JsonParse/JsonParse.h"

int main() {
    std::string filePath = "IDRdataset.json";
    std::string jsonContent = readFileIntoString(filePath);
    if (jsonContent.empty()) {
        std::cerr << "Failed to read JSON file." << std::endl;
        return 1;
    }

    std::vector<ProteinData> data = parseJsonArray(jsonContent);

    for (const auto& d : data) {
        std::cout << "ID: " << d.id << std::endl;
        std::cout << "Sequence: " << d.sequence << std::endl;
        std::cout << "Reference: " << d.reference << std::endl;
        std::cout << "-------------------" << std::endl;
    }

    return 0;
}


