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

#include <sylvan_obj.hpp>

using namespace sylvan;

/***
 * Implementation of class Bdd
 */

int
Bdd::operator==(const Bdd& other) const
{
    return bdd == other.bdd;
}

int
Bdd::operator!=(const Bdd& other) const
{
    return bdd != other.bdd;
}

Bdd
Bdd::operator=(const Bdd& right)
{
    bdd = right.bdd;
    return *this;
}

int
Bdd::operator<=(const Bdd& other) const
{
    // TODO: better implementation, since we are not interested in the BDD result
    LACE_ME;
    BDD r = sylvan_ite(this->bdd, sylvan_not(other.bdd), sylvan_false);
    return r == sylvan_false;
}

int
Bdd::operator>=(const Bdd& other) const
{
    // TODO: better implementation, since we are not interested in the BDD result
    return other <= *this;
}

int
Bdd::operator<(const Bdd& other) const
{
    return bdd != other.bdd && *this <= other;
}

int
Bdd::operator>(const Bdd& other) const
{
    return bdd != other.bdd && *this >= other;
}

Bdd
Bdd::operator!() const
{
    return Bdd(sylvan_not(bdd));
}

Bdd
Bdd::operator~() const
{
    return Bdd(sylvan_not(bdd));
}

Bdd
Bdd::operator*(const Bdd& other) const
{
    LACE_ME;
    return Bdd(sylvan_and(bdd, other.bdd));
}

Bdd
Bdd::operator*=(const Bdd& other)
{
    LACE_ME;
    bdd = sylvan_and(bdd, other.bdd);
    return *this;
}

Bdd
Bdd::operator&(const Bdd& other) const
{
    LACE_ME;
    return Bdd(sylvan_and(bdd, other.bdd));
}

Bdd
Bdd::operator&=(const Bdd& other)
{
    LACE_ME;
    bdd = sylvan_and(bdd, other.bdd);
    return *this;
}

Bdd
Bdd::operator+(const Bdd& other) const
{
    LACE_ME;
    return Bdd(sylvan_or(bdd, other.bdd));
}

Bdd
Bdd::operator+=(const Bdd& other)
{
    LACE_ME;
    bdd = sylvan_or(bdd, other.bdd);
    return *this;
}

Bdd
Bdd::operator|(const Bdd& other) const
{
    LACE_ME;
    return Bdd(sylvan_or(bdd, other.bdd));
}

Bdd
Bdd::operator|=(const Bdd& other)
{
    LACE_ME;
    bdd = sylvan_or(bdd, other.bdd);
    return *this;
}

Bdd
Bdd::operator^(const Bdd& other) const
{
    LACE_ME;
    return Bdd(sylvan_xor(bdd, other.bdd));
}

Bdd
Bdd::operator^=(const Bdd& other)
{
    LACE_ME;
    bdd = sylvan_xor(bdd, other.bdd);
    return *this;
}

Bdd
Bdd::operator-(const Bdd& other) const
{
    LACE_ME;
    return Bdd(sylvan_and(bdd, sylvan_not(other.bdd)));
}

Bdd
Bdd::operator-=(const Bdd& other)
{
    LACE_ME;
    bdd = sylvan_and(bdd, sylvan_not(other.bdd));
    return *this;
}

Bdd
Bdd::AndAbstract(const Bdd &g, const Bdd &cube) const
{
    LACE_ME;
    return sylvan_and_exists(bdd, g.bdd, cube.bdd);
}

Bdd
Bdd::ExistAbstract(const Bdd &cube) const
{
    LACE_ME;
    return sylvan_exists(bdd, cube.bdd);
}

Bdd
Bdd::UnivAbstract(const Bdd &cube) const
{
    LACE_ME;
    return sylvan_forall(bdd, cube.bdd);
}

Bdd
Bdd::Ite(const Bdd &g, const Bdd &h) const
{
    LACE_ME;
    return sylvan_ite(bdd, g.bdd, h.bdd);
}

Bdd
Bdd::And(const Bdd &g) const
{
    LACE_ME;
    return sylvan_and(bdd, g.bdd);
}

Bdd
Bdd::Or(const Bdd &g) const
{
    LACE_ME;
    return sylvan_or(bdd, g.bdd);
}

Bdd
Bdd::Nand(const Bdd &g) const
{
    LACE_ME;
    return sylvan_nand(bdd, g.bdd);
}

