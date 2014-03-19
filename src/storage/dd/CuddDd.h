#ifndef STORM_STORAGE_DD_CUDDDD_H_
#define STORM_STORAGE_DD_CUDDDD_H_

#include "src/storage/dd/Dd.h"
#include "src/storage/dd/CuddDdManager.h"

// Include the C++-interface of CUDD.
#include "cuddObj.hh"

namespace storm {
    namespace dd {
        
        template<>
        class Dd<CUDD> {
            /*!
             * Creates a DD that encapsulates the given CUDD ADD.
             *
             * @param cuddAdd The CUDD ADD to store.
             */
            Dd(ADD cuddAdd);
            
            // Instantiate all copy/move constructors/assignments with the default implementation.
            Dd(Dd<CUDD> const& other) = default;
            Dd(Dd<CUDD>&& other) = default;
            Dd& operator=(Dd<CUDD> const& other) = default;
            Dd& operator=(Dd<CUDD>&& other) = default;
            
            /*!
             * Adds the two DDs.
             *
             * @param other The DD to add to the current one.
             * @return The result of the addition.
             */
            Dd<CUDD> operator+(Dd<CUDD> const& other) const;

            /*!
             * Adds the given DD to the current one.
             *
             * @param other The DD to add to the current one.
             * @return A reference to the current DD after the operation.
             */
            Dd<CUDD>& operator+=(Dd<CUDD> const& other);
            
            /*!
             * Multiplies the two DDs.
             *
             * @param other The DD to multiply with the current one.
             * @return The result of the multiplication.
             */
            Dd<CUDD> operator*(Dd<CUDD> const& other) const;

            /*!
             * Multiplies the given DD with the current one and assigns the result to the current DD.
             *
             * @param other The DD to multiply with the current one.
             * @return A reference to the current DD after the operation.
             */
            Dd<CUDD>& operator*=(Dd<CUDD> const& other);
            
            /*!
             * Subtracts the given DD from the current one.
             *
             * @param other The DD to subtract from the current one.
             * @return The result of the subtraction.
             */
            Dd<CUDD> operator-(Dd<CUDD> const& other) const;
            
            /*!
             * Subtracts the given DD from the current one and assigns the result to the current DD.
             *
             * @param other The DD to subtract from the current one.
             * @return A reference to the current DD after the operation.
             */
            Dd<CUDD>& operator-=(Dd<CUDD> const& other);
            
            /*!
             * Divides the current DD by the given one.
             *
             * @param other The DD by which to divide the current one.
             * @return The result of the division.
             */
            Dd<CUDD> operator/(Dd<CUDD> const& other) const;
            
            /*!
             * Divides the current DD by the given one and assigns the result to the current DD.
             *
             * @param other The DD by which to divide the current one.
             * @return A reference to the current DD after the operation.
             */
            Dd<CUDD>& operator/=(Dd<CUDD> const& other);
            
            /*!
             * Retrieves the logical complement of the current DD. The result will map all encodings with a value
             * unequal to zero to false and all others to true.
             *
             * @return The logical complement of the current DD.
             */
            Dd<CUDD> operator~() const;
            
            /*!
             * Logically complements the current DD. The result will map all encodings with a value
             * unequal to zero to false and all others to true.
             */
            void complement();
            
            /*!
             * Retrieves the manager that is responsible for this DD.
             *
             * A pointer to the manager that is responsible for this DD.
             */
            std::shared_ptr<DdManager<CUDD>> getDdManager() const;
            
        private:
            // A pointer to the manager responsible for this DD.
            std::shared_ptr<DdManager<CUDD>> ddManager;

            // The ADD created by CUDD.
            ADD cuddAdd;
            
            // The names of all meta variables that appear in this DD.
            std::unordered_set<std::string> containedMetaVariableNames;
        };
    }
}

#endif /* STORM_STORAGE_DD_CUDDDD_H_ */