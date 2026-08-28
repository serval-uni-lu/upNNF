//
// Created by oz on 1/12/22.
//
//
//

#ifndef BN_CNF_HPP
#define BN_CNF_HPP

#include<unordered_set>
#include<vector>
#include<map>
#include<ostream>

#include "var.hpp"

std::string mtrim(std::string const& s);

/**
 * \brief Represents a clause as a list of literals
 */
struct Clause {
    private:
    //std::set<Literal> c;
    std::vector<Literal> c;
    mutable LitSet ls_c;

    public:
    /**
     * \brief Construct an empty clause
     */
    Clause();
    Clause(Clause const& c) = default;
    Clause(Clause && c) = default;

    Clause& operator=(Clause const& c) = default;
    Clause& operator=(Clause && c) = default;

    /**
     * \brief Add a literal to the clause if it is not already present
     */
    void push(Literal const& l);

    /**
     * \brief Remove the literal from the clause
     */
    void remove(Literal const& l);

    /**
     * \brief Remove every occurence of the varliable v from the clause
     * \details
     * removes the literals v and ~v from the clause;
     */
    void remove(Variable const& v);

    /**
     * \returns True if the clause contains l
     */
    bool contains(Literal const& l) const;

    /**
     * \returns True if the clause contains very literal in the clause cls
     */
    bool contains(Clause const& cls) const;

    /**
     * \returns An iterator to the beginning of the clause
     */
    inline auto begin() const {
        return c.begin();
    }

    /**
     * \returns An iterator to the end of the clause (past the end iterator)
     */
    inline auto end() const {
        return c.end();
    }

    /**
     * \returns The number of literals in the clause
     */
    inline std::size_t size() const {
        return c.size();
    }

    /**
     * \returns A constant reference to the literal at index i
     */
    inline auto const& operator[](std::size_t i) const {
        return c[i];
    }

    friend bool operator==(Clause const& a, Clause const& b);
};

/**
 * \brief Prints the clause in DIMACS format to the stream ('0' terminated)
 * \relates Clause
 */
inline std::ostream & operator<<(std::ostream & out, Clause const& c) {
    for(auto const& l : c) {
        out << l << " ";
    }
    out << "0";
    return out;
}

/**
 * \brief Represents a propositional formula in CNF form
 */
class CNF {
    private:
        std::vector<Clause> clauses;
        std::vector<bool> active;
        std::vector<std::unordered_set<std::size_t> > idx;

        std::unordered_set<Literal> units;
        std::unordered_set<Variable> free;

        std::unordered_set<Variable> vars;

        std::vector<std::size_t> available_ids;

        std::size_t nb_active = 0;

        void compute_idx();

    public:
        CNF() = default;
        CNF(std::size_t nbv);
        /**
         * \brief Reads a formula from a DIMACS file located at path
         * \param Path path to the file containing the formula in DIMACS format
         */
        CNF(std::string const& path);
        CNF(CNF const& c) = default;
        CNF(CNF && c) = default;

        CNF& operator=(CNF const& c) = default;
        CNF& operator=(CNF && c) = default;

        /**
         * \brief Computes the set of unconstrained variables (i.e., that do no appear in the formula syntactically)
         * \details The result is stored in a class member
         */
        void compute_free_vars();

        /**
         * \brief Applies boolean constraint propagation to the formula
         */
        void simplify();
        /**
         * \brief Removes clauses based on subsumption
         * \details
         * If we have two clauses A and B, and we have A.contains(B), then we know that A is a logical implication of B.
         *
         * Therefore, A is redundant as it will always be true if B is true and we can safely remove A from the formula.
         */
        void subsumption();

        /**
         * \brief Add the clause to the formula
         */
        void add_clause(Clause c);

        /**
         * \brief Add the clause only if there is no clause A such that c.contains(A)
         */
        void add_clause_nonredundant(Clause c);

        /**
         * \brief Removes the clause with the given id
         * \pre 0 <= id < nb_clauses()
         */
        void rm_clause(std::size_t id);

        /**
          * \returns The occurence count of variable v
          * \details idx[positive].size() + idx[negative].size()
          */
        std::size_t occurrence_count(Variable v);

        /**
          * \returns The occurence product of variable v
          * \details idx[positive].size() * idx[negative].size()
          */
        std::size_t occurrence_product(Variable v);

