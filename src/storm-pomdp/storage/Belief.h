namespace storm {
    namespace pomdp {
        // Structure used to represent a belief
        template<typename ValueType>
        struct Belief {
            uint64_t id;
            uint32_t observation;
            //TODO make this sparse?
            std::map<uint64_t, ValueType> probabilities;
        };
    }
}