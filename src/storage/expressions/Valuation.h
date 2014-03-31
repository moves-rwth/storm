#ifndef STORM_STORAGE_EXPRESSIONS_VALUATION_H_
#define STORM_STORAGE_EXPRESSIONS_VALUATION_H_

namespace storm {
    namespace expressions {
        class Valuation {
        public:
            virtual bool getBooleanValue(std::string const& name) const = 0;
            virtual int_fast64_t getIntegerValue(std::string const& name) const = 0;
            virtual double getDoubleValue(std::string const& name) const = 0;
        };
    }
}

#endif /* STORM_STORAGE_EXPRESSIONS_VALUATION_H_ */