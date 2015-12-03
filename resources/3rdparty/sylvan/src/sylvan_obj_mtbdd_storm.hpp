    /**
     * @brief Computes f - g
     */
    Mtbdd Minus(const Mtbdd &other) const;

    /**
     * @brief Computes f / g
     */
    Mtbdd Divide(const Mtbdd &other) const;
    
    Bdd NotZero() const;
    
    Bdd Equals(const Mtbdd& other) const;
    
    Bdd Less(const Mtbdd& other) const;

    Bdd LessOrEqual(const Mtbdd& other) const;
    
    double getDoubleMax() const;

    double getDoubleMin() const;
    
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