#ifndef GRAPH_HPP
#define GRAPH_HPP

#include <unordered_map>
#include <queue>
#include <vector>
#include <ostream>
#include <string>

struct Edge {
    std::size_t src;
    std::size_t dst;
    double w;
};

inline bool operator<(Edge const& l, Edge const& r) {
    return l.w < r.w;
}

inline std::ostream& operator<<(std::ostream & out, Edge const& e) {
    out << "Edge(" << e.src << " -> " << e.dst << " : " << e.w << ")";
    return out;
}

class Graph {
    std::unordered_map<std::size_t, std::unordered_map<std::size_t, double> > edges;

public:
    void add_edge(std::size_t const src, std::size_t const dst, double const w);
    void set_edge(std::size_t const src, std::size_t const dst, double const w);
    void merge_nodes(std::size_t const a, std::size_t const b, std::size_t const dst);

    double get_edge(std::size_t const src, std::size_t const dst) const;
    std::unordered_map<std::size_t, double> const& get_edges(std::size_t const src);
    std::vector<std::size_t> get_nodes() const;
    std::size_t nb_edges() const;

    std::priority_queue<Edge> get_edge_set() const;
    void to_dot_file(std::string const& path) const;
};

#endif
