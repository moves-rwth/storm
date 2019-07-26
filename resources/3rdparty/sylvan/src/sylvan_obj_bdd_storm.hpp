// Methods to convert a BDD to the canonical 0/1 MTBDD for different types.
Mtbdd toDoubleMtbdd() const;
Mtbdd toInt64Mtbdd() const;
Mtbdd toStormRationalNumberMtbdd() const;

void PrintText(FILE *out) const;
#if defined(SYLVAN_HAVE_CARL) || defined(STORM_HAVE_CARL)
Mtbdd toStormRationalFunctionMtbdd() const;
#endif

// Other functions to add to sylvan's Bdd class.
Mtbdd Ite(Mtbdd const& thenDd, Mtbdd const& elseDd) const;
Bdd ExistAbstractRepresentative(const BddSet& cube) const;