Bdd
Bdd::Nor(const Bdd &g) const
{
    LACE_ME;
    return sylvan_nor(bdd, g.bdd);
}

Bdd
Bdd::Xor(const Bdd &g) const
{
    LACE_ME;
    return sylvan_xor(bdd, g.bdd);
}

Bdd
Bdd::Xnor(const Bdd &g) const
{
    LACE_ME;
    return sylvan_equiv(bdd, g.bdd);
}

int
Bdd::Leq(const Bdd &g) const
{
    // TODO: better implementation, since we are not interested in the BDD result
    LACE_ME;
    BDD r = sylvan_ite(bdd, sylvan_not(g.bdd), sylvan_false);
    return r == sylvan_false;
}

Bdd
Bdd::RelPrev(const Bdd& relation, const Bdd& cube) const
{
    LACE_ME;
    return sylvan_relprev(relation.bdd, bdd, cube.bdd);
}

Bdd
Bdd::RelNext(const Bdd &relation, const Bdd &cube) const
{
    LACE_ME;
    return sylvan_relnext(bdd, relation.bdd, cube.bdd);
}

Bdd
Bdd::Closure() const
{
    LACE_ME;
    return sylvan_closure(bdd);
}

Bdd
Bdd::Constrain(const Bdd &c) const
{
    LACE_ME;
    return sylvan_constrain(bdd, c.bdd);
}

Bdd
Bdd::Restrict(const Bdd &c) const
{
    LACE_ME;
    return sylvan_restrict(bdd, c.bdd);
}

Bdd
Bdd::Compose(const BddMap &m) const
{
    LACE_ME;
    return sylvan_compose(bdd, m.bdd);
}

Bdd
Bdd::Permute(const std::vector<Bdd>& from, const std::vector<Bdd>& to) const
{
    LACE_ME;

    /* Create a map */
    BddMap map;
    for (int i=from.size()-1; i>=0; i--) {
        map.put(from[i].TopVar(), to[i]);
    }

    return sylvan_compose(bdd, map.bdd);
}

Bdd
Bdd::Support() const
{
    LACE_ME;
    return sylvan_support(bdd);
}

BDD
Bdd::GetBDD() const
{
    return bdd;
}

void
Bdd::PrintDot(FILE *out) const
{
    sylvan_fprintdot(out, bdd);
}

void
Bdd::GetShaHash(char *string) const
{
    sylvan_getsha(bdd, string);
}

std::string
Bdd::GetShaHash() const
{
    char buf[65];
    sylvan_getsha(bdd, buf);
    return std::string(buf);
}

double
Bdd::SatCount(const Bdd &variables) const
{
    LACE_ME;
    return sylvan_satcount_cached(bdd, variables.bdd);
}

void
Bdd::PickOneCube(const Bdd &variables, uint8_t *values) const
{
    LACE_ME;
    sylvan_sat_one(bdd, variables.bdd, values);
}

std::vector<bool>
Bdd::PickOneCube(const Bdd &variables) const
{
    std::vector<bool> result = std::vector<bool>();

    BDD bdd = this->bdd;
    BDD vars = variables.bdd;

    if (bdd == sylvan_false) return result;

    for (; !sylvan_set_isempty(vars); vars = sylvan_set_next(vars)) {
        uint32_t var = sylvan_set_var(vars);
        if (bdd == sylvan_true) {
            // pick 0
            result.push_back(false);
        } else {
            if (sylvan_var(bdd) != var) {
                // pick 0
                result.push_back(false);
            } else {
                if (sylvan_low(bdd) == sylvan_false) {
                    // pick 1
                    result.push_back(true);
                    bdd = sylvan_high(bdd);
                } else {
                    // pick 0
                    result.push_back(false);
                    bdd = sylvan_low(bdd);
                }
            }
        }
    }

    return result;
}

Bdd
Bdd::PickOneCube() const
{
    LACE_ME;
    return Bdd(sylvan_sat_one_bdd(bdd));
}

Bdd
Bdd::UnionCube(const Bdd &variables, uint8_t *values) const
{
    LACE_ME;
    return sylvan_union_cube(bdd, variables.bdd, values);
}

Bdd
Bdd::UnionCube(const Bdd &variables, std::vector<uint8_t> values) const
{
    LACE_ME;
    uint8_t *data = values.data();
    return sylvan_union_cube(bdd, variables.bdd, data);
}

/**
 * @brief Generate a cube representing a set of variables
 */
