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

/**
  * \brief does one level/iteration of the capkc n-ary search algorithm
  * \param cnf_path the path to the original CNF file
  * \param cnf the original CNF formula
  * \param clids the clause IDs ordered according to the chosen ordering and limited to only active clauses
  * \param low the index (in clids) of the prefix formula that is assumed to compile with D4
  * \param high the index (in clids) of the prefix formula that is assumed not to compile with D4
  * \param N the number of parallel tries to do
  * \param mem the maximum amount of memory in MB for each individual try
  * \param time the maximum amount of time in seconds for each individual try
  */
RunRes test(std::string const& cnf_path, CNF & cnf, std::vector<std::size_t> const& clids, std::size_t & low, std::size_t & high, std::size_t const& N, std::size_t const& mem, std::size_t const& time) {
    std::cout << cnf_path << ":\n";
    std::cout << "    " << low << " -> " << high << "\n";
    std::size_t const range = high - low + 1;
    std::size_t const step = std::max(1UL, range / (N + 1));

    std::cout << "    range " << range << "\n";
    std::cout << "    step " << step << "\n";

    std::vector<std::string> paths;
    std::vector<std::size_t> nbcls;

    for(std::size_t const cid : clids) {
        cnf.set_active(cid, false);
    }

    for(std::size_t j = 0; j <= low; j++) {
        cnf.set_active(clids[j], true);
    }

    for(std::size_t i = low + step; i < high && paths.size() < N; i += step) {
        for(std::size_t j = i - step; j <= i; j++) {
            cnf.set_active(clids[j], true);
        }

        std::cout << "    c " << i << "\n";

        std::string lp = cnf_path + ".up" + std::to_string(i);
        std::ofstream out(lp);
        out << cnf;
        out.close();

        paths.push_back(lp);
        nbcls.push_back(i);
    }

    for(std::size_t const cid : clids) {
        cnf.set_active(cid, true);
    }

    auto res = run_many(paths, time, mem);

    for(std::size_t i = 0; i < res.size(); i++) {
        std::cout << "csv ";
        std::cout << cnf_path;
        std::cout << ", " << cnf.nb_vars();
        std::cout << ", " << nbcls[i];
        std::cout << ", " << res[i].status;
        std::cout << ", " << res[i].mem;
        std::cout << ", " << res[i].time.count();
        std::cout << ", " << res[i].output;
        std::cout << "\n";
    }

    RunRes rr;
    rr.status = RunStatus::Void;
    bool cont = true;
    for(int i = res.size() - 1; i >= 0 && cont; i--) {
        std::cout << "    r";
        std::cout << " i:" << nbcls[i];
        std::cout << " status:" << res[i].status;
        switch(res[i].status) {
            case RunStatus::Void:
                std::cerr << cnf_path << ":: CRASH OCCURED: state == void\n";
                exit(1);
                break;
            case RunStatus::Running:
                std::cerr << cnf_path << ":: CRASH OCCURED: state == running\n";
                exit(1);
                break;
            case RunStatus::Error:
                std::cerr << cnf_path << ":: CRASH OCCURED: state == error\n";
                exit(1);
                break;
            case RunStatus::Impossible:
                std::cerr << cnf_path << ":: CRASH OCCURED: state == impossible\n";
                exit(1);
                break;
            case RunStatus::Done:
                low = nbcls[i];
                rr = res[i];
                if(i + 1 < static_cast<int>(res.size())) {
                    high = nbcls[i + 1];
                }

                cont = false;
                std::cout << " s:\"" << res[i].output << "\"";
                break;
            case RunStatus::Memory:
            case RunStatus::Time:
                high = nbcls[i];
                rr = res[i];
                break;
        }

        std::cout << "\n";
    }
    
    for(auto const& ir : res) {
        std::filesystem::remove(ir.input);
    }

    std::cout << "    >>" << low << " -> " << high << "\n";
    return rr;
}

enum class ClauseOrder {
    Ascending,
    Community,
    Random
};

void run(std::string const& cnf_path, std::size_t const mem, std::size_t const time, std::size_t const nthreads, ClauseOrder const& co, std::size_t const ilow, std::size_t const ihigh) {
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

    std::size_t low = ilow;
    std::size_t high = clids.size();
    if(ihigh > 0 && ihigh < clids.size()) {
        high = ihigh;
    }
    std::size_t nb = 0;
    RunRes rr;
    while(low + 1 < high) {
        std::cout << "low: " << low << " -- high: " << high << "\n";
        RunRes lrr = test(cnf_path, cnf, clids, low, high, nthreads, mem, time);
        if(rr.input != lrr.input && lrr.status == RunStatus::Done) {
            //std::filesystem::remove(rr.input);
            //std::filesystem::remove(rr.nnf_path);
            rr = lrr;
        }

        std::cout << "s " << rr.output << "\n";
        std::cout << "o " << rr.input << "\n";
        nb++;
    }
    //std::cout << "low: " << low << " -- high: " << high << "\n";
    std::cout << "nb tries: " << nb << "\n";

    //std::filesystem::remove(rr.input);
    //std::filesystem::remove(rr.nnf_path);

    {
        for(std::size_t i = 0; i < clids.size(); i++) {
            cnf.set_active(clids[i], i <= low);
        }

        std::string lp = cnf_path + ".up";
        std::ofstream out(lp);
        out << cnf;
        out.close();
    }

    {
        for(std::size_t i = 0; i < clids.size(); i++) {
            cnf.set_active(clids[i], i > low);
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
        ("n", po::value<std::size_t>()->default_value(2), "set the number of parallel tries")
        ("mem", po::value<std::size_t>()->default_value(4000), "set maximum amount of memory per try (in MB)")
        ("time", po::value<std::size_t>()->default_value(3600), "set maximum amount of time per try (in s)")
        ("low", po::value<std::size_t>()->default_value(1), "set minimum number of clauses")
        ("high", po::value<std::size_t>()->default_value(0), "set maximum number of clauses")
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

        std::cerr << "D4 path: \"" << get_d4_path() << "\"\n";

        std::string const cnf_path = vm["cnf"].as<std::string>();
        std::size_t const nthreads = vm["n"].as<std::size_t>();
        std::size_t const time = vm["time"].as<std::size_t>();
        std::size_t const mem = vm["mem"].as<std::size_t>();
        std::size_t const low = vm["low"].as<std::size_t>();
        std::size_t const high = vm["high"].as<std::size_t>();
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

        run(cnf_path, mem, time, nthreads, co, low, high);
    }
    catch(std::exception & e) {
        std::cerr << "ERROR: " << e.what() << "\n";
        return 1;
    }

    return 0;
}
