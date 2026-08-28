#ifndef ORDERING_HPP
#define ORDERING_HPP

#include <vector>

#include <networkit/structures/Partition.hpp>

#include "cnf.hpp"

struct CLevel {
    std::size_t cid;
    std::size_t level;
    double prob;

    CLevel(std::size_t id) : cid(id), level(0), prob(0) {}
    CLevel(std::size_t id, std::size_t l) : cid(id), level(l), prob(0) {}
};

/**
  * \returns The modularity as obtained by the parallel louvain method available in networkit, computed on the variable incidence graph
  */
double compute_modularity(CNF const& cnf);

/**
  * \return The networkit partition as computed by the parallel louvain method for the CNF on the variable incidence graph
  */
NetworKit::Partition compute_community(CNF const& cnf);

/**
  * \returns The clause ids as found in the formula (only active clauses)
  */
std::vector<std::size_t> get_active_clause_ids(CNF const& cnf);

/**
  * \returns The clause ids ordered according to ascending clause size (only active clauses)
  */
std::vector<std::size_t> ascending_clause_ordering(CNF const& cnf);

/**
  * \returns The clause ids lexicographically ordered according to the community based ordering and ascending clause size (only active clauses)
  */
std::vector<std::size_t> compute_clause_ordering(CNF const& cnf);

/**
  * \returns The clause ids in a random order (only active clauses)
  */
std::vector<std::size_t> random_clause_ordering(CNF const& cnf);

/**
  * \returns A partial ordering on the clauses based on the community structure
  */
std::vector<CLevel> compute_partial_clause_ordering(CNF const& cnf);

#endif
