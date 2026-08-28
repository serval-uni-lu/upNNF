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

/**
  * \returns The amount of virtual memory used by the given process in bytes
  */
std::size_t get_mem_usage(pid_t p);

/**
  * \returns The path to the D4 executable that was set at compile time
  */
char const* get_d4_path();

/**
  * \brief Tries running D4 on the given CNF formula with the given time and memory limits.
  * \param cnf_path Path to the CNF file
  * \param time_seconds Maximum amount of time in seconds
  * \param memory_mb Maximum amount of virtual memory in MB
  * \param compile If true runs D4 with -dDNNF option (d-DNNF will be written to RunRes.output path if successful), otherwise uses -mc option
  */
RunRes run(std::string cnf_path, long int const time_seconds, std::size_t const memory_mb, bool const compile = false);

/**
  * \brief Tries running D4 on the given CNF formulae in parallel with the given time and memory limits for each individual call (memory is by process, not global so memory_mb = 2 means each D4 process gets 2 megabytes of memory)
  * \details
  * Here D4 is run with -mc, so no d-DNNF is generated to avoid frequent and large disk writes.
  */
std::vector<RunRes> run_many(std::vector<std::string> const& cnf_path, long int const time_seconds, std::size_t const memory_mb);


/**
  * \returns The given string with each char converted to its lower case
  */
std::string to_lower_case(std::string in);

#endif
