#pragma once

#include <map>
#include <string>
#include <vector>

namespace storm {
namespace automata {
class APSet {
   public:
    // TODO: uint32
    typedef std::size_t alphabet_element;

    APSet();

    unsigned int size() const;
    std::size_t alphabetSize() const;
    void add(const std::string& ap);
    unsigned int getIndex(const std::string& ap) const;
    bool contains(const std::string& ap) const;
    const std::string& getAP(unsigned int index) const;
    const std::vector<std::string>& getAPs() const;

    alphabet_element elementAllFalse() const;
    alphabet_element elementAddAP(alphabet_element element, unsigned int ap) const;

    const unsigned int MAX_APS = 32;

   private:
    std::map<std::string, unsigned int> ap_to_index;
    std::vector<std::string> index_to_ap;
};
}  // namespace automata
}  // namespace storm
