#include <iostream>
#include <fstream>
#include <filesystem>
// #include <random>
// #include <unordered_set>
// #include <algorithm>

#include <boost/program_options.hpp>

#include <networkit/auxiliary/Log.hpp>

#include "cnf.hpp"
#include "ordering.hpp"
#include "util.hpp"

namespace po = boost::program_options;

enum class ClauseOrder {
    Ascending,
    Community,
    Random
};

void run(std::string const& cnf_path, double const N, ClauseOrder const& co) {
    CNF cnf(cnf_path);
    cnf.simplify();
    cnf.subsumption();

    std::vector<std::size_t> clids;

    switch(co) {
        case ClauseOrder::Ascending:
            clids = ascending_clause_ordering(cnf);
            break;
        case ClauseOrder::Community:
            clids = compute_clause_ordering(cnf);
            break;
        case ClauseOrder::Random:
            clids = random_clause_ordering(cnf);
            break;
    }

    std::size_t low = clids.size();
    if(N < 1.0) {
        low -= N * clids.size() - 1;
    }
    else {
        if(N >= low) {
            low = 0;
        }
        else {
            low -= N;
        }
    }

    {
        for(std::size_t i = 0; i < clids.size(); i++) {
            cnf.set_active(clids[i], i < low);
        }

        std::string lp = cnf_path + ".up";
        std::ofstream out(lp);
        out << cnf;
        out.close();
    }

    {
        for(std::size_t i = 0; i < clids.size(); i++) {
            cnf.set_active(clids[i], i >= low);
        }

        std::string lp = cnf_path + ".ign";
        std::ofstream out(lp);
        out << cnf;
        out.close();
    }

    {
        for(std::size_t i = 0; i < clids.size(); i++) {
            cnf.set_active(clids[i], true);
        }

        std::string lp = cnf_path + ".smp";
        std::ofstream out(lp);
        out << cnf;
        out.close();
    }
}

po::options_description get_program_options() {
    po::options_description desc("Allowed options");
    desc.add_options()
        ("help,h", "Display help message")
        ("cnf", po::value<std::string>(), "path to CNF file")
        ("n", po::value<double>()->default_value(100), "set the number of clauses to remove")
        ("ordering", po::value<std::string>()->default_value("community"), "set clause ordering: community, ascending, random");

    return desc;
}

int main(int argc, char** argv) {
    Aux::Log::setLogLevel("ERROR"); // Only show errors and above

    try {
        po::options_description desc = get_program_options();

        po::variables_map vm;
        po::store(po::parse_command_line(argc, argv, desc), vm);
        po::notify(vm);

        if(vm.count("help")) {
            std::cout << desc << "\n";
            return 0;
        }

        if(! vm.count("cnf")) {
            std::cerr << "ERROR: cnf option not set\n";
            return 1;
        }

        //std::cerr << "D4 path: \"" << get_d4_path() << "\"\n";

        std::string const cnf_path = vm["cnf"].as<std::string>();
        double const N = vm["n"].as<double>();
        std::string const strco = to_lower_case(vm["ordering"].as<std::string>());

        ClauseOrder co = ClauseOrder::Community;
        if("community" == strco) {
            co = ClauseOrder::Community;
        }
        else if("ascending" == strco) {
            co = ClauseOrder::Ascending;
        }
        else if("random" == strco) {
            co = ClauseOrder::Random;
        }
        else {
            std::cerr << "ERROR: unknown clause ordering: " + strco + "\nexiting\n";
            return 1;
        }

        run(cnf_path, N, co);
    }
    catch(std::exception & e) {
        std::cerr << "ERROR: " << e.what() << "\n";
        return 1;
    }

    return 0;
}
