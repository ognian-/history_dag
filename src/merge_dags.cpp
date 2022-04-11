#include "merge_dags.hpp"

CladeSet::CladeSet(Node node) {
    Range subtree{PreOrderIterator{node}, PreOrderIterator{}};
    for (Node node : subtree) {
        if (node.IsLeaf()) {
            leafs_.push_back(node.GetId());
        }
    }
}
