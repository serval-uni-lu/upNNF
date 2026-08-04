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

double compute_modularity(CNF const& cnf);
NetworKit::Partition compute_community(CNF const& cnf);

std::vector<std::size_t> get_active_clause_ids(CNF const& cnf);
std::vector<std::size_t> ascending_clause_ordering(CNF const& cnf);
std::vector<std::size_t> compute_clause_ordering(CNF const& cnf);
std::vector<std::size_t> random_clause_ordering(CNF const& cnf);

std::vector<CLevel> compute_partial_clause_ordering(CNF const& cnf);

#endif
