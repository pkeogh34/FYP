//
// Created by Patrick Ross Keogh on 18/01/2024.
//


#ifndef FYP_PROTEIN_H
#define FYP_PROTEIN_H

#include <iostream>
#include <string>
#include <unordered_map>
#include <algorithm>

class Protein {
public:
    Protein();
    ~Protein();

    std::string getId() const;
    void setId(const std::string& newId);

    std::string getSequence() const;
    void setSequence(const std::string& newSequence);

    std::string getReference() const;
    void setReference(const std::string& newReference);

    int sequenceLength() const;

    std::unordered_map<char, int> aminoAcidFrequency() const;

    double percentageOfDisorder() const;

    void toString() const;

private:
    std::string id;
    std::string sequence;
    std::string reference{};
};


#endif //FYP_PROTEIN_H
