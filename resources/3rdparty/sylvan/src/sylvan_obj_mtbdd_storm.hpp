// Functions that are to be added to sylvan's Mtbdd class.

// Functions that operate on all or standard Mtbdds.
Bdd NotZero() const;
size_t CountLeaves() const;
double NonZeroCount(size_t variableCount) const;
bool isValid() const;
void PrintDot(FILE *out) const;
std::string GetShaHash() const;
void PrintText(FILE *out) const;

Mtbdd Minus(const Mtbdd &other) const;
Mtbdd Divide(const Mtbdd &other) const;

Bdd Equals(const Mtbdd& other) const;
Bdd Less(const Mtbdd& other) const;
Bdd LessOrEqual(const Mtbdd& other) const;

Bdd AbstractMinRepresentative(const BddSet &variables) const;
Bdd AbstractMaxRepresentative(const BddSet &variables) const;

Mtbdd Pow(const Mtbdd& other) const;
Mtbdd Mod(const Mtbdd& other) const;
Mtbdd Logxy(const Mtbdd& other) const;

Mtbdd Floor() const;
Mtbdd Ceil() const;
Mtbdd Minimum() const;
Mtbdd Maximum() const;

bool EqualNorm(const Mtbdd& other, double epsilon) const;
bool EqualNormRel(const Mtbdd& other, double epsilon) const;

Mtbdd SharpenKwekMehlhorn(size_t precision) const;

Mtbdd ToRationalNumber() const;

// Functions that operate on Mtbdds over rational numbers.
static Mtbdd stormRationalNumberTerminal(storm::RationalNumber const& value);

Bdd EqualsRN(const Mtbdd& other) const;
Bdd LessRN(const Mtbdd& other) const;
Bdd LessOrEqualRN(const Mtbdd& other) const;

Mtbdd MinRN(const Mtbdd& other) const;
Mtbdd MaxRN(const Mtbdd& other) const;

Mtbdd PlusRN(const Mtbdd &other) const;
Mtbdd MinusRN(const Mtbdd &other) const;
Mtbdd TimesRN(const Mtbdd &other) const;
Mtbdd DivideRN(const Mtbdd &other) const;

Mtbdd FloorRN() const;
Mtbdd CeilRN() const;
Mtbdd PowRN(const Mtbdd& other) const;
Mtbdd MinimumRN() const;
Mtbdd MaximumRN() const;

Mtbdd AndExistsRN(const Mtbdd &other, const BddSet &variables) const;
Mtbdd AbstractPlusRN(const BddSet &variables) const;
Mtbdd AbstractMinRN(const BddSet &variables) const;
Mtbdd AbstractMaxRN(const BddSet &variables) const;

Bdd AbstractMinRepresentativeRN(const BddSet &variables) const;
Bdd AbstractMaxRepresentativeRN(const BddSet &variables) const;

Bdd BddThresholdRN(storm::RationalNumber const& rn) const;
Bdd BddStrictThresholdRN(storm::RationalNumber const& rn) const;

bool EqualNormRN(const Mtbdd& other, storm::RationalNumber const& epsilon) const;
bool EqualNormRelRN(const Mtbdd& other, storm::RationalNumber const& epsilon) const;

Mtbdd ToDoubleRN() const;

// Functions that operate on Mtbdds over rational functions.
#if defined(SYLVAN_HAVE_CARL) || defined(STORM_HAVE_CARL)
static Mtbdd stormRationalFunctionTerminal(storm::RationalFunction const& value);

Bdd EqualsRF(const Mtbdd& other) const;
Bdd LessRF(const Mtbdd& other) const;
Bdd LessOrEqualRF(const Mtbdd& other) const;

Mtbdd MinRF(const Mtbdd& other) const;
Mtbdd MaxRF(const Mtbdd& other) const;

Mtbdd PlusRF(const Mtbdd &other) const;
Mtbdd MinusRF(const Mtbdd &other) const;
Mtbdd TimesRF(const Mtbdd &other) const;
Mtbdd DivideRF(const Mtbdd &other) const;

Mtbdd FloorRF() const;
Mtbdd CeilRF() const;
Mtbdd PowRF(const Mtbdd& other) const;
Mtbdd MinimumRF() const;
Mtbdd MaximumRF() const;

Mtbdd AndExistsRF(const Mtbdd &other, const BddSet &variables) const;
Mtbdd AbstractPlusRF(const BddSet &variables) const;
Mtbdd AbstractMinRF(const BddSet &variables) const;
Mtbdd AbstractMaxRF(const BddSet &variables) const;

Bdd BddThresholdRF(storm::RationalFunction const& rf) const;
Bdd BddStrictThresholdRF(storm::RationalFunction const& rf) const;

bool EqualNormRF(const Mtbdd& other, storm::RationalFunction const& epsilon) const;
bool EqualNormRelRF(const Mtbdd& other, storm::RationalFunction const& epsilon) const;

Mtbdd ToDoubleRF() const;
#endif

