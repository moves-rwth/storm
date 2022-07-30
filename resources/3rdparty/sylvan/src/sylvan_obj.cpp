/*
 * Copyright 2011-2016 Formal Methods and Tools, University of Twente
 * Copyright 2016 Tom van Dijk, Johannes Kepler University Linz
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

bool
Bdd::operator==(const Bdd& other) const
{
    return bdd == other.bdd;
}

bool
Bdd::operator!=(const Bdd& other) const
{
    return bdd != other.bdd;
}

Bdd&
Bdd::operator=(const Bdd& right)
{
    bdd = right.bdd;
    return *this;
}

bool
Bdd::operator<=(const Bdd& other) const
{
    // TODO: better implementation, since we are not interested in the BDD result
    BDD r = sylvan_ite(this->bdd, sylvan_not(other.bdd), sylvan_false);
    return r == sylvan_false;
}

bool
Bdd::operator>=(const Bdd& other) const
{
    // TODO: better implementation, since we are not interested in the BDD result
    return other <= *this;
}

bool
Bdd::operator<(const Bdd& other) const
{
    return bdd != other.bdd && *this <= other;
}

bool
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
    return Bdd(sylvan_and(bdd, other.bdd));
}

Bdd&
Bdd::operator*=(const Bdd& other)
{
    bdd = sylvan_and(bdd, other.bdd);
    return *this;
}

Bdd
Bdd::operator&(const Bdd& other) const
{
    return Bdd(sylvan_and(bdd, other.bdd));
}

Bdd&
Bdd::operator&=(const Bdd& other)
{
    bdd = sylvan_and(bdd, other.bdd);
    return *this;
}

Bdd
Bdd::operator+(const Bdd& other) const
{
    return Bdd(sylvan_or(bdd, other.bdd));
}

Bdd&
Bdd::operator+=(const Bdd& other)
{
    bdd = sylvan_or(bdd, other.bdd);
    return *this;
}

Bdd
Bdd::operator|(const Bdd& other) const
{
    return Bdd(sylvan_or(bdd, other.bdd));
}

Bdd&
Bdd::operator|=(const Bdd& other)
{
    bdd = sylvan_or(bdd, other.bdd);
    return *this;
}

Bdd
Bdd::operator^(const Bdd& other) const
{
    return Bdd(sylvan_xor(bdd, other.bdd));
}

Bdd&
Bdd::operator^=(const Bdd& other)
{
    bdd = sylvan_xor(bdd, other.bdd);
    return *this;
}

Bdd
Bdd::operator-(const Bdd& other) const
{
    return Bdd(sylvan_and(bdd, sylvan_not(other.bdd)));
}

Bdd&
Bdd::operator-=(const Bdd& other)
{
    bdd = sylvan_and(bdd, sylvan_not(other.bdd));
    return *this;
}

Bdd
Bdd::AndAbstract(const Bdd &g, const BddSet &cube) const
{
    return sylvan_and_exists(bdd, g.bdd, cube.set.bdd);
}

Bdd
Bdd::ExistAbstract(const BddSet &cube) const
{
    return sylvan_exists(bdd, cube.set.bdd);
}

Bdd
Bdd::UnivAbstract(const BddSet &cube) const
{
    return sylvan_forall(bdd, cube.set.bdd);
}

Bdd
Bdd::Ite(const Bdd &g, const Bdd &h) const
{
    return sylvan_ite(bdd, g.bdd, h.bdd);
}

Bdd
Bdd::And(const Bdd &g) const
{
    return sylvan_and(bdd, g.bdd);
}

Bdd
Bdd::Or(const Bdd &g) const
{
    return sylvan_or(bdd, g.bdd);
}

Bdd
Bdd::Nand(const Bdd &g) const
{
    return sylvan_nand(bdd, g.bdd);
}

Bdd
Bdd::Nor(const Bdd &g) const
{
    return sylvan_nor(bdd, g.bdd);
}

Bdd
Bdd::Xor(const Bdd &g) const
{
    return sylvan_xor(bdd, g.bdd);
}

Bdd
Bdd::Xnor(const Bdd &g) const
{
    return sylvan_equiv(bdd, g.bdd);
}

bool
Bdd::Leq(const Bdd &g) const
{
    // TODO: better implementation, since we are not interested in the BDD result
    BDD r = sylvan_ite(bdd, sylvan_not(g.bdd), sylvan_false);
    return r == sylvan_false;
}

Bdd
Bdd::RelPrev(const Bdd& relation, const BddSet& cube) const
{
    return sylvan_relprev(relation.bdd, bdd, cube.set.bdd);
}

Bdd
Bdd::RelNext(const Bdd &relation, const BddSet &cube) const
{
    return sylvan_relnext(bdd, relation.bdd, cube.set.bdd);
}

Bdd
Bdd::Closure() const
{
    return sylvan_closure(bdd);
}

Bdd
Bdd::Constrain(const Bdd &c) const
{
    return sylvan_constrain(bdd, c.bdd);
}

Bdd
Bdd::Restrict(const Bdd &c) const
{
    return sylvan_restrict(bdd, c.bdd);
}

Bdd
Bdd::Compose(const BddMap &m) const
{
    return sylvan_compose(bdd, m.bdd);
}

Bdd
Bdd::Permute(const std::vector<uint32_t>& from, const std::vector<uint32_t>& to) const
{
    /* Create a map */
    BddMap map;
    for (int i=from.size()-1; i>=0; i--) {
        map.put(from[i], Bdd::bddVar(to[i]));
    }

    return sylvan_compose(bdd, map.bdd);
}

