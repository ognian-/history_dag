#pragma once

#include <vector>

#include "history_dag_common.hpp"

class Mutation {
public:
    Mutation() = default;
    inline Mutation(MutationPosition position, char parent_nucleotide,
        char mutated_nucleotide);
	
	inline MutationPosition GetPosition() const;
    inline char GetParentNucleotide() const;
    inline char GetMutatedNucleotide() const;

    inline bool operator<(const Mutation& rhs) const;
    inline bool operator==(const Mutation& rhs) const;

private:
    MutationPosition position_ = {NoId};
    char parent_nucleotide_ = 0;
    char mutated_nucleotide_ = 0;
};

#include "inl/mutation_inl.hpp"
