#pragma once

#include <string>
#include <functional>
#include <queue>

namespace storm {
    namespace utility {
        namespace string {
            
            class SimilarStrings {
            public:
                /*!
                 * Gathers strings that are similar to the given reference string
                 * @param reference
                 * @param similarityFactor controls how similar the strings need to be (0 means any string is similar, 1 means only the reference string is similar)
                 * @param caseSensitive if false, lower/upper case is ignored
                 */
                SimilarStrings(std::string reference, double similarityFactor = 0.6, bool caseSensitive = true);
                
                /*!
                 * Adds the given string to the set of similar strings (if it is similar)
                 * @return true, if the given string is considered similar.
                 */
                bool add(std::string const& string);
                
                /*!
                 * Gets a list of all added strings that are similar to the reference string.
                 * Erases all strings gathered so far.
                 */
                std::vector<std::string> toList() const;
                
                /*!
                 * Returns a "Did you mean abc?" string
                 * @return
                 */
                std::string toDidYouMeanString() const;

            private:
                std::string reference;
                double similarityFactor;
                bool caseSensitive;
                std::function<bool (std::pair<uint64_t, std::string> const&, std::pair<uint64_t, std::string> const&)> cmp;
                std::priority_queue<std::pair<uint64_t, std::string>, std::vector<std::pair<uint64_t, std::string>>, decltype(cmp)> distances;
            };
            
            /*!
             * Levenstein distance to find similar strings
             */
            uint64_t levenshteinDistance(std::string const& lhs, std::string const& rhs, bool caseSensitive = true);
        }
    }
}