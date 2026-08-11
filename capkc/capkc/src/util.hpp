#ifndef UTIL_HPP
#define UTIL_HPP

#include <string>
#include <vector>
#include <chrono>
#include <ostream>

enum class RunStatus {
    Void,
    Running,
    Done,
    Error,
    Impossible,
    Memory,
    Time
};

std::ostream& operator<<(std::ostream & out, RunStatus const& r);

struct RunRes {
    std::string input = "";
    std::string nnf_path = "";

    std::string output = "";
    RunStatus status = RunStatus::Void;
    std::size_t mem = 0; // max mem in kB
    std::chrono::milliseconds time;
};

std::size_t get_mem_usage(pid_t p);

char const* get_d4_path();
RunRes run(std::string cnf_path, long int const time_seconds, std::size_t const memory_mb, bool const compile = false);

std::vector<RunRes> run_many(std::vector<std::string> const& cnf_path, long int const time_seconds, std::size_t const memory_mb);

std::string to_lower_case(std::string in);

#endif
