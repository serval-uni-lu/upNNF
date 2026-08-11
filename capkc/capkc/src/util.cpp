#include <fstream>
#include <sstream>
#include <thread>
#include <iostream>
#include <algorithm>
#include <cctype>

#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>

#include "util.hpp"

#ifdef CONTAINER
#define D4_PATH "/d4/d4"
#else
#define D4_PATH "/home/oz/.local/bin/d4"
#endif

char const* get_d4_path() {
    return D4_PATH;
}

std::ostream& operator<<(std::ostream & out, RunStatus const& r) {
    switch(r) {
        case RunStatus::Void:
            out << "RunStatus::Void";
            break;
        case RunStatus::Running:
            out << "RunStatus::Running";
            break;
        case RunStatus::Done:
            out << "RunStatus::Done";
            break;
        case RunStatus::Error:
            out << "RunStatus::Error";
            break;
        case RunStatus::Impossible:
            out << "RunStatus::Impossible";
            break;
        case RunStatus::Memory:
            out << "RunStatus::Memory";
            break;
        case RunStatus::Time:
            out << "RunStatus::Time";
            break;
    }
    return out;
}

// return mem in bytes
std::size_t get_mem_usage(pid_t p) {
    std::string path = "/proc/" + std::to_string(p) + "/status";
    std::ifstream in(path);

    if(in) {
        std::string line;
        std::string key;
        std::size_t value;
        std::string unit;

        // while(in >> key >> value >> unit) {
        //     std::cout << key << value << unit << "\n";
        //     if(key == "VmSize:") {
        //         return value;
        //     }
        // }
        while(std::getline(in, line)) {
            // VmSize: total virtual memory
            // VmRSS : resident set size (in RAM right now)
            if(line.starts_with("VmRSS:")) {
                std::stringstream stream(line);
                stream >> key >> value >> unit;

                if(unit == "kB") {
                    value *= 1024;
                }
                else {
                    std::cerr << "get_mem_usage: Unexpected unit: " << unit << "\n";
                }

                return value;
            }
        }
    }
    else {
        return 42;
    }
    return 32;
}

std::string extract_mc(std::string const& str) {
    std::istringstream f(str);
    std::string line;
    while(std::getline(f, line)) {
        if(line.starts_with("s ")) {
            return line.substr(2);
        }
    }
    return "";
}

struct ForkRes {
    pid_t pid;
    RunStatus status = RunStatus::Void;
    bool running = true;
    bool killed = false;
    int pipe_fd[2];
    std::string nnf_path;
};

ForkRes fork_one(std::string cnf_path, bool const compile = false) {
    ForkRes res;
    res.running = true;
    res.killed = false;
    res.status = RunStatus::Running;
    res.pid = 0;
    res.nnf_path = "";

    if(pipe(res.pipe_fd) == -1) {
        std::cerr << "ERROR in run(char*): pipe creation\n";
        res.status = RunStatus::Error;
        return res;
    }

    res.pid = fork();
    if(res.pid == -1) {
        std::cerr << "ERROR in run(char*): failed to fork\n";
        res.status = RunStatus::Error;
        res.running = false;
        return res;
    }

    std::string nnf_buffer = "-out=" + cnf_path + ".nnf";
    res.nnf_path = cnf_path + ".nnf";

    if(res.pid == 0) {

        close(res.pipe_fd[0]);
        dup2(res.pipe_fd[1], STDOUT_FILENO); // Redirect stdout to pipe
        //dup2(res.pipe_fd[1], STDERR_FILENO); // Redirect stdout to pipe
        close(res.pipe_fd[1]);

        int vp_res;
        if(compile) {
            char * const argv[] = {(char*)D4_PATH
                , (char*)"-dDNNF"
                , const_cast<char*>(cnf_path.c_str())
                , const_cast<char*>(nnf_buffer.c_str())
                , nullptr
                };

            vp_res = execvp(argv[0], argv);
        }
        else {
            char * const argv[] = {(char*)D4_PATH
                , (char*)"-mc"
                , const_cast<char*>(cnf_path.c_str())
                , nullptr
                };

            vp_res = execvp(argv[0], argv);
        }

        std::cerr << "ERROR in run(char*): execvp failed: " << vp_res << "\n";
        res.status = RunStatus::Error;
        exit(1);
        //return res;
    }
    else {
        close(res.pipe_fd[1]); // Close write end
        fcntl(res.pipe_fd[0], F_SETFL, O_NONBLOCK);
    }

    return res;
}

void read_into(int fd, std::string & out) {
    char buffer[4096];
    ssize_t bytes_read;
    while ((bytes_read = read(fd, buffer, sizeof(buffer) - 1)) > 0) {
        buffer[bytes_read] = '\0';
        out += buffer;
    }
}