Bdd
Bdd::VectorCube(const std::vector<Bdd> variables)
{
    Bdd result = Bdd::bddOne();
    for (int i=variables.size()-1; i>=0; i--) {
        result *= variables[i];
    }
    return result;
}

/**
 * @brief Generate a cube representing a set of variables
 */
Bdd
Bdd::VariablesCube(std::vector<uint32_t> variables)
{
    BDD result = sylvan_true;
    for (int i=variables.size()-1; i>=0; i--) {
        result = sylvan_makenode(variables[i], sylvan_false, result);
    }
    return result;
}

size_t
Bdd::NodeCount() const
{
    return sylvan_nodecount(bdd);
}

Bdd
Bdd::bddOne()
{
    return sylvan_true;
}

Bdd
Bdd::bddZero()
{
    return sylvan_false;
}

Bdd
Bdd::bddVar(uint32_t index)
{
    LACE_ME;
    return sylvan_ithvar(index);
}

Bdd
Bdd::bddCube(const Bdd &variables, uint8_t *values)
{
    LACE_ME;
    return sylvan_cube(variables.bdd, values);
}

Bdd
Bdd::bddCube(const Bdd &variables, std::vector<uint8_t> values)
{
    LACE_ME;
    uint8_t *data = values.data();
    return sylvan_cube(variables.bdd, data);
}

int
Bdd::isConstant() const
{
    return bdd == sylvan_true || bdd == sylvan_false;
}

int
Bdd::isTerminal() const
{
    return bdd == sylvan_true || bdd == sylvan_false;
}

int
Bdd::isOne() const
{
    return bdd == sylvan_true;
}

int
Bdd::isZero() const
{
    return bdd == sylvan_false;
}

uint32_t
Bdd::TopVar() const
{
    return sylvan_var(bdd);
}

Bdd
Bdd::Then() const
{
    return Bdd(sylvan_high(bdd));
}

Bdd
Bdd::Else() const
{
    return Bdd(sylvan_low(bdd));
}

/***
 * Implementation of class BddMap
 */

BddMap::BddMap(uint32_t key_variable, const Bdd value)
{
    bdd = sylvan_map_add(sylvan_map_empty(), key_variable, value.bdd);
}


BddMap
BddMap::operator+(const Bdd& other) const
{
    return BddMap(sylvan_map_addall(bdd, other.bdd));
}

BddMap
BddMap::operator+=(const Bdd& other)
{
    bdd = sylvan_map_addall(bdd, other.bdd);
    return *this;
}

BddMap
BddMap::operator-(const Bdd& other) const
{
    return BddMap(sylvan_map_removeall(bdd, other.bdd));
}

BddMap
BddMap::operator-=(const Bdd& other)
{
    bdd = sylvan_map_removeall(bdd, other.bdd);
    return *this;
}

void
BddMap::put(uint32_t key, Bdd value)
{
    bdd = sylvan_map_add(bdd, key, value.bdd);
}

void
BddMap::removeKey(uint32_t key)
{
    bdd = sylvan_map_remove(bdd, key);
}

size_t
BddMap::size() const
{
    return sylvan_map_count(bdd);
}

int
BddMap::isEmpty() const
{
    return sylvan_map_isempty(bdd);
}


/***
 * Implementation of class Mtbdd
 */

Mtbdd
Mtbdd::uint64Terminal(uint64_t value)
{
    return mtbdd_uint64(value);
}

Mtbdd
Mtbdd::doubleTerminal(double value)
{
    return mtbdd_double(value);
}

Mtbdd
Mtbdd::fractionTerminal(uint64_t nominator, uint64_t denominator)
{
    return mtbdd_fraction(nominator, denominator);
}

Mtbdd
Mtbdd::terminal(uint32_t type, uint64_t value)
{
    return mtbdd_makeleaf(type, value);
}

Mtbdd
Mtbdd::mtbddVar(uint32_t variable)
{
    return mtbdd_makenode(variable, mtbdd_false, mtbdd_true);
}

Mtbdd
Mtbdd::mtbddOne()
{
    return mtbdd_true;
}

Mtbdd
Mtbdd::mtbddZero()
{
    return mtbdd_false;
}

Mtbdd
Mtbdd::mtbddCube(const Mtbdd &variables, uint8_t *values, const Mtbdd &terminal)
{
    LACE_ME;
    return mtbdd_cube(variables.mtbdd, values, terminal.mtbdd);
}

