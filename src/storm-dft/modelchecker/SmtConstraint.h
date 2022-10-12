#pragma once

#include <storm/storage/expressions/Expression.h>
#include <string>

namespace storm::dft {
namespace modelchecker {

class SmtConstraint {
   public:
    virtual ~SmtConstraint() {}

    /** Generate a string describing the constraint in Smtlib2 format
     *
     * @param varNames vector of variable names
     * @return Smtlib2 format string
     */
    virtual std::string toSmtlib2(std::vector<std::string> const &varNames) const = 0;

    /** Generate an expression describing the constraint in Storm format
     *
     * @param varNames vector of variable names
     * @param manager the expression manager used to handle the expressions
     * @return the expression
     */
    virtual storm::expressions::Expression toExpression(std::vector<std::string> const &varNames,
                                                        std::shared_ptr<storm::expressions::ExpressionManager> manager) const = 0;

    virtual std::string description() const {
        return descript;
    }

    void setDescription(std::string const &descr) {
        descript = descr;
    }

   private:
    std::string descript;
};

}  // namespace modelchecker
}  // namespace storm::dft