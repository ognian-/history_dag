#pragma once

#include <map>

#include "mutation.hpp"

class CompactGenome {
public:

    inline void AddMutations(CollectionOf<Mutation> auto mutations);
    inline CollectionOf<Mutation> auto GetMutations() const;

    inline bool operator==(const CompactGenome& rhs) const;
    inline bool operator<(const CompactGenome& rhs) const;

private:
    std::map<MutationPosition, std::pair<char, char>> mutations_;
};

#include "inl/compact_genome_inl.hpp"
