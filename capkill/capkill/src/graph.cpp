#include "graph.hpp"

#include <fstream>

void Graph::add_edge(std::size_t const src, std::size_t const dst, double const w) {
    edges[src][dst] += w;
    edges[dst][src] += w;
}

void Graph::set_edge(std::size_t const src, std::size_t const dst, double const w) {
    edges[src][dst] = w;
    edges[dst][src] = w;
}

double Graph::get_edge(std::size_t const src, std::size_t const dst) const {
    auto it = edges.find(src);
    if(it == edges.end()) {
        return 0;
    }
    auto it2 = it->second.find(dst);
    if(it2 == it->second.end()) {
        return 0;
    }
    return it2->second;
}

std::unordered_map<std::size_t, double> const& Graph::get_edges(std::size_t const src) {
    return edges[src];
}

void Graph::merge_nodes(std::size_t const a, std::size_t const b, std::size_t const dst) {
    auto edges_a = edges[a];
    auto edges_b = edges[b];
    
    for(auto const& it : edges_a) {
        edges[it.first].erase(a);
        if(it.first != a && it.first != b) {
            add_edge(dst, it.first, it.second);
        }
    }

    for(auto const& it : edges_b) {
        edges[it.first].erase(b);
        if(it.first != a && it.first != b) {
            add_edge(dst, it.first, it.second);
        }
    }
    edges.erase(a);
    edges.erase(b);
}

std::size_t Graph::nb_edges() const {
    std::size_t res = 0;

    for(auto const& e : edges) {
        res += e.second.size();
    }

    return res / 2;
}

std::vector<std::size_t> Graph::get_nodes() const {
    std::vector<std::size_t> res;

    for(auto const& e: edges) {
        res.push_back(e.first);
    }

    return res;
}

std::priority_queue<Edge> Graph::get_edge_set() const {
    std::priority_queue<Edge> res;

    for(auto const& er : edges) {
        for(auto const& el : er.second) {
            if(er.first < el.first) {
                res.emplace(er.first, el.first, el.second);
            }
        }
    }

    return res;
}

void Graph::to_dot_file(std::string const& path) const {
    std::ofstream out(path);
    out << "graph graphname {\n";

    for(auto const& er : edges) {
        for(auto const& el : er.second) {
            if(er.first < el.first) {
                out << "\"" << er.first << "\" -- \"" << el.first << "\" [label=\"" << el.second << "\"];\n";
            }
        }
    }

    out << "}";
}
