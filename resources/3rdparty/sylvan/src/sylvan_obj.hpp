/*
 * Copyright 2011-2015 Formal Methods and Tools, University of Twente
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef SYLVAN_OBJ_H
#define SYLVAN_OBJ_H

#include <string>
#include <vector>

#include <lace.h>
#include <sylvan.h>

namespace sylvan {

class BddMap;

class Bdd {
    friend class Sylvan;
    friend class BddMap;
    friend class Mtbdd;

public:
    Bdd() { bdd = sylvan_false; sylvan_protect(&bdd); }
    Bdd(const BDD from) : bdd(from) { sylvan_protect(&bdd); }
    Bdd(const Bdd &from) : bdd(from.bdd) { sylvan_protect(&bdd); }
    Bdd(const uint32_t var) { bdd = sylvan_ithvar(var); sylvan_protect(&bdd); }
    ~Bdd() { sylvan_unprotect(&bdd); }

    /**
     * @brief Creates a Bdd representing just the variable index in its positive form
     * The variable index must be a 0<=index<=2^23 (we use 24 bits internally)
     */
    static Bdd bddVar(uint32_t index);

    /**
     * @brief Returns the Bdd representing "True"
     */
    static Bdd bddOne();

    /**
     * @brief Returns the Bdd representing "False"
     */
    static Bdd bddZero();

    /**
     * @brief Returns the Bdd representing a cube of variables, according to the given values.
     * @param variables the variables that will be in the cube in their positive or negative form
     * @param values a character array describing how the variables will appear in the result
     * The length of string must be equal to the number of variables in the cube.
     * For every ith char in string, if it is 0, the corresponding variable will appear in its negative form,
     * if it is 1, it will appear in its positive form, and if it is 2, it will appear as "any", thus it will
     * be skipped.
     */
    static Bdd bddCube(const Bdd &variables, unsigned char *values);

    /**
     * @brief Returns the Bdd representing a cube of variables, according to the given values.
     * @param variables the variables that will be in the cube in their positive or negative form
     * @param string a character array describing how the variables will appear in the result
     * The length of string must be equal to the number of variables in the cube.
     * For every ith char in string, if it is 0, the corresponding variable will appear in its negative form,
     * if it is 1, it will appear in its positive form, and if it is 2, it will appear as "any", thus it will
     * be skipped.
     */
    static Bdd bddCube(const Bdd &variables, std::vector<uint8_t> values);

    int operator==(const Bdd& other) const;
    int operator!=(const Bdd& other) const;
    Bdd operator=(const Bdd& right);
    int operator<=(const Bdd& other) const;
    int operator>=(const Bdd& other) const;
    int operator<(const Bdd& other) const;
    int operator>(const Bdd& other) const;
    Bdd operator!() const;
    Bdd operator~() const;
    Bdd operator*(const Bdd& other) const;
    Bdd operator*=(const Bdd& other);
    Bdd operator&(const Bdd& other) const;
    Bdd operator&=(const Bdd& other);
    Bdd operator+(const Bdd& other) const;
    Bdd operator+=(const Bdd& other);
    Bdd operator|(const Bdd& other) const;
    Bdd operator|=(const Bdd& other);
    Bdd operator^(const Bdd& other) const;
    Bdd operator^=(const Bdd& other);
    Bdd operator-(const Bdd& other) const;
    Bdd operator-=(const Bdd& other);

    /**
     * @brief Returns non-zero if this Bdd is bddOne() or bddZero()
     */
    int isConstant() const;

    /**
     * @brief Returns non-zero if this Bdd is bddOne() or bddZero()
     */
    int isTerminal() const;

    /**
     * @brief Returns non-zero if this Bdd is bddOne()
     */
    int isOne() const;

    /**
     * @brief Returns non-zero if this Bdd is bddZero()
     */
    int isZero() const;

    /**
     * @brief Returns the top variable index of this Bdd (the variable in the root node)
     */
    uint32_t TopVar() const;

    /**
     * @brief Follows the high edge ("then") of the root node of this Bdd
     */
    Bdd Then() const;

    /**
     * @brief Follows the low edge ("else") of the root node of this Bdd
     */
    Bdd Else() const;

    /**
     * @brief Computes \exists cube: f \and g
     */
    Bdd AndAbstract(const Bdd& g, const Bdd& cube) const;

    /**
     * @brief Computes \exists cube: f
     */
    Bdd ExistAbstract(const Bdd& cube) const;

    /**
     * @brief Computes \forall cube: f
     */
    Bdd UnivAbstract(const Bdd& cube) const;

    /**
     * @brief Computes if f then g else h
     */
    Bdd Ite(const Bdd& g, const Bdd& h) const;

    /**
     * @brief Computes f \and g
     */
    Bdd And(const Bdd& g) const;

    /**
     * @brief Computes f \or g
     */
    Bdd Or(const Bdd& g) const;

    /**
     * @brief Computes \not (f \and g)
     */
    Bdd Nand(const Bdd& g) const;

    /**
     * @brief Computes \not (f \or g)
     */
    Bdd Nor(const Bdd& g) const;

    /**
     * @brief Computes f \xor g
     */
    Bdd Xor(const Bdd& g) const;

    /**
     * @brief Computes \not (f \xor g), i.e. f \equiv g
     */
    Bdd Xnor(const Bdd& g) const;

    /**
     * @brief Returns whether all elements in f are also in g
     */
    int Leq(const Bdd& g) const;

    /**
     * @brief Computes the reverse application of a transition relation to this set.
     * @param relation the transition relation to apply
     * @param cube the variables that are in the transition relation
     * This function assumes that s,t are interleaved with s odd and t even.
     * Other variables in the relation are ignored (by existential quantification)
     * Set cube to "false" (illegal cube) to assume all encountered variables are in s,t
     *
     * Use this function to concatenate two relations   --> -->
     * or to take the 'previous' of a set               -->  S
     */
    Bdd RelPrev(const Bdd& relation, const Bdd& cube) const;

    /**
     * @brief Computes the application of a transition relation to this set.
     * @param relation the transition relation to apply
     * @param cube the variables that are in the transition relation
     * This function assumes that s,t are interleaved with s odd and t even.
     * Other variables in the relation are ignored (by existential quantification)
     * Set cube to "false" (illegal cube) to assume all encountered variables are in s,t
     *
     * Use this function to take the 'next' of a set     S  -->
     */
    Bdd RelNext(const Bdd& relation, const Bdd& cube) const;

    /**
     * @brief Computes the transitive closure by traversing the BDD recursively.
     * See Y. Matsunaga, P. C. McGeer, R. K. Brayton
     *     On Computing the Transitive Closre of a State Transition Relation
     *     30th ACM Design Automation Conference, 1993.
     */
    Bdd Closure() const;

    /**
     * @brief Computes the constrain f @ c
     */
    Bdd Constrain(const Bdd &c) const;

    /**
     * @brief Computes the BDD restrict according to Coudert and Madre's algorithm (ICCAD90).
     */
    Bdd Restrict(const Bdd &c) const;

    /**
     * @brief Functional composition. Whenever a variable v in the map m is found in the BDD,
     *        it is substituted by the associated function.
     * You can also use this function to implement variable reordering.
     */
    Bdd Compose(const BddMap &m) const;

    /**
     * @brief Substitute all variables in the array from by the corresponding variables in to.
     */
    Bdd Permute(const std::vector<Bdd>& from, const std::vector<Bdd>& to) const;

    /**
     * @brief Computes the support of a Bdd.
     */
    Bdd Support() const;

    /**
     * @brief Gets the BDD of this Bdd (for C functions)
     */
    BDD GetBDD() const;

    /**
     * @brief Writes .dot file of this Bdd. Not thread-safe!
     */
    void PrintDot(FILE *out) const;

    /**
     * @brief Gets a SHA2 hash that describes the structure of this Bdd.
     * @param string a character array of at least 65 characters (includes zero-termination)
     * This hash is 64 characters long and is independent of the memory locations of BDD nodes.
     */
    void GetShaHash(char *string) const;

    std::string GetShaHash() const;

    /**
     * @brief Computes the number of satisfying variable assignments, using variables in cube.
     */
    double SatCount(const Bdd &variables) const;

    /**
     * @brief Gets one satisfying assignment according to the variables.
     * @param variables The set of variables to be assigned, must include the support of the Bdd.
     */
    void PickOneCube(const Bdd &variables, uint8_t *string) const;

    /**
     * @brief Gets one satisfying assignment according to the variables.
     * @param variables The set of variables to be assigned, must include the support of the Bdd.
     * Returns an empty vector when either this Bdd equals bddZero() or the cube is empty.
     */
    std::vector<bool> PickOneCube(const Bdd &variables) const;

    /**
     * @brief Gets a cube that satisfies this Bdd.
     */
    Bdd PickOneCube() const;

    /**
     * @brief Faster version of: *this + Sylvan::bddCube(variables, values);
     */
    Bdd UnionCube(const Bdd &variables, uint8_t *values) const;

    /**
     * @brief Faster version of: *this + Sylvan::bddCube(variables, values);
     */
    Bdd UnionCube(const Bdd &variables, std::vector<uint8_t> values) const;

    /**
     * @brief Generate a cube representing a set of variables
     */
    static Bdd VectorCube(const std::vector<Bdd> variables);

    /**
     * @brief Generate a cube representing a set of variables
     * @param variables An sorted set of variable indices
     */
    static Bdd VariablesCube(const std::vector<uint32_t> variables);

    /**
     * @brief Gets the number of nodes in this Bdd. Not thread-safe!
     */
    size_t NodeCount() const;

