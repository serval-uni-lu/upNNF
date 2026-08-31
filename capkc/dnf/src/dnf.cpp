#include <iostream>
#include <fstream>
#include <queue>
#include <unordered_map>
#include <chrono>

#include <boost/program_options.hpp>

#include "cnf.hpp"
#include "compiler.hpp"
#include "heuristics.hpp"

namespace po = boost::program_options;

po::options_description get_program_options() {
    po::options_description desc("Allowed options");
    desc.add_options()
        ("help,h", "Display help message")
        ("cnf", po::value<std::string>(), "path to CNF file")
        ("timeout", po::value<std::size_t>()->default_value(3600), "maximum amount of time allowed to generate cubes in seconds")
        ("n", po::value<std::size_t>()->default_value(10000), "maximum number of cubes to generate");

    return desc;
}

std::size_t overlap_size(BitSet<std::size_t> a, BitSet<std::size_t> b ) {
    a.intersect(b);
    return a.size();
}

template<typename T>
void intersection(std::unordered_set<T> & a, std::unordered_set<T> const& b) {
    for(auto it = a.begin(); it != a.end(); ) {
        if(b.find(*it) != b.end()) {
            it++;
        }
        else {
            it = a.erase(it);
        }
    }
}

template<typename H>
std::vector<LitSet > compile_main(H heuristic, CNF const& cnf_ign, CNF const& cnf_smp, std::size_t const N, std::size_t const timeout) {
    Compiler compiler(cnf_ign, cnf_smp, heuristic);
    compiler.init();
    compiler.compile(N, timeout);

    Cubes const& cube_manager = compiler.cube_manager();

    std::cout << "Generated " << cube_manager.nb_cubes() << " cubes\n\n";

    std::vector<LitSet > cubes;

    for(Cubes::Cube const& c : cube_manager) {
        cubes.push_back(c.mlits);
    }

    return cubes;
}

BitSet<std::size_t> compute_known(CNF const& cnf, LitSet const& cube) {
    BitSet<std::size_t> res;

    for(std::size_t i = 0; i < cnf.nb_clauses(); i++) {
        if(cnf.is_active(i)) {
            bool is_sat = false;
            for(Literal const l : cnf.clause(i)) {
                if(cube.contains(l)) {
                    is_sat = true;
                    break;
                }
            }

            if(is_sat) {
                res.insert(i);
            }
        }
    }

    return res;
}

int main(int argc, char** argv) {
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

        std::string const cnf_path = vm["cnf"].as<std::string>();
        std::size_t const N = vm["n"].as<std::size_t>();
        std::size_t const timeout = vm["timeout"].as<std::size_t>();

        std::cout << "Loading\n";

        CNF cnf_ign(cnf_path + ".ign");
        CNF cnf_smp(cnf_path + ".smp");
        //NNF nnf(cnf_path + ".unnf");

        //MOMSVarBase heuristic;
        //MOMSLitBase heuristic;
        //MOMSVarPicker heuristic;
        //MOMSLitPicker heuristic;
        LitCount heuristic;
        //LitCountSMP heuristic;
        //LitWeight heuristic;
        //LitWeightSMP heuristic;
        //VarCount heuristic;
        //VarWeight heuristic;
        //VarWeightSMP heuristic;
        //VarCountSMP heuristic;
        //NNFLookahead heuristic(nnf);

        auto const start = std::chrono::high_resolution_clock::now();

        std::cout << "Compiling\n";
        auto cubes = compile_main(heuristic, cnf_ign, cnf_smp, N, timeout);

        auto const compile_end = std::chrono::high_resolution_clock::now();

        std::vector<BitSet<std::size_t> > known(cubes.size());

        std::cout << "Computing known clauses\n";

        #pragma omp parallel for
        for(std::size_t i = 0; i < cubes.size(); i++) {
            known[i] = compute_known(cnf_ign, cubes[i]);
        }

        auto const known_compute_end = std::chrono::high_resolution_clock::now();

        for(std::size_t i = 0; i < cubes.size(); i++) {
            std::cout << "csv " << i << ", " << known[i].size() << "\n";
        }

        std::ofstream out_file(cnf_path + ".cubes");
        for(std::size_t cube_id = 0; cube_id < cubes.size(); cube_id++) {
            for(int i = 1; i <= cnf_ign.nb_vars(); i++) {
                Literal lp(i);
                Literal ln = ~lp;

                if(cubes[cube_id].contains(lp)) {
                    out_file << lp << " ";
                }
                if(cubes[cube_id].contains(ln)) {
                    out_file << ln << " ";
                }
            }
            //for(Literal const l : cubes[cube_id]) {
            //    out_file << l << " ";
            //}
            out_file << "0\n";
        }
        out_file.close();

        std::chrono::duration<double, std::milli> const compile_duration = compile_end - start;
        std::chrono::duration<double, std::milli> const known_compute_duration = known_compute_end - compile_end;

        std::cout << "t Compile: " << compile_duration.count() << " ms\n";
        std::cout << "t known compute: " << known_compute_duration.count() << " ms\n";
    }
    catch(std::exception & e) {
        std::cerr << "ERROR: " << e.what() << "\n";
        return 1;
    }

    return 0;
}
