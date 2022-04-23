#pragma once

#include <map>

#include "mutation.hpp"

class CompactGenome {
public:

    void AddMutations(CollectionOf<Mutation> auto mutations);
    CollectionOf<Mutation> auto GetMutations() const;

private:
    std::map<MutationPosition, std::pair<char, char>> mutations_;
};

#include "inl/compact_genome_inl.hpp"