private:
    BDD bdd;
};

class BddMap
{
    friend class Bdd;
    BDD bdd;
    BddMap(const BDD from) : bdd(from) { sylvan_protect(&bdd); }
    BddMap(const Bdd &from) : bdd(from.bdd) { sylvan_protect(&bdd); }
public:
    BddMap() : bdd(sylvan_map_empty()) { sylvan_protect(&bdd); }
    ~BddMap() { sylvan_unprotect(&bdd); }

    BddMap(uint32_t key_variable, const Bdd value);

    BddMap operator+(const Bdd& other) const;
    BddMap operator+=(const Bdd& other);
    BddMap operator-(const Bdd& other) const;
    BddMap operator-=(const Bdd& other);

    /**
     * @brief Adds a key-value pair to the map
     */
    void put(uint32_t key, Bdd value);

    /**
     * @brief Removes a key-value pair from the map
     */
    void removeKey(uint32_t key);

    /**
     * @brief Returns the number of key-value pairs in this map
     */
    size_t size() const;

    /**
     * @brief Returns non-zero when this map is empty
     */
    int isEmpty() const;
};

class MtbddMap;

class Mtbdd {
    friend class Sylvan;
    friend class MtbddMap;

public:
    Mtbdd() { mtbdd = sylvan_false; mtbdd_protect(&mtbdd); }
    Mtbdd(const MTBDD from) : mtbdd(from) { mtbdd_protect(&mtbdd); }
    Mtbdd(const Mtbdd &from) : mtbdd(from.mtbdd) { mtbdd_protect(&mtbdd); }
    Mtbdd(const Bdd &from) : mtbdd(from.bdd) { mtbdd_protect(&mtbdd); }
    ~Mtbdd() { mtbdd_unprotect(&mtbdd); }

