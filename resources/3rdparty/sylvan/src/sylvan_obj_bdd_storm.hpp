    Mtbdd toDoubleMtbdd() const;
    Mtbdd toInt64Mtbdd() const;
#if defined(SYLVAN_HAVE_CARL) || defined(STORM_HAVE_CARL)
	Mtbdd toStormRationalFunctionMtbdd() const;
#endif
    Mtbdd Ite(Mtbdd const& thenDd, Mtbdd const& elseDd) const;
