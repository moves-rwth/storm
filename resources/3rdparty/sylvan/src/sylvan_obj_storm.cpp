Bdd
Bdd::ExistAbstractRepresentative(const BddSet& cube) const {
	LACE_ME;
    return sylvan_existsRepresentative(bdd, cube.set.bdd);
}

Mtbdd
Bdd::toDoubleMtbdd() const {
    LACE_ME;
    return mtbdd_bool_to_double(bdd);
}

Mtbdd
Bdd::toInt64Mtbdd() const {
    LACE_ME;
    return mtbdd_bool_to_int64(bdd);
}

#if defined(SYLVAN_HAVE_CARL) || defined(STORM_HAVE_CARL)
Mtbdd
Bdd::toStormRationalFunctionMtbdd() const {
    LACE_ME;
    return mtbdd_bool_to_storm_rational_function(bdd);
}

Mtbdd
Mtbdd::PlusRF(const Mtbdd &other) const
{
    LACE_ME;
    return sylvan_storm_rational_function_plus(mtbdd, other.mtbdd);
}


Mtbdd
Mtbdd::TimesRF(const Mtbdd &other) const
{
    LACE_ME;
    return sylvan_storm_rational_function_times(mtbdd, other.mtbdd);
}

Mtbdd
Mtbdd::MinusRF(const Mtbdd &other) const
{
    LACE_ME;
    return sylvan_storm_rational_function_minus(mtbdd, other.mtbdd);
}

Mtbdd
Mtbdd::DivideRF(const Mtbdd &other) const
{
    LACE_ME;
    return sylvan_storm_rational_function_divide(mtbdd, other.mtbdd);
}

Mtbdd Mtbdd::AbstractPlusRF(const BddSet &variables) const {
	LACE_ME;
    return sylvan_storm_rational_function_abstract_plus(mtbdd, variables.set.bdd);
}

#endif

Mtbdd
Bdd::Ite(Mtbdd const& thenDd, Mtbdd const& elseDd) const {
    LACE_ME;
    return mtbdd_ite(bdd, thenDd.GetMTBDD(), elseDd.GetMTBDD());
}

Mtbdd
Mtbdd::Minus(const Mtbdd &other) const
{
    LACE_ME;
    return mtbdd_minus(mtbdd, other.mtbdd);
}

Mtbdd
Mtbdd::Divide(const Mtbdd &other) const
{
    LACE_ME;
    return mtbdd_divide(mtbdd, other.mtbdd);
}

Bdd
Mtbdd::NotZero() const
{
    LACE_ME;
    return mtbdd_not_zero(mtbdd);
}

Bdd
Mtbdd::Equals(const Mtbdd& other) const {
    LACE_ME;
    return mtbdd_equals(mtbdd, other.mtbdd);
}

Bdd
Mtbdd::Less(const Mtbdd& other) const {
    LACE_ME;
    return mtbdd_less_as_bdd(mtbdd, other.mtbdd);
}

Bdd
Mtbdd::LessOrEqual(const Mtbdd& other) const {
    LACE_ME;
    return mtbdd_less_or_equal_as_bdd(mtbdd, other.mtbdd);
}

bool
Mtbdd::EqualNorm(const Mtbdd& other, double epsilon) const {
    LACE_ME;
    return mtbdd_equal_norm_d(mtbdd, other.mtbdd, epsilon);
}

bool
Mtbdd::EqualNormRel(const Mtbdd& other, double epsilon) const {
    LACE_ME;
    return mtbdd_equal_norm_rel_d(mtbdd, other.mtbdd, epsilon);
}

Mtbdd
Mtbdd::Floor() const {
    LACE_ME;
    return mtbdd_floor(mtbdd);
}

Mtbdd
Mtbdd::Ceil() const {
    LACE_ME;
    return mtbdd_ceil(mtbdd);
}

Mtbdd
Mtbdd::Pow(const Mtbdd& other) const {
    LACE_ME;
    return mtbdd_pow(mtbdd, other.mtbdd);
}

Mtbdd
Mtbdd::Mod(const Mtbdd& other) const {
    LACE_ME;
    return mtbdd_mod(mtbdd, other.mtbdd);
}

Mtbdd
Mtbdd::Logxy(const Mtbdd& other) const {
    LACE_ME;
    return mtbdd_logxy(mtbdd, other.mtbdd);
}

size_t
Mtbdd::CountLeaves() const {
    LACE_ME;
    return mtbdd_leafcount(mtbdd);
}

double
Mtbdd::NonZeroCount(size_t variableCount) const {
    LACE_ME;
    return mtbdd_non_zero_count(mtbdd, variableCount);
}

bool
Mtbdd::isValid() const {
    LACE_ME;
    return mtbdd_test_isvalid(mtbdd) == 1;
}

Mtbdd
Mtbdd::Minimum() const {
    LACE_ME;
    return mtbdd_minimum(mtbdd);
}

Mtbdd
Mtbdd::Maximum() const {
    LACE_ME;
    return mtbdd_maximum(mtbdd);
}

void
Mtbdd::PrintDot(FILE *out) const {
    mtbdd_fprintdot(out, mtbdd, NULL);
}

std::string
Mtbdd::GetShaHash() const {
    char buf[65];
    mtbdd_getsha(mtbdd, buf);
    return std::string(buf);
}

