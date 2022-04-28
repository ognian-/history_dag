void CompactGenome::SetMutations(CollectionOf<Mutation> auto mutations,
    bool keep_trivial) {
    mutations_ = {};
    for (const Mutation& mutation : mutations) {
        mutations_[mutation.GetPosition()] = {mutation.GetParentNucleotide(),
                mutation.GetMutatedNucleotide()};
    }
    if (not keep_trivial) RemoveTrivial();
}

void CompactGenome::AddMutations(CollectionOf<Mutation> auto mutations) {
    for (const Mutation& mutation : mutations) {
        auto i = mutations_.find(mutation.GetPosition());
        if (i != mutations_.end()) {
            i->second.first = mutation.GetParentNucleotide();
        } else {
            auto& [parent, mutated] = mutations_[mutation.GetPosition()];
            parent = mutation.GetParentNucleotide();
            mutated = mutation.GetMutatedNucleotide();
        }
    }
    RemoveTrivial();
}

CollectionOf<Mutation> auto CompactGenome::GetMutations() const {
    return mutations_ | std::views::transform([](const auto& i) {
        return Mutation{i.first, i.second.first, i.second.second};
    });
}

char CompactGenome::GetParentNucleotideAt(MutationPosition position) const {
    return mutations_.at(position).first;
}

char CompactGenome::GetMutatedNucleotideAt(MutationPosition position) const {
    return mutations_.at(position).second;
}

bool CompactGenome::operator==(const CompactGenome& rhs) const {
    return mutations_ == rhs.mutations_;
}

bool CompactGenome::operator<(const CompactGenome& rhs) const {
    return mutations_ < rhs.mutations_;
}

void CompactGenome::RemoveTrivial() {
    std::erase_if(mutations_,
    [](const std::pair<MutationPosition, std::pair<char, char>>& i) {
        return i.second.first == i.second.second;
    });
}