Mtbdd
Mtbdd::mtbddCube(const Mtbdd &variables, std::vector<uint8_t> values, const Mtbdd &terminal)
{
    LACE_ME;
    uint8_t *data = values.data();
    return mtbdd_cube(variables.mtbdd, data, terminal.mtbdd);
}

int
Mtbdd::isTerminal() const
{
    return mtbdd_isleaf(mtbdd);
}

int
Mtbdd::isLeaf() const
{
    return mtbdd_isleaf(mtbdd);
}

int
Mtbdd::isOne() const
{
    return mtbdd == mtbdd_true;
}

int
Mtbdd::isZero() const
{
    return mtbdd == mtbdd_false;
}

uint32_t
Mtbdd::TopVar() const
{
    return mtbdd_getvar(mtbdd);
}

Mtbdd
Mtbdd::Then() const
{
    return mtbdd_isnode(mtbdd) ? mtbdd_gethigh(mtbdd) : mtbdd;
}

Mtbdd
Mtbdd::Else() const
{
    return mtbdd_isnode(mtbdd) ? mtbdd_getlow(mtbdd) : mtbdd;
}

Mtbdd
Mtbdd::Negate() const
{
    return mtbdd_negate(mtbdd);
}

Mtbdd
Mtbdd::Apply(const Mtbdd &other, mtbdd_apply_op op) const
{
    LACE_ME;
    return mtbdd_apply(mtbdd, other.mtbdd, op);
}

Mtbdd
Mtbdd::UApply(mtbdd_uapply_op op, size_t param) const
{
    LACE_ME;
    return mtbdd_uapply(mtbdd, op, param);
}

Mtbdd
Mtbdd::Abstract(const Mtbdd &variables, mtbdd_abstract_op op) const
{
    LACE_ME;
    return mtbdd_abstract(mtbdd, variables.mtbdd, op);
}

Mtbdd
Mtbdd::Ite(const Mtbdd &g, const Mtbdd &h) const
{
    LACE_ME;
    return mtbdd_ite(mtbdd, g.mtbdd, h.mtbdd);
}

Mtbdd
Mtbdd::Plus(const Mtbdd &other) const
{
    LACE_ME;
    return mtbdd_plus(mtbdd, other.mtbdd);
}

Mtbdd
Mtbdd::Times(const Mtbdd &other) const
{
    LACE_ME;
    return mtbdd_times(mtbdd, other.mtbdd);
}

Mtbdd
Mtbdd::Min(const Mtbdd &other) const
{
    LACE_ME;
    return mtbdd_min(mtbdd, other.mtbdd);
}

Mtbdd
Mtbdd::Max(const Mtbdd &other) const
{
    LACE_ME;
    return mtbdd_max(mtbdd, other.mtbdd);
}

Mtbdd
Mtbdd::AbstractPlus(const Mtbdd &variables) const
{
    LACE_ME;
    return mtbdd_abstract_plus(mtbdd, variables.mtbdd);
}

Mtbdd
Mtbdd::AbstractTimes(const Mtbdd &variables) const
{
    LACE_ME;
    return mtbdd_abstract_times(mtbdd, variables.mtbdd);
}

Mtbdd
Mtbdd::AbstractMin(const Mtbdd &variables) const
{
    LACE_ME;
    return mtbdd_abstract_min(mtbdd, variables.mtbdd);
}

Mtbdd
Mtbdd::AbstractMax(const Mtbdd &variables) const
{
    LACE_ME;
    return mtbdd_abstract_max(mtbdd, variables.mtbdd);
}

Mtbdd
Mtbdd::AndExists(const Mtbdd &other, const Mtbdd &variables) const
{
    LACE_ME;
    return mtbdd_and_exists(mtbdd, other.mtbdd, variables.mtbdd);
}

int
Mtbdd::operator==(const Mtbdd& other) const
{
    return mtbdd == other.mtbdd;
}

int
Mtbdd::operator!=(const Mtbdd& other) const
{
    return mtbdd != other.mtbdd;
}

Mtbdd
Mtbdd::operator=(const Mtbdd& right)
{
    mtbdd = right.mtbdd;
    return *this;
}

Mtbdd
Mtbdd::operator!() const
{
    return mtbdd_not(mtbdd);
}

Mtbdd
Mtbdd::operator~() const
{
    return mtbdd_not(mtbdd);
}

Mtbdd
Mtbdd::operator*(const Mtbdd& other) const
{
    LACE_ME;
    return mtbdd_times(mtbdd, other.mtbdd);
}

