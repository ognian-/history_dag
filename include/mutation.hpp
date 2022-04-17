#pragma once

#include <vector>

#include "history_dag_common.hpp"

class Mutation {
public:
    Mutation() = default;
    inline Mutation(size_t position, char reference_nucleotide,
		char parent_nucleotide, const auto& mutated_nucleotide);
	
	inline size_t GetPosition() const;
    inline char GetReferenceNucleotide() const;
    inline char GetParentNucleotide() const;
    inline CollectionOf<char> auto GetMutatedNucleotide() const;

    inline bool operator<(const Mutation& rhs) const;

private:
    size_t position_ = NoId;
    char reference_nucleotide_ = 0;
    char parent_nucleotide_ = 0;
    std::vector<char> mutated_nucleotide_ = {};
};
