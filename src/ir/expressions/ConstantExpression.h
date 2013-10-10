/*
 * ConstantExpression.h
 *
 *  Created on: 03.01.2013
 *      Author: Christian Dehnert
 */

#ifndef STORM_IR_EXPRESSIONS_CONSTANTEXPRESSION_H_
#define STORM_IR_EXPRESSIONS_CONSTANTEXPRESSION_H_

#include "BaseExpression.h"

namespace storm {
    namespace ir {
        namespace expressions {
            
            // A struct for storing whether the constant was defined and if so, what value it holds.
            template<class T>
            struct ConstantDefinitionStruct {
                /*!
                 * Constructs a structure indicating that the constant has been defined with the given value.
                 *
                 * @param value The value with which the constant was defined.
                 */
                ConstantDefinitionStruct(T value) : value(value), defined(true) {
                   // Nothing to do here. 
                }
                
                /*!
                 * Constructs a structure indicating that the constant has not yet been defined.
                 *
                 * @param value The value with which the constant was defined.
                 */
                ConstantDefinitionStruct() : value(), defined(false) {
                    // Nothing to do here. 
                }
                
                T value;
                bool defined;
            };
            
            /*!
             * A class representing a generic constant expression.
             */
            template<class T>
            class ConstantExpression : public BaseExpression {
            public:
                /*!
                 * Constructs a constant expression of the given type with the given constant name.
                 *
                 * @param type The type of the constant.
                 * @param constantName The name of the constant.
                 */
                ConstantExpression(ReturnType type, std::string const& constantName) : BaseExpression(type), constantName(constantName), valueStructPointer(new ConstantDefinitionStruct<T>()) {
                    // Nothing to do here.
                }
                
                /*!
                 * Copy-constructs from the given expression.
                 *
                 * @param constantExpression The expression to copy.
                 */
                ConstantExpression(ConstantExpression const& constantExpression) : BaseExpression(constantExpression), constantName(constantExpression.constantName), valueStructPointer(constantExpression.valueStructPointer) {
                    // Nothing to do here.
                }
                
                /*!
                 * Retrieves the name of the constant.
                 *
                 * @return The name of the constant.
                 */
                std::string const& getConstantName() const {
                    return constantName;
                }
                
                virtual std::string toString() const override {
                    std::stringstream result;
                    if (this->valueStructPointer->defined) {
                        result << this->valueStructPointer->value;
                    } else {
                        result << this->getConstantName();
                    }
                    return result.str();
                }
                
                /*!
                 * Retrieves whether the constant is defined or not.
                 *
                 * @return True if the constant is defined.
                 */
                bool isDefined() const {
                    return this->valueStructPointer->defined;
                }
                
                /*!
                 * Retrieves the value of the constant if it is defined.
                 */
                T getValue() const {
                    return this->valueStructPointer->value;
                }
                
                /*!
                 * Defines the constant using the given value.
                 *
                 * @param value The value to use for defining the constant.
                 */
                void define(T value) {
                    this->valueStructPointer->defined = true;
                    this->valueStructPointer->value = value;
                }
                
                /*!
                 * Undefines the value that was previously set for this constant (if any).
                 */
                void undefine() {
                    this->valueStructPointer->defined = false;
                    this->valueStructPointer->value = T();
                }
                
            private:
                // The name of the constant.
                std::string constantName;
                
                // The definedness status and (if applicable) the value of the constant.
                std::shared_ptr<ConstantDefinitionStruct<T>> valueStructPointer;
            };
            
        } // namespace expressions
    } // namespace ir
} // namespace storm

#endif /* STORM_IR_EXPRESSIONS_CONSTANTEXPRESSION_H_ */
