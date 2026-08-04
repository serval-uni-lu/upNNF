#include <random>
#include <unordered_set>
#include <algorithm>

#include <networkit/graph/Graph.hpp>
#include <networkit/community/PLM.hpp>
#include <networkit/community/Modularity.hpp>
#include <networkit/structures/Partition.hpp>
#include <networkit/coarsening/MatchingCoarsening.hpp>
#include <networkit/matching/PathGrowingMatcher.hpp>

#include "ordering.hpp"

NetworKit::Graph build_vig(CNF const& cnf) {
    NetworKit::count node_count = cnf.nb_vars();
    NetworKit::Graph g(node_count, true);

    for(NetworKit::count i = 1; i <= node_count; i++) {
        Literal pl(i);
        Literal nl(-1 * i);

        std::unordered_set<std::size_t> idx = cnf.get_idx(pl);
        idx.insert(cnf.get_idx(nl).begin(), cnf.get_idx(nl).end());

        std::map<NetworKit::node, double> edges;

        for(std::size_t cid : idx) {
            if(cnf.is_active(cid) && cnf.clause(cid).size() > 1) {
                Clause const& cls = cnf.clause(cid);
                double const cw = 2.0 / (cls.size() * (cls.size() - 1));
                // double const cw = pow(2, cls.size()) / (pow(2, cls.size()) - 1);

                for(Literal l : cls) {
                    Variable v(l);

                    if(static_cast<NetworKit::count>(v.to_int()) > i) {
                        edges[v.get()] += cw;
                    }
                }
            }
        }

        for(auto const& p : edges) {
            g.addEdge(i - 1, p.first, p.second);
        }
    }

    return g;
}

std::pair<NetworKit::Graph, std::vector<NetworKit::node> > coarsen_graph(NetworKit::Graph const& g) {
    NetworKit::PathGrowingMatcher matcher(g);
    matcher.run();
    NetworKit::Matching m = matcher.getMatching();

    NetworKit::MatchingCoarsening coarsener(g, m, true);
    coarsener.run();
    auto cp = coarsener.getFineToCoarseNodeMapping();
    auto rg = coarsener.getCoarseGraph();

    return std::pair(rg, cp);
}

std::pair<NetworKit::Graph, std::vector<NetworKit::node> > initial_coarsen(NetworKit::Graph const& g) {
    NetworKit::PLM plm(g);
    plm.run();

    NetworKit::Partition p = plm.getPartition();
    return NetworKit::PLM::coarsen(g, p);
}

std::vector<std::vector<NetworKit::node> > clusterize(CNF const& cnf) {
    NetworKit::Graph g = build_vig(cnf);
    std::vector<std::vector<NetworKit::node> > clustering;

    auto coarseResult = initial_coarsen(g);
    coarseResult.first.removeSelfLoops();
    clustering.push_back(coarseResult.second);
    auto gclean = coarseResult.first;

    while(gclean.numberOfEdges() > 0) {
        auto [gc, cp] = coarsen_graph(gclean);
        gc.removeSelfLoops();
        clustering.push_back(cp);
        gclean = gc;
    }
    return clustering;
}

std::vector<std::size_t> get_active_clause_ids(CNF const& cnf) {
    std::vector<std::size_t> clids;

    for(std::size_t cid = 0; cid < cnf.nb_clauses(); cid++) {
        if(cnf.is_active(cid)) {
            clids.push_back(cid);
        }
    }

    return clids;
}

std::vector<std::size_t> ascending_clause_ordering(CNF const& cnf) {
    std::vector<std::size_t> clids = get_active_clause_ids(cnf);

    std::sort(clids.begin(), clids.end(), [&](std::size_t const a, std::size_t const b) {
        return cnf.clause(a).size() < cnf.clause(b).size();
    });

    return clids;
}

std::vector<std::size_t> compute_clause_ordering(CNF const& cnf) {
    std::vector<std::size_t> clids = ascending_clause_ordering(cnf);
    std::vector<std::size_t> res;
    std::vector<std::vector<NetworKit::node> > const clustering = clusterize(cnf);
    std::vector<NetworKit::node> level = clustering[0];

    for(std::size_t i = 0; i < clustering.size(); i++) {
        std::vector<std::size_t> rclids;
        for(std::size_t j = 0; j < clids.size(); j++) {
            Variable const rv = Variable(cnf.clause(clids[j])[0]);

            bool good = true;
            for(Literal const l : cnf.clause(clids[j])) {
                if(level[rv.get()] != level[Variable(l).get()]) {
                    good = false;
                    break;
                }
            }

            if(good) {
                res.push_back(clids[j]);
                //clids.erase(clids.begin() + j);
                //j--;
            }
            else {
                rclids.push_back(clids[j]);
            }
        }
        clids = rclids;

        if(i + 1 < clustering.size()) {
            for(std::size_t j = 0; j < level.size(); j++) {
                level[j] = clustering[i + 1][level[j]];
            }
        }
    }

    return res;
}

std::vector<std::size_t> random_clause_ordering(CNF const& cnf) {
    std::vector<std::size_t> clids = get_active_clause_ids(cnf);

    std::random_device rd;
    std::mt19937 g(rd());
    std::shuffle(clids.begin(), clids.end(), g);

    return clids;
}

std::vector<CLevel> compute_partial_clause_ordering(CNF const& cnf) {
    std::vector<std::size_t> clids = get_active_clause_ids(cnf);
    std::vector<CLevel> res;
    std::vector<std::vector<NetworKit::node> > const clustering = clusterize(cnf);
    std::vector<NetworKit::node> level = clustering[0];

    for(std::size_t i = 0; i < clustering.size(); i++) {
        std::vector<std::size_t> rclids;
        for(std::size_t j = 0; j < clids.size(); j++) {
            Variable const rv = Variable(cnf.clause(clids[j])[0]);

            bool good = true;
            for(Literal const l : cnf.clause(clids[j])) {
                if(level[rv.get()] != level[Variable(l).get()]) {
                    good = false;
                    break;
                }
            }

            if(good) {
                res.emplace_back(clids[j], i);
                //clids.erase(clids.begin() + j);
                //j--;
            }
            else {
                rclids.push_back(clids[j]);
            }
        }
        clids = rclids;

        if(i + 1 < clustering.size()) {
            for(std::size_t j = 0; j < level.size(); j++) {
                level[j] = clustering[i + 1][level[j]];
            }
        }
    }

    return res;
}

double compute_modularity(CNF const& cnf) {
    NetworKit::Graph g = build_vig(cnf);

    NetworKit::PLM plm(g);
    plm.run();
    NetworKit::Partition p = plm.getPartition();

    NetworKit::Modularity modularity;
    return modularity.getQuality(p, g);
}

NetworKit::Partition compute_community(CNF const& cnf) {
    NetworKit::Graph g = build_vig(cnf);
    NetworKit::PLM plm(g);
    plm.run();
    return plm.getPartition();
}