Bdd
Bdd::Support() const
{
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
Bdd::SatCount(const BddSet &variables) const
{
    return sylvan_satcount(bdd, variables.set.bdd);
}

double
Bdd::SatCount(size_t nvars) const
{
    // Note: the mtbdd_satcount can be called without initializing the MTBDD module.
    return mtbdd_satcount(bdd, nvars);
}

void
Bdd::PickOneCube(const BddSet &variables, uint8_t *values) const
{
    sylvan_sat_one(bdd, variables.set.bdd, values);
}

std::vector<bool>
Bdd::PickOneCube(const BddSet &variables) const
{
    std::vector<bool> result = std::vector<bool>();

    BDD bdd = this->bdd;
    BDD vars = variables.set.bdd;

    if (bdd == sylvan_false) return result;

    for (; !sylvan_set_isempty(vars); vars = sylvan_set_next(vars)) {
        uint32_t var = sylvan_set_first(vars);
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
    return Bdd(sylvan_sat_one_bdd(bdd));
}

Bdd
Bdd::UnionCube(const BddSet &variables, uint8_t *values) const
{
    return sylvan_union_cube(bdd, variables.set.bdd, values);
}

Bdd
Bdd::UnionCube(const BddSet &variables, std::vector<uint8_t> values) const
{
    uint8_t *data = values.data();
    return sylvan_union_cube(bdd, variables.set.bdd, data);
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
    return sylvan_ithvar(index);
}

Bdd
Bdd::bddCube(const BddSet &variables, uint8_t *values)
{
    return sylvan_cube(variables.set.bdd, values);
}

Bdd
Bdd::bddCube(const BddSet &variables, std::vector<uint8_t> values)
{
    uint8_t *data = values.data();
    return sylvan_cube(variables.set.bdd, data);
}

bool
Bdd::isConstant() const
{
    return bdd == sylvan_true || bdd == sylvan_false;
}

bool
Bdd::isTerminal() const
{
    return bdd == sylvan_true || bdd == sylvan_false;
}

bool
Bdd::isOne() const
{
    return bdd == sylvan_true;
}

bool
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

BddMap&
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

BddMap&
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

bool
BddMap::isEmpty() const
{
    return sylvan_map_isempty(bdd);
}


/***
 * Implementation of class Mtbdd
 */

Mtbdd
Mtbdd::int64Terminal(int64_t value)
{
    return mtbdd_int64(value);
}

Mtbdd
Mtbdd::doubleTerminal(double value)
{
    return mtbdd_double(value);
}

Mtbdd
Mtbdd::fractionTerminal(int64_t nominator, uint64_t denominator)
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
Mtbdd::mtbddCube(const BddSet &variables, uint8_t *values, const Mtbdd &terminal)
{
    return mtbdd_cube(variables.set.bdd, values, terminal.mtbdd);
}

Mtbdd
Mtbdd::mtbddCube(const BddSet &variables, std::vector<uint8_t> values, const Mtbdd &terminal)
{
    uint8_t *data = values.data();
    return mtbdd_cube(variables.set.bdd, data, terminal.mtbdd);
}

bool
Mtbdd::isTerminal() const
{
    return mtbdd_isleaf(mtbdd);
}

bool
Mtbdd::isLeaf() const
{
    return mtbdd_isleaf(mtbdd);
}

bool
Mtbdd::isOne() const
{
    return mtbdd == mtbdd_true;
}

bool
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
    return mtbdd_apply(mtbdd, other.mtbdd, op);
}

Mtbdd
Mtbdd::UApply(mtbdd_uapply_op op, size_t param) const
{
    return mtbdd_uapply(mtbdd, op, param);
}

Mtbdd
Mtbdd::Abstract(const BddSet &variables, mtbdd_abstract_op op) const
{
    return mtbdd_abstract(mtbdd, variables.set.bdd, op);
}

Mtbdd
Mtbdd::Ite(const Mtbdd &g, const Mtbdd &h) const
{
    return mtbdd_ite(mtbdd, g.mtbdd, h.mtbdd);
}

Mtbdd
Mtbdd::Plus(const Mtbdd &other) const
{
    return mtbdd_plus(mtbdd, other.mtbdd);
}

Mtbdd
Mtbdd::Times(const Mtbdd &other) const
{
    return mtbdd_times(mtbdd, other.mtbdd);
}

Mtbdd
Mtbdd::Min(const Mtbdd &other) const
{
    return mtbdd_min(mtbdd, other.mtbdd);
}

Mtbdd
Mtbdd::Max(const Mtbdd &other) const
{
    return mtbdd_max(mtbdd, other.mtbdd);
}

Mtbdd
Mtbdd::AbstractPlus(const BddSet &variables) const
{
    return mtbdd_abstract_plus(mtbdd, variables.set.bdd);
}

Mtbdd
Mtbdd::AbstractTimes(const BddSet &variables) const
{
    return mtbdd_abstract_times(mtbdd, variables.set.bdd);
}

Mtbdd
Mtbdd::AbstractMin(const BddSet &variables) const
{
    return mtbdd_abstract_min(mtbdd, variables.set.bdd);
}

Mtbdd
Mtbdd::AbstractMax(const BddSet &variables) const
{
    return mtbdd_abstract_max(mtbdd, variables.set.bdd);
}

Mtbdd
Mtbdd::AndExists(const Mtbdd &other, const BddSet &variables) const
{
    return mtbdd_and_exists(mtbdd, other.mtbdd, variables.set.bdd);
}

bool
Mtbdd::operator==(const Mtbdd& other) const
{
    return mtbdd == other.mtbdd;
}

bool
Mtbdd::operator!=(const Mtbdd& other) const
{
    return mtbdd != other.mtbdd;
}

Mtbdd&
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
    return mtbdd_times(mtbdd, other.mtbdd);
}

