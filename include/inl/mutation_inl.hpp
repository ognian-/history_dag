Mutation::Mutation(size_t position, char reference_nucleotide,
    char parent_nucleotide, const auto& mutated_nucleotide) :
        position_{position},
        reference_nucleotide_{reference_nucleotide},
        parent_nucleotide_{parent_nucleotide},
        mutated_nucleotide_{std::begin(mutated_nucleotide),
            std::end(mutated_nucleotide)} {}
	
size_t Mutation::GetPosition() const { return position_; }

char Mutation::GetReferenceNucleotide() const { return reference_nucleotide_; }

char Mutation::GetParentNucleotide() const { return parent_nucleotide_; }

CollectionOf<char> auto Mutation::GetMutatedNucleotide() const {
    return std::views::all(mutated_nucleotide_);
}

bool Mutation::operator<(const Mutation& rhs) const {
    return position_ < rhs.position_ &&
        reference_nucleotide_ < rhs.reference_nucleotide_ &&
        parent_nucleotide_ < rhs.parent_nucleotide_ &&
        mutated_nucleotide_ < rhs.mutated_nucleotide_;
}

bool Mutation::operator==(const Mutation& rhs) const {
    return position_ == rhs.position_ &&
        reference_nucleotide_ == rhs.reference_nucleotide_ &&
        parent_nucleotide_ == rhs.parent_nucleotide_ &&
        mutated_nucleotide_ == rhs.mutated_nucleotide_;
}