        /**
         * \returns The number of variables 
         */
        inline int nb_vars() const { return vars.size(); }

        /**
         * \returns The number of variables that do not appear syntactically in the formula
         * if compute_free_vars() has been called before
         */
        inline int nb_free_vars() const { return free.size(); }

        /**
         * \returns The number of unit clauses (containing only one literal)
         * if simplify() has been called before
         */
        inline int nb_units() const { return units.size(); }

        /**
         * \returns The number of variables that appear syntactically in the formula
         * if compute_free_vars() has been called before
         */
        inline int nb_c_vars() const { return nb_vars() - nb_free_vars(); }

        /**
         * \returns The total number of clauses
         */
        inline std::size_t nb_clauses() const { return clauses.size(); }

        /**
         * \returns The number of clauses that are active
         * \brief When adding or removing clauses, they aren't removed from the array, therefore, some ids are present but unused (i.e. not active).
         */
        inline std::size_t nb_active_clauses() const { return nb_active; }

        /**
          * \returns The units present in the formula
          * if compute_free_vars() has been called before
          */
        inline std::unordered_set<Literal> const& get_units() const {
            return units;
        }

        /**
          * \returns True, if l or ~l is a unit literal, l is modified to be the unit literal
          * if compute_free_vars() has been called before
          */
        inline bool is_unit(Literal & l) const {
            if(units.find(l) != units.end()) {
                return true;
            }
            if(units.find(~l) != units.end()) {
                l = ~l;
                return true;
            }
            return false;
        }

        /**
          * \brief Activate or deactivate a clause ID by setting v to true or false respectively
          * \details Inactive clauses are ignored, can be overridden when calling add_clause and will not
          * Appear when writing the formula out to a stream or file.
          */
        inline void set_active(std::size_t const id, bool v) {
            if(active[id] && !v) {
                nb_active--;
            }
            else if(!active[id] && v) {
                nb_active++;
            }
            active[id] = v;
        }

        /**
          * \returns The list of active clauses that contain the literal l
          */
        inline std::unordered_set<std::size_t> const& get_idx(Literal const l) const { return idx[l.get()]; }

        /**
          * \returns True if the clause is active
          */
        inline bool is_active(std::size_t const id) const { return active[id]; }

        /**
          * \returns A constant reference to the requested clause
          */
        inline Clause const& clause(std::size_t const id) const { return clauses[id]; }

        /**
          * \brief Writes the active clauses in DIMACS format to the stream
          */
    friend std::ostream & operator<<(std::ostream & out, CNF const& cnf);
};

std::ostream & operator<<(std::ostream & out, CNF const& cnf);

/**
  * \returns True if the clause c is satisfied by the given cube, false otherwise
  */
bool clause_is_sat(Clause const& c, LitSet const& cube);

/**
  * \returns The number of clauses that don't have at least one literal in common with the cube
  */
std::size_t nb_unsat(CNF const& cnf, LitSet const& cube);

/**
  * \returns The number of clauses that don't have at least one literal in common with the cube if we set the literals in lits vector to the given values
  * \params n_unsat is returned by nb_unsat(cnf, cube)
  */
std::size_t nb_unsat_flip(CNF const& cnf, LitSet & cube, std::size_t const n_unsat, std::vector<Literal> const& lits);

/**
  * \returns The number of clauses that don't have at least one literal in common with the cube if we flip the value of literal lp in m
  * \params n_unsat is returned by nb_unsat(cnf, m)
  * \brief This function is intended as a faster way to try multiple flips instead of calling nb_unsat each time
  */
std::size_t nb_flip_unsatisfied(CNF const& cnf, LitSet & m, std::size_t const n_unsat, Literal & lp);

/**
  * \returns The clause ids of the clauses that don't share at least one literal with the given cube
  */
BitSet<std::size_t> unsat_clause_ids(CNF const& cnf, LitSet const& cube);

/**
  * \returns The clause ids of the clauses that don't share at least one literal with the given cube if we flip the value of literal lp in cube
  * \params unsatids is returned by unsat_clause_ids(cnf, m)
  * \brief This function is intended as a faster way to try multiple flips instead of calling unsat_clause_ids each time
  */
BitSet<std::size_t> unsat_clause_ids_flip(CNF const& cnf, LitSet & m, BitSet<std::size_t> const& unsat_ids, Literal & lp);

#endif //BN_CNF_HPP
