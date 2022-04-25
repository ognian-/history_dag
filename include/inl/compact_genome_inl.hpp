void CompactGenome::AddMutations(CollectionOf<Mutation> auto mutations) {
    for (const Mutation& mutation : mutations) {
        auto i = mutations_.find(mutation.GetPosition());
        if (i == mutations_.end()) {
            mutations_[mutation.GetPosition()] = {mutation.GetParentNucleotide(),
                mutation.GetReferenceNucleotide()};
        } else {
            auto& [parent, reference] = i->second;
            if (mutation.GetParentNucleotide() != reference) {
                // Warning: old base does not match existing base in CG.
                continue;
            }
            reference = mutation.GetReferenceNucleotide();
            if (reference == parent) {
                mutations_.erase(i);
            }
        }
    }
}

CollectionOf<Mutation> auto CompactGenome::GetMutations() const {
    return mutations_ | std::views::transform([](const auto& i) {
        return Mutation{i.first, i.second.first, i.second.second};
    });
}

bool CompactGenome::operator==(const CompactGenome& rhs) const {
    return mutations_ == rhs.mutations_;
}

bool CompactGenome::operator<(const CompactGenome& rhs) const {
    return mutations_ < rhs.mutations_;
}
