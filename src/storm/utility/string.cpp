#include "storm/utility/string.h"
#include <vector>
#include <boost/algorithm/string/join.hpp>

namespace storm {
    namespace utility {
        namespace string {

            SimilarStrings::SimilarStrings(std::string reference, double similarityFactor, bool caseSensitive) : reference(reference), similarityFactor(similarityFactor), caseSensitive(caseSensitive), cmp([](std::pair<uint64_t, std::string> const& lhs, std::pair<uint64_t, std::string> const& rhs) { return lhs.first > rhs.first; }), distances(cmp) {
                // intentionally left empty.
            }
            
            bool SimilarStrings::add(std::string const& string) {
                double distance = levenshteinDistance(reference, string, caseSensitive);
                if (distance <= static_cast<double>(std::max(reference.size(), string.size())) * (1.0 - similarityFactor)) {
                    distances.emplace(storm::utility::string::levenshteinDistance(reference, string, caseSensitive), string);
                    return true;
                }
                return false;
            }
            
            std::vector<std::string> SimilarStrings::toList() const {
                auto distancesCopy = distances;
                std::vector<std::string> result;
                while (!distancesCopy.empty()) {
                    result.push_back(distancesCopy.top().second);
                    distancesCopy.pop();
                }
                return result;
            }
            
            std::string SimilarStrings::toDidYouMeanString() const {
                uint64_t size = distances.size();
                std::string result = boost::algorithm::join(toList(), ", ");
                if (size == 0) {
                    return "";
                } else if (size == 1) {
                    return "Did you mean " + result + "?";
                } else {
                    return "Did you mean any of [" + result + "]?";
                }
            }
            
            
            uint64_t levenshteinDistance(std::string const& lhs, std::string const& rhs, bool caseSensitive) {
                std::vector<std::vector<uint64_t>> d(lhs.size() + 1, std::vector<uint64_t>(rhs.size() + 1, 0ull));
                for (uint64_t row = 1; row < d.size(); ++row) {
                    d[row].front() = row;
                }
                for (uint64_t col = 1; col < d.front().size(); ++col) {
                    d.front()[col] = col;
                }
                
                for (uint64_t row = 1; row < d.size(); ++row) {
                    for (uint64_t col = 1; col < d[row].size(); ++col) {
                        uint64_t cost = 1;
                        if (caseSensitive) {
                            if (tolower(lhs[row-1]) == tolower(rhs[col-1])) {
                                cost = 0;
                            }
                        } else {
                            if (lhs[row-1] == rhs[col-1]) {
                                cost = 0;
                            }
                        }
                        d[row][col] = std::min( { d[row-1][col] + 1, d[row][col - 1] + 1, d[row-1][col-1] + cost } );
                    }
                }
                return d.back().back();
            }
        }
    }
}