    /**
     * @brief Creates a Mtbdd leaf representing the uint64 value <value>
     */
    static Mtbdd uint64Terminal(uint64_t value);

    /**
     * @brief Creates a Mtbdd leaf representing the floating-point value <value>
     */
    static Mtbdd doubleTerminal(double value);

    /**
     * @brief Creates a Mtbdd leaf representing the fraction value <nominator>/<denominator>
     * Internally, Sylvan uses 32-bit values and reports overflows to stderr.
     */
    static Mtbdd fractionTerminal(uint64_t nominator, uint64_t denominator);

    /**
     * @brief Creates a Mtbdd leaf of type <type> holding value <value>
     * This is useful for custom Mtbdd types.
     */
    static Mtbdd terminal(uint32_t type, uint64_t value);

    /**
     * @brief Creates a Boolean Mtbdd representing jsut the variable index in its positive form
     * The variable index must be 0<=index<=2^23 (Sylvan uses 24 bits internally)
     */
    static Mtbdd mtbddVar(uint32_t variable);

    /**
     * @brief Returns the Boolean Mtbdd representing "True"
     */
    static Mtbdd mtbddOne();

    /**
     * @brief Returns the Boolean Mtbdd representing "False"
     */
    static Mtbdd mtbddZero();

    /**
     * @brief Returns the Mtbdd representing a cube of variables, according to the given values.
     * @param variables the variables that will be in the cube in their positive or negative form
     * @param values a character array describing how the variables will appear in the result
     * @param terminal the leaf of the cube
     * The length of string must be equal to the number of variables in the cube.
     * For every ith char in string, if it is 0, the corresponding variable will appear in its negative form,
     * if it is 1, it will appear in its positive form, and if it is 2, it will appear as "any", thus it will
     * be skipped.
     */
    static Mtbdd mtbddCube(const Mtbdd &variables, unsigned char *values, const Mtbdd &terminal);

