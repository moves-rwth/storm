#ifndef STORM_STORAGE_EXPRESSIONS_EXPRESSIONRETURNTYPE_H_
#define STORM_STORAGE_EXPRESSIONS_EXPRESSIONRETURNTYPE_H_

#include <iostream>
#include <memory>
#include <cstdint>

#include "src/storage/expressions/OperatorType.h"

namespace storm {
    namespace expressions {
        // Forward-declare expression manager class.
        class ExpressionManager;
        
        class BaseType {
        public:
            /*!
             * Retrieves the mask that is associated with this type.
             *
             * @return The mask associated with this type.
             */
            virtual uint64_t getMask() const = 0;
            
            /*!
             * Checks whether two types are actually the same.
             *
             * @param other The type to compare with.
             * @return True iff the types are the same.
             */
            virtual bool operator==(BaseType const& other) const;
            
            /*!
             * Returns a string representation of the type.
             *
             * @return A string representation of the type.
             */
            virtual std::string getStringRepresentation() const = 0;
        };

        class BooleanType : public BaseType {
        public:
            virtual uint64_t getMask() const override;
            virtual std::string getStringRepresentation() const override;

        private:
            static const uint64_t mask = (1 << 61);
        };
        
        class IntegerType : public BaseType {
        public:
            virtual uint64_t getMask() const override;
            virtual std::string getStringRepresentation() const override;
            
        private:
            static const uint64_t mask = (1 << 62);
        };
        
        class BoundedIntegerType : public BaseType {
        public:
            /*!
             * Creates a new bounded integer type with the given bit width.
             *
             * @param width The bit width of the type.
             */
            BoundedIntegerType(std::size_t width);
            
            /*!
             * Retrieves the bit width of the bounded type.
             *
             * @return The bit width of the bounded type.
             */
            std::size_t getWidth() const;

            virtual uint64_t getMask() const override;

            virtual bool operator==(BaseType const& other) const override;

            virtual std::string getStringRepresentation() const override;

        private:
            static const uint64_t mask = (1 << 61) | (1 << 62);
            
            // The bit width of the type.
            std::size_t width;
        };

        class RationalType : public BaseType {
        public:
            virtual uint64_t getMask() const override;
            
            virtual std::string getStringRepresentation() const override;

        private:
            static const uint64_t mask = (1 << 63);
        };
        
        class ErrorType : public BaseType {
        public:
            virtual uint64_t getMask() const override;
            
            virtual std::string getStringRepresentation() const override;
            
        private:
            static const uint64_t mask = 0;
        };
        
        class Type {
        public:
            Type(ExpressionManager const& manager, std::shared_ptr<BaseType> innerType);

            /*!
             * Checks whether two types are the same.
             *
             * @other The type to compare with.
             * @return True iff the types are the same.
             */
            bool operator==(Type const& other) const;
            
            /*!
             * Retrieves the bit mask of the type.
             *
             * @return The bit mask of the type.
             */
            uint64_t getMask() const;
            
            /*!
             * Retrieves a string representation of the type.
             *
             * @return A string representation of the type.
             */
            std::string getStringRepresentation() const;

            /*!
             * Checks whether this type is a numerical type.
             *
             * @return True iff the type is a numerical one.
             */
            bool isNumericalType() const;

            /*!
             * Checks whether this type is an integral type.
             *
             * @return True iff the type is a integral one.
             */
            bool isIntegralType() const;

            /*!
             * Checks whether this type is a boolean type.
             *
             * @return True iff the type is a boolean one.
             */
            bool isBooleanType() const;

            /*!
             * Checks whether this type is an unbounded integral type.
             *
             * @return True iff the type is a unbounded integral one.
             */
            bool isUnboundedIntegralType() const;
            
            /*!
             * Checks whether this type is a bounded integral type.
             *
             * @return True iff the type is a bounded integral one.
             */
            bool isBoundedIntegralType() const;
            
            /*!
             * Retrieves the bit width of the type, provided that it is a bounded integral type.
             *
             * @return The bit width of the bounded integral type.
             */
            std::size_t getWidth() const;

            /*!
             * Checks whether this type is a rational type.
             *
             * @return True iff the type is a rational one.
             */
            bool isRationalType() const;
            
            // Functions that, given the input types, produce the output type of the corresponding function application.
            Type plusMinusTimes(Type const& other) const;
            Type minus() const;
            Type divide(Type const& other) const;
            Type logicalConnective(Type const& other) const;
            Type logicalConnective() const;
            Type numericalComparison(Type const& other) const;
            Type ite(Type const& thenType, Type const& elseType) const;
            Type floorCeil() const;
            Type minimumMaximum(Type const& other) const;
            
        private:
            // The manager responsible for the type.
            ExpressionManager const& manager;
            
            // The encapsulated type.
            std::shared_ptr<BaseType> innerType;
        };
        
        std::ostream& operator<<(std::ostream& stream, Type const& type);
        
    }
}

namespace std {
    // Provide a hashing operator, so we can put types in unordered collections.
    template <>
    struct hash<storm::expressions::Type> {
        std::size_t operator()(storm::expressions::Type const& type) const {
            return std::hash<uint64_t>()(type.getMask());
        }
    };
    
    // Provide a less operator, so we can put types in ordered collections.
    template <>
    struct less<storm::expressions::Type> {
        std::size_t operator()(storm::expressions::Type const& type1, storm::expressions::Type const& type2) const {
            return type1.getMask() < type2.getMask();
        }
    };
}

#endif /* STORM_STORAGE_EXPRESSIONS_EXPRESSIONRETURNTYPE_H_ */