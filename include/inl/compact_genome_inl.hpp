void CompactGenome::AddMutations(CollectionOf<Mutation> auto mutations) {
    for (const Mutation& mutation : mutations) {
        auto i = mutations_.find(mutation.GetPosition());
        if (i == mutations_.end()) {
            mutations_[mutation.GetPosition()] = {mutation.GetParentNucleotide(),
                mutation.GetReferenceNucleotide()};
        } else {
            auto& [parent, reference] = i->second;
            assert(mutation.GetParentNucleotide() == reference);
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
