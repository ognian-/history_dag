#pragma once

#include <stack>
#include <vector>
#include <string>

template <typename T, typename N, typename E>
void ParseNewick(const T& source, N&& on_node, E&& on_edge) {
    struct Node {
        size_t id;
        std::string label;
    };
    std::stack<std::vector<Node>> nodes;
    std::string label;
    size_t node_id = 0;

    auto begin_node = [&]() {
        nodes.push({});
    };

    auto end_label = [&]() {
        if (!nodes.empty()) {
            for (auto& i : nodes.top()) {
                on_edge(node_id, i.id);
            }
            nodes.pop();
            if (!nodes.empty()) {
                nodes.top().push_back({node_id, label});
            }
        }
        on_node(node_id++, label);
        label = "";
    };

    begin_node();
    for (auto i = source.begin(); i != source.end(); ++i) {
        switch (*i) {
        case '(':
            begin_node();
            break;
        case ',':
            end_label();
            begin_node();
            break;
        case ')':
        case ';':
            end_label();
            break;
        default:
            label += *i;
            break;
        }
    }
}