Mtbdd&
Mtbdd::operator*=(const Mtbdd& other)
{
    mtbdd = mtbdd_times(mtbdd, other.mtbdd);
    return *this;
}

Mtbdd
Mtbdd::operator+(const Mtbdd& other) const
{
    return mtbdd_plus(mtbdd, other.mtbdd);
}

Mtbdd&
Mtbdd::operator+=(const Mtbdd& other)
{
    mtbdd = mtbdd_plus(mtbdd, other.mtbdd);
    return *this;
}

Mtbdd
Mtbdd::operator-(const Mtbdd& other) const
{
    return mtbdd_minus(mtbdd, other.mtbdd);
}

Mtbdd&
Mtbdd::operator-=(const Mtbdd& other)
{
    mtbdd = mtbdd_minus(mtbdd, other.mtbdd);
    return *this;
}

Mtbdd
Mtbdd::MtbddThreshold(double value) const
{
    return mtbdd_threshold_double(mtbdd, value);
}

Mtbdd
Mtbdd::MtbddStrictThreshold(double value) const
{
    return mtbdd_strict_threshold_double(mtbdd, value);
}

Bdd
Mtbdd::BddThreshold(double value) const
{
    return mtbdd_threshold_double(mtbdd, value);
}

Bdd
Mtbdd::BddStrictThreshold(double value) const
{
    return mtbdd_strict_threshold_double(mtbdd, value);
}

Mtbdd
Mtbdd::Support() const
{
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
    return mtbdd_compose(mtbdd, m.mtbdd);
}

Mtbdd
Mtbdd::Permute(const std::vector<uint32_t>& from, const std::vector<uint32_t>& to) const
{
    /* Create a map */
    MtbddMap map;
    for (int i=from.size()-1; i>=0; i--) {
        map.put(from[i], Bdd::bddVar(to[i]));
    }

    return mtbdd_compose(mtbdd, map.mtbdd);
}

double
Mtbdd::SatCount(size_t nvars) const
{
    return mtbdd_satcount(mtbdd, nvars);
}

double
Mtbdd::SatCount(const BddSet &variables) const
{
    return SatCount(sylvan_set_count(variables.set.bdd));
}

size_t
Mtbdd::NodeCount() const
{
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

MtbddMap&
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

MtbddMap&
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

bool
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
    sylvan_set_sizes(initialTableSize, maxTableSize, initialCacheSize, maxCacheSize);
    sylvan_init_package();
}

void
Sylvan::setGranularity(int granularity)
{
    sylvan_set_granularity(granularity);
}

int
Sylvan::getGranularity()
{
    return sylvan_get_granularity();
}

void
Sylvan::initBdd()
{
    sylvan_init_bdd();
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
