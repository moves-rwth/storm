#include "src/utility/vector.h"

// Template was causing problems as Carl has the same function
/*
template<typename ValueType>
std::ostream& operator<<(std::ostream& out, std::vector<ValueType> const& vector) {
    out << "vector (" << vector.size() << ") [ ";
    for (uint_fast64_t i = 0; i < vector.size() - 1; ++i) {
        out << vector[i] << ", ";
    }
    out << vector.back();
    out << " ]";
    return out;
}

// Explicitly instantiate functions.
template std::ostream& operator<<(std::ostream& out, std::vector<double> const& vector);
template std::ostream& operator<<(std::ostream& out, std::vector<uint_fast64_t> const& vector);
*/

std::ostream& operator<<(std::ostream& out, std::vector<double> const& vector) {
    out << "vector (" << vector.size() << ") [ ";
    for (uint_fast64_t i = 0; i < vector.size() - 1; ++i) {
        out << vector[i] << ", ";
    }
    out << vector.back();
    out << " ]";
    return out;
}

std::ostream& operator<<(std::ostream& out, std::vector<uint_fast64_t> const& vector) {
    out << "vector (" << vector.size() << ") [ ";
    for (uint_fast64_t i = 0; i < vector.size() - 1; ++i) {
        out << vector[i] << ", ";
    }
    out << vector.back();
    out << " ]";
    return out;
}