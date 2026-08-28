#ifndef GRAPH_HPP
#define GRAPH_HPP

#include <unordered_map>
#include <queue>
#include <vector>
#include <ostream>
#include <string>

struct Edge {
    /**
      * \brief The source ID of the edge
      */
    std::size_t src;

    /**
      * \brief The destination ID of the edge
      */
    std::size_t dst;

    /**
      * \brief The weight of the edge
      */
    double w;
};

/**
  * \return True if l has a smaller weight than r
  */
inline bool operator<(Edge const& l, Edge const& r) {
    return l.w < r.w;
}

inline std::ostream& operator<<(std::ostream & out, Edge const& e) {
    out << "Edge(" << e.src << " -> " << e.dst << " : " << e.w << ")";
    return out;
}

/**
  * \brief An undirected graph structure based on unordered_map used as a scarce adhacency matrix
  */
class Graph {
    std::unordered_map<std::size_t, std::unordered_map<std::size_t, double> > edges;

public:
    /**
      * \brief Adds the edge. If an edge already exists, the weight is incremented.
      */
    void add_edge(std::size_t const src, std::size_t const dst, double const w);

    /**
      * \brief Sets the edge. If an edge already exists, the weight is overwritten.
      */
    void set_edge(std::size_t const src, std::size_t const dst, double const w);

    /**
      * \brief The two nodes, a and b, are merged and replaced by node dst.
      * \details The edges to and from a or b are replaced by edges to and from dst. The nodes a and b are removed. If dst already exists, the edges are added just like add_edge(...).
      */
    void merge_nodes(std::size_t const a, std::size_t const b, std::size_t const dst);

    /**
      * \returns the weight of the edge
      */
    double get_edge(std::size_t const src, std::size_t const dst) const;

    /**
      * \returns the edges of node src
      */
    std::unordered_map<std::size_t, double> const& get_edges(std::size_t const src);

    /**
      * \returns The nodes that have at least one edge.
      */
    std::vector<std::size_t> get_nodes() const;

    /**
      * \returns The number of edges in the graph
      */
    std::size_t nb_edges() const;

    /**
      * \returns A priority queue where each edge is ordered according to its weight.
      */
    std::priority_queue<Edge> get_edge_set() const;

    /**
      * \brief Writes the graph to a file in dot format.
      */
    void to_dot_file(std::string const& path) const;
};

#endif