     /**
     * @brief Returns the Mtbdd representing a cube of variables, according to the given values.
     * @param variables the variables that will be in the cube in their positive or negative form
     * @param values a character array describing how the variables will appear in the result
     * @param terminal the leaf of the cube
     * The length of string must be equal to the number of variables in the cube.
     * For every ith char in string, if it is 0, the corresponding variable will appear in its negative form,
     * if it is 1, it will appear in its positive form, and if it is 2, it will appear as "any", thus it will
     * be skipped.
     */
    static Mtbdd mtbddCube(const Mtbdd &variables, std::vector<uint8_t> values, const Mtbdd &terminal);

    int operator==(const Mtbdd& other) const;
    int operator!=(const Mtbdd& other) const;
    Mtbdd operator=(const Mtbdd& right);
    Mtbdd operator!() const;
    Mtbdd operator~() const;
    Mtbdd operator*(const Mtbdd& other) const;
    Mtbdd operator*=(const Mtbdd& other);
    Mtbdd operator+(const Mtbdd& other) const;
    Mtbdd operator+=(const Mtbdd& other);
    Mtbdd operator-(const Mtbdd& other) const;
    Mtbdd operator-=(const Mtbdd& other);

    // not implemented (compared to Bdd): <=, >=, <, >, &, &=, |, |=, ^, ^=

    /**
     * @brief Returns non-zero if this Mtbdd is a leaf
     */
    int isTerminal() const;

    /**
     * @brief Returns non-zero if this Mtbdd is a leaf
     */
    int isLeaf() const;

    /**
     * @brief Returns non-zero if this Mtbdd is mtbddOne()
     */
    int isOne() const;

    /**
     * @brief Returns non-zero if this Mtbdd is mtbddZero()
     */
    int isZero() const;

    /**
     * @brief Returns the top variable index of this Mtbdd (the variable in the root node)
     */
    uint32_t TopVar() const;

    /**
     * @brief Follows the high edge ("then") of the root node of this Mtbdd
     */
    Mtbdd Then() const;

    /**
     * @brief Follows the low edge ("else") of the root node of this Mtbdd
     */
    Mtbdd Else() const;

    /**
     * @brief Returns the negation of the MTBDD
     * For Boolean, this means "not", for floating-point and fractions, this means "negative"
     */
    Mtbdd Negate() const;

    /**
     * @brief Applies the binary operation <op>
     */
    Mtbdd Apply(const Mtbdd &other, mtbdd_apply_op op) const;

    /**
     * @brief Applies the unary operation <op> with parameter <param>
     */
    Mtbdd UApply(mtbdd_uapply_op op, size_t param) const;

    /**
     * @brief Computers the abstraction on variables <variables> using operator <op>.
     * See also: AbstractPlus, AbstractTimes, AbstractMin, AbstractMax
     */
    Mtbdd Abstract(const Mtbdd &variables, mtbdd_abstract_op op) const;

    /**
     * @brief Computes if f then g else h
     * This Mtbdd must be a Boolean Mtbdd
     */
    Mtbdd Ite(const Mtbdd &g, const Mtbdd &h) const;

    /**
     * @brief Computes f + g
     */
    Mtbdd Plus(const Mtbdd &other) const;

    /**
     * @brief Computes f * g
     */
    Mtbdd Times(const Mtbdd &other) const;

    /**
     * @brief Computes min(f, g)
     */
    Mtbdd Min(const Mtbdd &other) const;

    /**
     * @brief Computes max(f, g)
     */
    Mtbdd Max(const Mtbdd &other) const;

    /**
     * @brief Computes abstraction by summation (existential quantification)
     */
    Mtbdd AbstractPlus(const Mtbdd &variables) const;

    /**
     * @brief Computes abstraction by multiplication (universal quantification)
     */
    Mtbdd AbstractTimes(const Mtbdd &variables) const;

    /**
     * @brief Computes abstraction by minimum
     */
    Mtbdd AbstractMin(const Mtbdd &variables) const;

    /**
     * @brief Computes abstraction by maximum
     */
    Mtbdd AbstractMax(const Mtbdd &variables) const;

