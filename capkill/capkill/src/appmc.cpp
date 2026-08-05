#include <iostream>
#include <fstream>
#include <filesystem>
#include <limits>
#include <random>
#include <atomic>
#include <algorithm>

#include <boost/program_options.hpp>

#include <boost/math/distributions/normal.hpp>
#include <boost/multiprecision/gmp.hpp>

#include "cnf.hpp"
#include "nnf.hpp"
#include "xoshiro.hpp"

#include "assignment_generator.hpp"

namespace po = boost::program_options;

using boost::multiprecision::mpf_float;
using boost::math::normal;

po::options_description get_program_options() {
    po::options_description desc("Allowed options");
    desc.add_options()
        ("help,h", "Display help message")
        ("verbose", "Display all of the intermediate estimates as well")
        ("cnf", po::value<std::string>(), "path to CNF file")
        ("alpha", po::value<double>()->default_value(0.01), "alpha value for the CLT")
        ("nb", po::value<std::size_t>()->default_value(10'000), "the maximum number of samples to use")
        ("lnb", po::value<std::size_t>()->default_value(0), "the minimum number of samples to use, set to 0 to disable early stopping (see --epsilon)")
        ("epsilon", po::value<double>()->default_value(1.1), "the algorithm stops early (before --nb samples have been reached) if (Y / epsilon) <= Yl and (Y * epsilon) >= Yh")

        ("cubes", "Use cubes")
        ("no-nnf", "Doesn't use the .unnf file, instead generates random assignments without considering any constraints");

    return desc;
}

template<typename RAG>
int setup_runner(po::variables_map const& vm, RAG & rag) {
    try {
        std::size_t const N = vm["nb"].as<std::size_t>();
        double const alpha = vm["alpha"].as<double>();
        std::size_t const lN = vm["lnb"].as<std::size_t>();
        mpf_float const epsilon = vm["epsilon"].as<double>();

        bool const verbose = vm.count("verbose") > 0;

        std::random_device dev;
        xoshiro512plusplus prng(dev);

        //CNF const& cnf = rag.get_cnf();

        std::size_t nb_tries = 0;
        std::size_t nb_success = 0;
        mpf_float rmean = 0;
        mpf_float rm = 0;

        using boost::math::normal_distribution;
        mpf_float const z = quantile(normal_distribution<mpf_float>(), 1 - mpf_float(alpha) / 2);

        std::cout << "N,Y,Yl,Yh\n";
        std::atomic<bool> done = false;

        #pragma omp parallel
        {
            xoshiro512plusplus lprng;
            #pragma omp critical(init)
            {
                lprng = prng;
                prng.jump();
            }

            while(! done.load()) {
                sampler::Cube cube = rag(lprng);

                #pragma omp critical
                {
                    nb_tries++;
                    if(cube.nb_unsat == 0) {
                        nb_success++;
                    }

                    if(nb_tries > 1) {
                        mpf_float sigma = 0.5;
                        sigma *= 0.5;
                        sigma /= nb_tries;
                        sigma = boost::multiprecision::sqrt(sigma);

                        mpf_float estimate = rag.get_mc();
                        estimate *= nb_success;
                        estimate /= nb_tries;

                        auto yl = estimate - z * sigma * rag.get_mc();
                        auto yh = estimate + z * sigma * rag.get_mc();

                        if(verbose || nb_tries == N) {
                            std::cout << nb_tries << ", " << estimate << ", " << yl << ", " << yh << "\n";
                        }

                        if(nb_success > 0 && lN > 0 && nb_tries >= lN && estimate / epsilon <= yl && estimate * epsilon >= yh) {
                            done.store(true);

                            if(!verbose) {
                                std::cout << nb_tries << ", " << estimate << ", " << yl << ", " << yh << "\n";
                            }
                        }

                        if(nb_tries >= N) {
                            done.store(true);
                        }
                    }
                }
            }
        }
    }
    catch(std::exception & e) {
        std::cerr << "EXCEPTION ERROR: " << e.what() << "\n";
        return 1;
    }
    return 0;
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
        bool const CUBES = vm.count("cubes") != 0;
        bool const NO_NNF = vm.count("no-nnf") != 0;

        std::cout << "c Initializing\n";

        CNF cnf(cnf_path + ".smp");
        //CNF cnf_ign(cnf_path + ".ign");
        NNF const unnf(cnf_path + ".unnf");


        if(CUBES) {
            auto cubes = dnf::read_cubes_from_file(cnf_path + ".cubes");
            sampler::RandomCubeAssignmentGenerator rag(cnf, unnf, cubes);
            std::cout << "c Using cubes\n";

            return setup_runner(vm, rag);
        }
        else if(NO_NNF) {
            sampler::RandomAssignmentGenerator rag(cnf);
            std::cout << "c Not using a d-DNNF\n";
            return setup_runner(vm, rag);
        }
        else {
            ANNF aunnf(unnf);
            aunnf.annotate_mc();
            sampler::RandomNNFAssignmentGenerator rag(cnf, aunnf);
            std::cout << "c Using unnf\n";
            return setup_runner(vm, rag);
        }
    }
    catch(std::exception & e) {
        std::cerr << "EXCEPTION ERROR: " << e.what() << "\n";
        return 1;
    }
    return 0;
}
