#include <set>

#include "merge.hpp"

static std::string ToString(Node node, const LeafSet& leaf_set) {
	std::string result;
    
	if (!node.IsRoot()) {
		for (auto& i : node.GetFirstParent().GetMutations()) {
			result += i.GetMutatedNucleotide();
		}
	} else {
		for (auto& i : (*node.GetChildren().begin()).GetMutations()) {
			result += i.GetParentNucleotide();
		}
	}

	result += " [";
	bool remove_clade_sep = false;
	for (auto clade : leaf_set.GetLeafs(node.GetId())) {
		bool remove_leaf_sep = false;
		for (auto leaf : clade) {
			for (auto& i : leaf.GetFirstParent().GetMutations()) {
				result += i.GetMutatedNucleotide();
			}
			result += ",";
			remove_leaf_sep = true;
		}
		if (remove_leaf_sep) result.erase(result.size() - 1);
		result += "|";
		remove_clade_sep = true;
	}
	if (remove_clade_sep) result.erase(result.size() - 1);
	result += "]";

	return result;
}

static const auto RootMutations = [](const Mutation& mutation) -> Mutation {
    return {mutation.GetPosition(), mutation.GetParentNucleotide(),
        mutation.GetParentNucleotide()};
};

HistoryDAG Merge(
    const std::vector<std::reference_wrapper<const HistoryDAG>>& trees) {
    std::vector<LeafSet> leafs;
    for (auto i : trees) leafs.emplace_back(i.get());
    std::unordered_map<std::string, NodeId> nodes;
    std::set<std::pair<std::string, std::string>> edges;
    HistoryDAG result;

    MutableNode ua = result.AddNode({0});

    for (size_t i = 0; i < trees.size(); ++i) {
        for (Node node : trees[i].get().GetNodes()) {
            const std::string key =  ToString(node, leafs[i]);
            auto i = nodes.find(key);
            NodeId id;
            if (i == nodes.end()) {
                id = result.AddNode({result.GetNodes().size()}).GetId();
                nodes[key] = id;
            } else {
                id = nodes.at(key);
            }
            if (node.IsRoot()) {
                if (edges.insert({"p", key}).second) {
                    result.AddEdge({result.GetEdges().size()},
                        ua.GetId(), id, {0})
                        .SetMutations((*node.GetChildren().begin())
                        .GetMutations() | std::views::transform(RootMutations));
                }
            }
        }
    }

    for (size_t i = 0; i < trees.size(); ++i) {
        for (Edge edge : trees[i].get().GetEdges()) {
            const std::pair<std::string, std::string> key =
                {ToString(edge.GetParent(), leafs[i]),
                ToString(edge.GetChild(), leafs[i])};
            if (key.first == key.second) continue;
            auto i = edges.find(key);
            if (i == edges.end()) {
                edges.insert(key);
                result.AddEdge({result.GetEdges().size()},
                    nodes.at(key.first),
                    nodes.at(key.second),
                    edge.GetClade())
                    .SetMutations(edge.GetMutations());
            }
        }
    }

    result.BuildConnections();
    
    return result;
}