    /**
     * @brief Computes abstraction by summation of f \times g
     */
    Mtbdd AndExists(const Mtbdd &other, const Mtbdd &variables) const;

    /**
     * @brief Convert floating-point/fraction Mtbdd to a Boolean Mtbdd, leaf >= value ? true : false
     */
    Mtbdd MtbddThreshold(double value) const;

    /**
     * @brief Convert floating-point/fraction Mtbdd to a Boolean Mtbdd, leaf > value ? true : false
     */
    Mtbdd MtbddStrictThreshold(double value) const;

    /**
     * @brief Convert floating-point/fraction Mtbdd to a Boolean Mtbdd, leaf >= value ? true : false
     * Same as MtbddThreshold (Bdd = Boolean Mtbdd)
     */
    Bdd BddThreshold(double value) const;

    /**
     * @brief Convert floating-point/fraction Mtbdd to a Boolean Mtbdd, leaf > value ? true : false
     * Same as MtbddStrictThreshold (Bdd = Boolean Mtbdd)
     */
    Bdd BddStrictThreshold(double value) const;

    /**
     * @brief Computes the support of a Mtbdd.
     */
    Mtbdd Support() const;

    /**
     * @brief Gets the MTBDD of this Mtbdd (for C functions)
     */
    MTBDD GetMTBDD() const;

    /**
     * @brief Functional composition. Whenever a variable v in the map m is found in the MTBDD,
     *        it is substituted by the associated function (which should be a Boolean MTBDD)
     * You can also use this function to implement variable reordering.
     */
    Mtbdd Compose(MtbddMap &m) const;

    /**
     * @brief Compute the number of satisfying variable assignments, using variables in cube.
     */
    double SatCount(const Mtbdd &variables) const;

    /**
     * @brief Gets the number of nodes in this Bdd. Not thread-safe!
     */
    size_t NodeCount() const;

private:
    MTBDD mtbdd;
};

class MtbddMap
{
    friend class Mtbdd;
    MTBDD mtbdd;
    MtbddMap(MTBDD from) : mtbdd(from) { mtbdd_protect(&mtbdd); }
    MtbddMap(Mtbdd &from) : mtbdd(from.mtbdd) { mtbdd_protect(&mtbdd); }
public:
    MtbddMap() : mtbdd(mtbdd_map_empty()) { mtbdd_protect(&mtbdd); }
    ~MtbddMap() { mtbdd_unprotect(&mtbdd); }

    MtbddMap(uint32_t key_variable, Mtbdd value);

    MtbddMap operator+(const Mtbdd& other) const;
    MtbddMap operator+=(const Mtbdd& other);
    MtbddMap operator-(const Mtbdd& other) const;
    MtbddMap operator-=(const Mtbdd& other);

    /**
     * @brief Adds a key-value pair to the map
     */
    void put(uint32_t key, Mtbdd value);

    /**
     * @brief Removes a key-value pair from the map
     */
    void removeKey(uint32_t key);

    /**
     * @brief Returns the number of key-value pairs in this map
     */
    size_t size();

    /**
     * @brief Returns non-zero when this map is empty
     */
    int isEmpty();
};

class Sylvan {
public:
    /**
     * @brief Initializes the Sylvan framework, call this only once in your program.
     * @param initialTableSize The initial size of the nodes table. Must be a power of two.
     * @param maxTableSize The maximum size of the nodes table. Must be a power of two.
     * @param initialCacheSize The initial size of the operation cache. Must be a power of two.
     * @param maxCacheSize The maximum size of the operation cache. Must be a power of two.
     */
    static void initPackage(size_t initialTableSize, size_t maxTableSize, size_t initialCacheSize, size_t maxCacheSize);

    /**
     * @brief Initializes the BDD module of the Sylvan framework.
     * @param granularity determins operation cache behavior; for higher values (2+) it will use the operation cache less often.
     * Values of 3-7 may result in better performance, since occasionally not using the operation cache is fine in practice.
     * A granularity of 1 means that every BDD operation will be cached at every variable level.
     */
    static void initBdd(int granularity);

    /**
     * @brief Initializes the MTBDD module of the Sylvan framework.
     */
    static void initMtbdd();

    /**
     * @brief Frees all memory in use by Sylvan.
     * Warning: if you have any Bdd objects which are not bddZero() or bddOne() after this, your program may crash!
     */
    static void quitPackage();
};

}

#endif