RunRes run(std::string cnf_path, long int const time_seconds, std::size_t const memory_mb, bool const compile) {
    RunRes res;
    res.input = cnf_path;
    res.status = RunStatus::Void;
    res.mem = 0;

    ForkRes child = fork_one(cnf_path, compile);

    res.nnf_path = child.nnf_path;

    if(child.pid == 0) {
        std::cerr << "ERROR in run(char*): execvp failed\n";
        res.status = RunStatus::Error;
        return res;
    }
    else {
        int status;
        bool killed = false;
        auto start_time = std::chrono::steady_clock::now();

        std::string output;

        while(true) {
            auto now = std::chrono::steady_clock::now();
            auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(now - start_time).count();
            res.time = std::chrono::duration_cast<std::chrono::milliseconds>(now - start_time);

            pid_t result = waitpid(child.pid, &status, WNOHANG);
            if (result == -1) {
                std::cerr << "ERROR in run(char*): waitpid failed\n";
                break;
            } else if (result > 0) {
                break;
            }

            read_into(child.pipe_fd[0], output);

            std::size_t memory = get_mem_usage(child.pid);
            res.mem = std::max(res.mem, memory >> 10);
            
            if (elapsed > time_seconds) {
                kill(child.pid, SIGKILL);
                while(0 == waitpid(child.pid, &status, 0));
                killed = true;
                res.status = RunStatus::Time;
                break;
            }

            if (memory > (memory_mb << 20)) {
                kill(child.pid, SIGKILL);
                while(0 == waitpid(child.pid, &status, 0));
                killed = true;
                res.status = RunStatus::Memory;
                break;
            }

            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }

        read_into(child.pipe_fd[0], output);
        close(child.pipe_fd[0]);

        if(!killed) {
            res.status = RunStatus::Done;
            res.output = extract_mc(output);
            return res;
        }
        else {
            // res.status = -2;
            return res;
        }
    }

    res.status = RunStatus::Impossible;
    return res;
}

std::vector<RunRes> run_many(std::vector<std::string> const& cnf_path, long int const time_seconds, std::size_t const memory_mb) {
    std::vector<RunRes> res;
    std::vector<ForkRes> children;

    for(std::string const& path : cnf_path) {
        ForkRes child = fork_one(path);
        RunRes tmp;
        tmp.input = path;
        tmp.nnf_path = child.nnf_path;

        res.push_back(tmp);
        children.push_back(child);
    }

    auto start_time = std::chrono::steady_clock::now();
    bool running = true;
    while(running) {
        running = false;
        for(std::size_t id = 0; id < children.size(); id++) {
            int status;

            ForkRes & child = children[id];
            if(child.running) {
                running = true;

                auto now = std::chrono::steady_clock::now();
                auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(now - start_time).count();
                res[id].time = std::chrono::duration_cast<std::chrono::milliseconds>(now - start_time);
                read_into(child.pipe_fd[0], res[id].output);

                pid_t result = waitpid(child.pid, &status, WNOHANG);
                if(result == -1) {
                    child.running = false;
                    std::cerr << "ERROR in run_many(...): waitpid failed\n";
                }
                else if(result > 0) {
                    //std::cout << "    join: " << result << " :: " << child.pid << "\n";
                    child.running = false;
                    if(WEXITSTATUS(status)) {
                        child.status = RunStatus::Done;
                    }
                    else {
                        child.status = RunStatus::Error;
                    }
                    continue;
                }
                else if(result == 0) {
                    //std::cout << "    failed join: " << result << " :: " << child.pid << "\n";
                }

                std::size_t memory = get_mem_usage(child.pid);
                res[id].mem = std::max(res[id].mem, memory >> 10);

                if (elapsed > time_seconds) {
                    kill(child.pid, SIGKILL);
                    while(0 == waitpid(child.pid, &status, 0));
                    res[id].status = RunStatus::Time;
                    child.running = false;
                    child.killed = true;
                    continue;
                }

                if (memory > (memory_mb << 20)) {
                    kill(child.pid, SIGKILL);
                    while(0 == waitpid(child.pid, &status, 0));
                    res[id].status = RunStatus::Memory;
                    child.running = false;
                    child.killed = true;
                    continue;
                }
            }
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    for(std::size_t id = 0; id < children.size(); id++) {
        ForkRes & child = children[id];

        read_into(child.pipe_fd[0], res[id].output);
        close(child.pipe_fd[0]);

        if(child.killed) {
        }
        else {
            res[id].status = RunStatus::Done;
            res[id].output = extract_mc(res[id].output);
        }
    }

    return res;
}

std::string to_lower_case(std::string in) {
    std::transform(in.begin(), in.end(), in.begin(),
                   [](unsigned char c){ return std::tolower(c); });
    return in;
}
