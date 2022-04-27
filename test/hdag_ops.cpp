#include "hdag_ops.hpp"

#include <numeric>
#include <iostream>

static void GenerateParents(HistoryDAG& dag,
	const std::vector<size_t>& children) {
	
	std::vector<size_t> parents;
	
	for (size_t i = 0; i < children.size(); i += 2) {
		Node parent = dag.AddNode({dag.GetNodes().size()});
		dag.AddEdge({dag.GetEdges().size()}, parent,
			dag.GetNode({children[i]}), {0});
		if (i + 1 < children.size()) {
			dag.AddEdge({dag.GetEdges().size()}, parent,
				dag.GetNode({children[i + 1]}), {0});
		} else {
			dag.AddEdge({dag.GetEdges().size()}, parent,
				dag.GetNode({children[i - 1]}), {1});
		}
		parents.push_back(parent.GetId().value);
	}
	
	if (parents.size() == 2) {
		Node root = dag.AddNode({dag.GetNodes().size()});
		dag.AddEdge({dag.GetEdges().size()}, root,
			dag.GetNode({parents[0]}), {0});
		dag.AddEdge({dag.GetEdges().size()}, root,
			dag.GetNode({parents[1]}), {0});
	} else {
		auto rng = std::default_random_engine{};
		std::shuffle(parents.begin(), parents.end(), rng);
		GenerateParents(dag, parents);
	}
}

HistoryDAG GenerateRandomDag(std::vector<std::string> leaf_sequences) {

	auto rng = std::default_random_engine{};
	std::shuffle(leaf_sequences.begin(), leaf_sequences.end(), rng);

	HistoryDAG dag;
	for (size_t i = 0; i < leaf_sequences.size(); ++i) {
		dag.AddNode({i});
	}
	std::vector<size_t> leaf_ids;
	leaf_ids.resize(leaf_sequences.size());
	std::iota(leaf_ids.begin(), leaf_ids.end(), 0);
	std::shuffle(leaf_ids.begin(), leaf_ids.end(), rng);
	
	GenerateParents(dag, leaf_ids);
	
	dag.BuildConnections();

	return dag;
}

HistoryDAG GenerateBinaryTree() {
    HistoryDAG dag;
    for (size_t i = 0; i < 15; ++i) dag.AddNode({i});
    dag.AddEdge({0}, dag.GetNode({8}), dag.GetNode({0}), {0});
    dag.AddEdge({1}, dag.GetNode({8}), dag.GetNode({1}), {0});
    dag.AddEdge({2}, dag.GetNode({9}), dag.GetNode({2}), {0});
    dag.AddEdge({3}, dag.GetNode({9}), dag.GetNode({3}), {0});
    dag.AddEdge({4}, dag.GetNode({10}), dag.GetNode({4}), {0});
    dag.AddEdge({5}, dag.GetNode({10}), dag.GetNode({5}), {0});
    dag.AddEdge({6}, dag.GetNode({11}), dag.GetNode({6}), {0});
    dag.AddEdge({7}, dag.GetNode({11}), dag.GetNode({7}), {0});
    dag.AddEdge({8}, dag.GetNode({12}), dag.GetNode({8}), {0});
    dag.AddEdge({9}, dag.GetNode({12}), dag.GetNode({9}), {0});
    dag.AddEdge({10}, dag.GetNode({13}), dag.GetNode({10}), {0});
    dag.AddEdge({11}, dag.GetNode({13}), dag.GetNode({11}), {0});
    dag.AddEdge({12}, dag.GetNode({14}), dag.GetNode({12}), {0});
    dag.AddEdge({13}, dag.GetNode({14}), dag.GetNode({13}), {0});
    dag.BuildConnections();
	return dag;
}

void PrintDag(HistoryDAG& dag) {
	for (auto node : dag.GetNodes()) {
		std::cout << "Node: " << node.GetId().value << "\n";
		std::cout << "  Parents: ";
		for (auto i : node.GetParents()) std::cout << i.GetId().value << " ";
		std::cout << "\n";
		std::cout << "  Clades:\n";
		size_t clade = 0;
		for (auto i : node.GetClades()) {
			std::cout << "    " << clade++ << ": ";
			for (auto j : i) std::cout << j.GetChild().GetId().value << " ";
			std::cout << "\n";
		}
		std::cout << "\n";
	}

	for (auto edge : dag.GetEdges()) {
		std::cout << "Edge: " << edge.GetId().value << "\n";
		std::cout << "  Parent: " << edge.GetParent().GetId().value << "\n";
		std::cout << "  Child: " << edge.GetChild().GetId().value << "\n";
		std::cout << "  Clade: " << edge.GetClade().value << "\n";
		std::cout << "\n";
	}
}

static std::string ToString(Node node, Edge edge) {
	std::string result;
	result += std::to_string(node.GetId().value);
	result += ": ";
	if (!node.IsRoot()) {
		for (auto& i : node.GetFirstParent().GetMutations()) {
			result += i.GetMutatedNucleotide();
		}
	} else {
		for (auto& i : edge.GetMutations()) {
			result += i.GetParentNucleotide();
		}
	}
	return result;
}

void ToDOT(HistoryDAG& dag, std::ostream& out) {
	out << "digraph {\n";
	for (auto i : dag.GetEdges()) {
		std::string parent = ToString(i.GetParent(), i);
		std::string child = ToString(i.GetChild(), i);
		out << "  \"" << parent << "\" -> \"" << child << "\"\n";
	}
	out << "}\n";
}

static std::string ToString(Node node, Edge edge,
	const std::vector<std::vector<std::set<NodeId>>>& clade_sets) {
	std::string result;
	result += std::to_string(node.GetId().value);
	result += ": ";
	if (!node.IsRoot()) {
		for (auto& i : node.GetFirstParent().GetMutations()) {
			result += i.GetMutatedNucleotide();
		}
	} else {
		for (auto& i : edge.GetMutations()) {
			result += i.GetParentNucleotide();
		}
	}

	result += " [";
	bool remove_clade_sep = false;
	for (auto& clade : clade_sets.at(node.GetId().value)) {
		bool remove_leaf_sep = false;
		for (auto leaf_id : clade) {
			Node leaf = node.GetDAG().GetNode(leaf_id);
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

void ToDOT(HistoryDAG& dag, const std::vector<std::vector<std::set<NodeId>>>& clade_sets,
    std::ostream& out) {
	out << "digraph {\n";
	for (auto i : dag.GetEdges()) {
		std::string parent = ToString(i.GetParent(), i, clade_sets);
		std::string child = ToString(i.GetChild(), i, clade_sets);
		out << "  \"" << parent << "\" -> \"" << child << "\"\n";
	}
	out << "}\n";
}
