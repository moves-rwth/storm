#ifndef STORM_STATEANNOTATION_H
#define STORM_STATEANNOTATION_H

namespace storm {
    namespace models {
        namespace sparse {
            class StateAnnotation {
            public:
                virtual std::string stateInfo(uint_fast64_t s) const = 0;
            };
        }
    }

}

#endif //STORM_STATEANNOTATION_H