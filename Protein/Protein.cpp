//
// Created by Patrick Ross Keogh on 18/01/2024.
//

#include "Protein.h"

std::string Protein::getId() const{
    return id;
}

void Protein::setId(const std::string& newId) {
    id = newId;
}

std::string Protein::getSequence() const{
    return sequence;
}

void Protein::setSequence(const std::string& newSequence) {
    sequence = newSequence;
}

std::string Protein::getReference() const {
    return reference;
}

void Protein::setReference(const std::string& newReference) {
    reference = newReference;
}

int Protein::sequenceLength() const {
    return sequence.length();
}

std::unordered_map<char, int> Protein::aminoAcidFrequency() const {
    std::unordered_map<char, int> frequency;
    for (char aa : sequence) {
        frequency[aa]++;
    }
    return frequency;
}

double Protein::percentageOfDisorder() const {
    int disorderCount = std::count(reference.begin(), reference.end(), '0');
    return 100.0 * disorderCount / sequenceLength();
}

void Protein::toString() const {
    std::cout << "ID: " << id << std::endl;
    std::cout << "Sequence: " << sequence << std::endl;
    std::cout << "Reference: " << reference << std::endl;
}

Protein::Protein() = default;

Protein::~Protein() = default;


