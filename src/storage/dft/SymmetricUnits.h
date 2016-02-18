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
                std::cout << "SYM GROUP FOR " << cl.first << std::endl;
                for(auto const& eqClass : cl.second) {
                    for(auto const& i : eqClass) {
                        std::cout << i << " ";
                    }
                    std::cout << std::endl;
                }
            }
            
            
            return os;
        }
    }
}