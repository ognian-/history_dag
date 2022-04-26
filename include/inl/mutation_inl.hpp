Mutation::Mutation(MutationPosition position, char parent_nucleotide,
    char mutated_nucleotide) :
        position_{position},
        parent_nucleotide_{parent_nucleotide},
        mutated_nucleotide_{mutated_nucleotide} {}
        
	
MutationPosition Mutation::GetPosition() const { return position_; }

char Mutation::GetParentNucleotide() const { return parent_nucleotide_; }

char Mutation::GetMutatedNucleotide() const { return mutated_nucleotide_; }

bool Mutation::operator<(const Mutation& rhs) const {
    return position_ < rhs.position_ &&
        mutated_nucleotide_ < rhs.mutated_nucleotide_ &&
        parent_nucleotide_ < rhs.parent_nucleotide_;
}

bool Mutation::operator==(const Mutation& rhs) const {
    return position_ == rhs.position_ &&
        mutated_nucleotide_ == rhs.mutated_nucleotide_ &&
        parent_nucleotide_ == rhs.parent_nucleotide_;
}
