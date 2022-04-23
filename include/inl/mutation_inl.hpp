Mutation::Mutation(MutationPosition position, char parent_nucleotide,
    char reference_nucleotide) :
        position_{position},
        parent_nucleotide_{parent_nucleotide},
        reference_nucleotide_{reference_nucleotide} {}
        
	
MutationPosition Mutation::GetPosition() const { return position_; }

char Mutation::GetParentNucleotide() const { return parent_nucleotide_; }

char Mutation::GetReferenceNucleotide() const { return reference_nucleotide_; }

bool Mutation::operator<(const Mutation& rhs) const {
    return position_ < rhs.position_ &&
        reference_nucleotide_ < rhs.reference_nucleotide_ &&
        parent_nucleotide_ < rhs.parent_nucleotide_;
}

bool Mutation::operator==(const Mutation& rhs) const {
    return position_ == rhs.position_ &&
        reference_nucleotide_ == rhs.reference_nucleotide_ &&
        parent_nucleotide_ == rhs.parent_nucleotide_;
}
