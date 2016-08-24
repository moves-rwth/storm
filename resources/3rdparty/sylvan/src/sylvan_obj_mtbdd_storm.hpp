    /**
     * @brief Computes f - g
     */
    Mtbdd Minus(const Mtbdd &other) const;

    /**
     * @brief Computes f / g
     */
    Mtbdd Divide(const Mtbdd &other) const;
    
#if defined(SYLVAN_HAVE_CARL) || defined(STORM_HAVE_CARL)
	/**
     * @brief Computes f + g for Rational Functions
     */
    Mtbdd PlusRF(const Mtbdd &other) const;

    /**
     * @brief Computes f * g for Rational Functions
     */
    Mtbdd TimesRF(const Mtbdd &other) const;
	
	/**
     * @brief Computes f - g for Rational Functions
     */
    Mtbdd MinusRF(const Mtbdd &other) const;

    /**
     * @brief Computes f / g for Rational Functions
     */
    Mtbdd DivideRF(const Mtbdd &other) const;
	
	Mtbdd AbstractPlusRF(const BddSet &variables) const;
#endif
	
	/**
     * @brief Computes abstraction by minimum
     */
    Bdd AbstractMinRepresentative(const BddSet &variables) const;

    /**
     * @brief Computes abstraction by maximum
     */
    Bdd AbstractMaxRepresentative(const BddSet &variables) const;
	
    Bdd NotZero() const;
    
    Bdd Equals(const Mtbdd& other) const;
    
    Bdd Less(const Mtbdd& other) const;

    Bdd LessOrEqual(const Mtbdd& other) const;

    Mtbdd Minimum() const;

    Mtbdd Maximum() const;

    bool EqualNorm(const Mtbdd& other, double epsilon) const;

    bool EqualNormRel(const Mtbdd& other, double epsilon) const;
    
    Mtbdd Floor() const;

    Mtbdd Ceil() const;
    
    Mtbdd Pow(const Mtbdd& other) const;

    Mtbdd Mod(const Mtbdd& other) const;

    Mtbdd Logxy(const Mtbdd& other) const;
    
    size_t CountLeaves() const;

    /**
     * @brief Compute the number of non-zero variable assignments, using variables in cube.
     */
    double NonZeroCount(size_t variableCount) const;

    bool isValid() const;

    /**
     * @brief Writes .dot file of this Bdd. Not thread-safe!
     */
    void PrintDot(FILE *out) const;

    std::string GetShaHash() const;
