#include <iostream>

#include "history_dag.hpp"

int main() {
	
	HistoryDAG dag;
	
	dag.AddNode({0}, std::string("hello"));
	dag.AddNode({1}, std::string("world"));
	dag.AddEdge({0}, dag.GetNode({0}), dag.GetNode({1}), 0);
	
	for (auto node : dag.GetNodes()) {
		std::cout << "Node: " << node.GetId().value << "\n";
		std::cout << "  Sequence: ";
		for (char i : node.GetSequence()) std::cout << i;
		std::cout << "\n";
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
	
	std::cout << "\n";
	for (auto edge : dag.GetEdges()) {
		std::cout << "Edge: " << edge.GetId().value << "\n";
		std::cout << "  Parent: " << edge.GetParent().GetId().value << "\n";
		std::cout << "  Child: " << edge.GetChild().GetId().value << "\n";
		std::cout << "  Clade: " << edge.GetClade() << "\n";
		std::cout << "\n";
	}
	
	std::cout << "Done.\n";
	return 0;
}
