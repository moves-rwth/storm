#include "storm/storage/jani/expressions/ValueArrayExpression.h"

#include "storm/storage/jani/visitor/JaniExpressionVisitor.h"
#include "storm/storage/expressions/ExpressionManager.h"

#include "storm/exceptions/InvalidArgumentException.h"
#include "storm/exceptions/NotImplementedException.h"
#include "storm/exceptions/UnexpectedException.h"

namespace storm {
    namespace expressions {
        
        ValueArrayExpression::ValueArrayExpression(ExpressionManager const& manager, Type const& type) : ArrayExpression(manager, type), elements(ValueArrayElements()) {
            // Intentionally left empty
        }

        ValueArrayExpression::ValueArrayExpression(ExpressionManager const& manager, Type const& type, ValueArrayElements elements) : ArrayExpression(manager, type), elements(std::move(elements)) {
            // Intentionally left empty
        }

        void ValueArrayExpression::gatherVariables(std::set<storm::expressions::Variable>& variables) const {
            this->gatherVariables(variables, elements);
        }

        void ValueArrayExpression::gatherVariables(std::set<storm::expressions::Variable>& variables, ValueArrayElements const& elementsToCheck) const {
            if (elementsToCheck.elementsWithValue) {
                for (std::shared_ptr<BaseExpression const> e : elementsToCheck.elementsWithValue.get()) {
                    e->gatherVariables(variables);
                }
            } else {
                for (std::shared_ptr<ValueArrayElements const> e : elementsToCheck.elementsOfElements.get()) {
                    gatherVariables(variables, *e);
                }
            }
        }
        
        bool ValueArrayExpression::containsVariables() const {
            return this->containsVariables(elements);
        }

        bool ValueArrayExpression::containsVariables(ValueArrayElements const& elementsToCheck) const {
            if (elementsToCheck.elementsWithValue) {
                for (auto const& e : elementsToCheck.elementsWithValue.get()) {
                    if (e->containsVariables()) {
                        return true;
                    }
                }
            } else {
                for (std::shared_ptr<ValueArrayElements const> e : elementsToCheck.elementsOfElements.get()) {
                    if (containsVariables(*e)) {
                        return true;
                    }
                }
            }
            return false;
        }

        std::shared_ptr<BaseExpression const> ValueArrayExpression::simplify() const {
            return std::shared_ptr<BaseExpression const>(new ValueArrayExpression(getManager(), getType(), simplify(elements)));
        }

        ValueArrayExpression::ValueArrayElements ValueArrayExpression::simplify(const ValueArrayElements &element) const {
            ValueArrayElements result;
            if (element.elementsWithValue) {
                result.elementsWithValue = std::vector<std::shared_ptr<BaseExpression const>>();
                auto const& elementsWithValue = element.elementsWithValue.get();
                for (auto const& e : elementsWithValue) {
                    result.elementsWithValue->push_back(e->simplify());
                }
            } else {
                std::vector<std::shared_ptr<ValueArrayElements const>> simplifiedElements;
                for (auto const& e : element.elementsOfElements.get()) {
                    simplifiedElements.push_back(std::make_shared<ValueArrayElements const>(simplify(*e)));
                }
                result.elementsOfElements = std::move(simplifiedElements);
            }
            return result;
        }
        
        boost::any ValueArrayExpression::accept(ExpressionVisitor& visitor, boost::any const& data) const {
            auto janiVisitor = dynamic_cast<JaniExpressionVisitor*>(&visitor);
            STORM_LOG_THROW(janiVisitor != nullptr, storm::exceptions::UnexpectedException, "Visitor of jani expression should be of type JaniVisitor.");
            return janiVisitor->visit(*this, data);
        }
        
        void ValueArrayExpression::printToStream(std::ostream& stream) const {
            printToStream(stream, elements);
        }

        void ValueArrayExpression::printToStream(std::ostream& stream, ValueArrayElements const& element) const {
            if (element.elementsWithValue) {
                stream << "array[ ";
                bool first = true;

                for (auto const& e : element.elementsWithValue.get()) {
                    stream << *e;
                    if (!first) {
                        stream <<  " , ";
                    }
                    first = false;
                }
                stream <<  "]";
            } else {
                stream <<  "array[ ";
                bool first = true;

                for (auto const& e : element.elementsOfElements.get()) {
                        printToStream(stream, *e);
                        if (!first) {
                            stream <<  " , ";
                        }
                        first = false;
                }
                stream <<  "]";
            }

        }
        
        std::shared_ptr<BaseExpression const> ValueArrayExpression::size() const {
            return getManager().integer(size(elements)).getBaseExpressionPointer();
        }

        std::vector<size_t> ValueArrayExpression::getSizes() const {
            return elements.getSizes();
        }

        const ValueArrayExpression::ValueArrayElements & ValueArrayExpression::getElements() const {
            return elements;
        }

        std::vector<size_t> ValueArrayExpression::ValueArrayElements::getSizes() const {
            std::vector<size_t> result;
            if (elementsOfElements) {
                result.push_back(elementsOfElements->size());
                auto element = elementsOfElements->at(0);
                while (element->elementsOfElements) {
                    result.push_back(element->elementsOfElements->size());
                    element = element->elementsOfElements->at(0);
                }
                result.push_back(element->elementsWithValue->size());
            } else {
                result.push_back(elementsWithValue->size());
            }

            return result;
        }

        size_t ValueArrayExpression::size(ValueArrayElements const& elementsToCheck) const {
            if (elementsToCheck.elementsWithValue) {
                return elementsToCheck.elementsWithValue->size();
            } else {
                return size(*elementsToCheck.elementsOfElements->at(0)) * elementsToCheck.elementsOfElements->size();
            }
        }
        
        std::shared_ptr<BaseExpression const> ValueArrayExpression::at(std::vector<uint64_t>& index) const {
            ValueArrayElements result = elements;
            for (auto i = 0; i < index.size() - 1; ++i) {
                assert (!elements.elementsWithValue);
                if (elements.elementsOfElements) {
                    STORM_LOG_THROW(index[i] < elements.elementsOfElements->size(), storm::exceptions::InvalidArgumentException, "Tried to access the element with index " << i << " of an array of size " << elements.elementsOfElements->size() << ".");

                } else {
                    STORM_LOG_THROW(index[i] < elements.elementsWithValue->size(), storm::exceptions::InvalidArgumentException, "Tried to access the element with index " << i << " of an array of size " << elements.elementsWithValue->size() << ".");
                }
                result = *elements.elementsOfElements->at(index[i]);
            }
            assert (result.elementsWithValue);
            return result.elementsWithValue->at(index[index.size() -1]);
        }

        std::shared_ptr<BaseExpression const> ValueArrayExpression::at(uint64_t index) const {
            if (elements.elementsWithValue) {
                return elements.elementsWithValue->at(index);
            } else {
                STORM_LOG_THROW(elements.elementsOfElements->at(0)->elementsWithValue, storm::exceptions::NotImplementedException, "Getting the element at index " << index << " is not yet implemented for more than twice nested arrays");
                auto size = elements.elementsOfElements->at(0)->elementsWithValue->size();
                uint64_t indexLastPart = index % size;
                uint64_t indexFirstPart = (index - indexLastPart) / size;
                return elements.elementsOfElements->at(indexFirstPart)->elementsWithValue->at(indexLastPart);

            }

        }







    }
}