Mtbdd
Mtbdd::operator*=(const Mtbdd& other)
{
    LACE_ME;
    mtbdd = mtbdd_times(mtbdd, other.mtbdd);
    return *this;
}

Mtbdd
Mtbdd::operator+(const Mtbdd& other) const
{
    LACE_ME;
    return mtbdd_plus(mtbdd, other.mtbdd);
}

Mtbdd
Mtbdd::operator+=(const Mtbdd& other)
{
    LACE_ME;
    mtbdd = mtbdd_plus(mtbdd, other.mtbdd);
    return *this;
}

Mtbdd
Mtbdd::operator-(const Mtbdd& other) const
{
    LACE_ME;
    return mtbdd_plus(mtbdd, mtbdd_negate(other.mtbdd));
}

Mtbdd
Mtbdd::operator-=(const Mtbdd& other)
{
    LACE_ME;
    mtbdd = mtbdd_plus(mtbdd, mtbdd_negate(other.mtbdd));
    return *this;
}

Mtbdd
Mtbdd::MtbddThreshold(double value) const
{
    LACE_ME;
    return mtbdd_threshold_double(mtbdd, value);
}

Mtbdd
Mtbdd::MtbddStrictThreshold(double value) const
{
    LACE_ME;
    return mtbdd_strict_threshold_double(mtbdd, value);
}

Bdd
Mtbdd::BddThreshold(double value) const
{
    LACE_ME;
    return mtbdd_threshold_double(mtbdd, value);
}

Bdd
Mtbdd::BddStrictThreshold(double value) const
{
    LACE_ME;
    return mtbdd_strict_threshold_double(mtbdd, value);
}

Mtbdd
Mtbdd::Support() const
{
    LACE_ME;
    return mtbdd_support(mtbdd);
}

MTBDD
Mtbdd::GetMTBDD() const
{
    return mtbdd;
}

Mtbdd
Mtbdd::Compose(MtbddMap &m) const
{
    LACE_ME;
    return mtbdd_compose(mtbdd, m.mtbdd);
}

double
Mtbdd::SatCount(const Mtbdd &variables) const
{
    LACE_ME;
    return mtbdd_satcount(mtbdd, variables.mtbdd);
}

size_t
Mtbdd::NodeCount() const
{
    LACE_ME;
    return mtbdd_nodecount(mtbdd);
}


/***
 * Implementation of class MtbddMap
 */

MtbddMap::MtbddMap(uint32_t key_variable, Mtbdd value)
{
    mtbdd = mtbdd_map_add(mtbdd_map_empty(), key_variable, value.mtbdd);
}

MtbddMap
MtbddMap::operator+(const Mtbdd& other) const
{
    return MtbddMap(mtbdd_map_addall(mtbdd, other.mtbdd));
}

MtbddMap
MtbddMap::operator+=(const Mtbdd& other)
{
    mtbdd = mtbdd_map_addall(mtbdd, other.mtbdd);
    return *this;
}

MtbddMap
MtbddMap::operator-(const Mtbdd& other) const
{
    return MtbddMap(mtbdd_map_removeall(mtbdd, other.mtbdd));
}

MtbddMap
MtbddMap::operator-=(const Mtbdd& other)
{
    mtbdd = mtbdd_map_removeall(mtbdd, other.mtbdd);
    return *this;
}

void
MtbddMap::put(uint32_t key, Mtbdd value)
{
    mtbdd = mtbdd_map_add(mtbdd, key, value.mtbdd);
}

void
MtbddMap::removeKey(uint32_t key)
{
    mtbdd = mtbdd_map_remove(mtbdd, key);
}

size_t
MtbddMap::size()
{
    return mtbdd_map_count(mtbdd);
}

int
MtbddMap::isEmpty()
{
    return mtbdd_map_isempty(mtbdd);
}


/***
 * Implementation of class Sylvan
 */

void
Sylvan::initPackage(size_t initialTableSize, size_t maxTableSize, size_t initialCacheSize, size_t maxCacheSize)
{
    sylvan_init_package(initialTableSize, maxTableSize, initialCacheSize, maxCacheSize);
}

void
Sylvan::initBdd(int granularity)
{
    sylvan_init_bdd(granularity);
}

void
Sylvan::initMtbdd()
{
    sylvan_init_mtbdd();
}

void
Sylvan::quitPackage()
{
    sylvan_quit();
}
