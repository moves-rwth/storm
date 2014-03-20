#include "src/storage/dd/CuddDd.h"

namespace storm {
    namespace dd {
        Dd(std::shared_ptr<DdManager<CUDD>> ddManager, ADD cuddAdd, std::unordered_set<std::string> const& containedMetaVariableNames) noexcept : ddManager(ddManager), cuddAdd(cuddAdd), containedMetaVariableNames(containedMetaVariableNames) {
            // Intentionally left empty.
        }
        
        Dd<CUDD> Dd<CUDD>::operator+(Dd<CUDD> const& other) const {
            Dd<CUDD> result(*this);
            result += other;
            return result;
        }
        
        Dd<CUDD>& Dd<CUDD>::operator+=(Dd<CUDD> const& other) {
            cuddAdd += other;
            
            // Join the variable sets of the two participating DDs.
            std::copy(other.getContainedMetaVariableNames().begin(), other.getContainedMetaVariableNames().containedMetaVariableNames.end(), std::inserter(this->containedMetaVariableNames, this->containedMetaVariableNames.end()));
            
            return *this;
        }
        
        Dd<CUDD> Dd<CUDD>::operator*(Dd<CUDD> const& other) const {
            Dd<CUDD> result(*this);
            result *= other;
            return result;
        }
        
        Dd<CUDD>& Dd<CUDD>::operator*=(Dd<CUDD> const& other) {
            cuddAdd *= other;
            
            // Join the variable sets of the two participating DDs.
            std::copy(other.getContainedMetaVariableNames().begin(), other.getContainedMetaVariableNames().containedMetaVariableNames.end(), std::inserter(this->containedMetaVariableNames, this->containedMetaVariableNames.end()));
            
            return *this;
        }
        
        Dd<CUDD> Dd<CUDD>::operator-(Dd<CUDD> const& other) const {
            Dd<CUDD> result(*this);
            result -= other;
            return result;
        }
        
        Dd<CUDD>& Dd<CUDD>::operator-=(Dd<CUDD> const& other) {
            cuddAdd -= other;
            
            // Join the variable sets of the two participating DDs.
            std::copy(other.getContainedMetaVariableNames().begin(), other.getContainedMetaVariableNames().containedMetaVariableNames.end(), std::inserter(this->containedMetaVariableNames, this->containedMetaVariableNames.end()));
            
            return *this;
        }
        
        Dd<CUDD> Dd<CUDD>::operator/(Dd<CUDD> const& other) const {
            Dd<CUDD> result(*this);
            result /= other;
            return result;
        }
        
        Dd<CUDD>& Dd<CUDD>::operator/=(Dd<CUDD> const& other) {
            cuddAdd.Divide(other);
            
            // Join the variable sets of the two participating DDs.
            std::copy(other.getContainedMetaVariableNames().begin(), other.getContainedMetaVariableNames().containedMetaVariableNames.end(), std::inserter(this->containedMetaVariableNames, this->containedMetaVariableNames.end()));
            
            return *this;
        }
        
        Dd<CUDD> Dd<CUDD>::operator~() const {
            Dd<CUDD> result(*this);
            result.complement();
            return result;
        }
        
        Dd<CUDD>& Dd<CUDD>::complement() {
            cuddAdd = ~cuddAdd;
            return *this;
        }
        
        void Dd<CUDD>::exportToDot(std::string const& filename) const {
            FILE* filePointer = fopen(filename.c_str() , "w");
            this->getDdManager()->getCuddManager().DumpDot({this->cuddAdd}, nullptr, nullptr, filePointer);
            fclose(filePointer);
        }
        
        std::shared_ptr<DdManager<CUDD>> Dd<CUDD>::getDdManager() const {
            return this->ddManager;
        }
    }
}