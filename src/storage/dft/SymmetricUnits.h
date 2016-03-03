#pragma once

namespace storm {
    namespace storage {
        struct DFTIndependentSymmetries {
            std::map<size_t, std::vector<std::vector<size_t>>> groups;
            
            DFTIndependentSymmetries(std::map<size_t, std::vector<std::vector<size_t>>> groups) : groups(groups) {
                
            }
        };
        
        inline std::ostream& operator<<(std::ostream& os, DFTIndependentSymmetries const& s)  {
            for(auto const& cl : s.groups) {
                os << "Symmetry group for " << cl.first << std::endl;
                for(auto const& eqClass : cl.second) {
                    for(auto const& i : eqClass) {
                        os << i << " ";
                    }
                    os << std::endl;
                }
            }
            return os;
        }
    }
}