#pragma once

#include <ostream>
#include <string>

namespace storm {

/*!
 * The algorithms that can be used to compute the values underlying a certificate.
 */
enum class CertificateAlgorithm {
    FpII,        /// Interval iteration with floating point numbers
    FpSmoothII,  /// Smooth interval iteration with floating point numbers
    FpRoundII,   /// Interval iteration with floating point numbers where lower bounds are rounded towards the sound direction
    ExPI         /// Policy iteration with exact (rational) numbers
};

std::ostream& operator<<(std::ostream& stream, CertificateAlgorithm const& algorithm);
CertificateAlgorithm certificateAlgorithmFromString(std::string const& algorithm);

}  // namespace storm
