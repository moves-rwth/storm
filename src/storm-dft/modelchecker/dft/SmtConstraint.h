#include <string>
#include <storm/storage/expressions/Expression.h>

namespace storm {
    namespace modelchecker {
        class SmtConstraint {
        public:
            virtual ~SmtConstraint() {
            }

            virtual std::string toSmtlib2(std::vector<std::string> const &varNames) const = 0;

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
    }
}