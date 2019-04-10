#include <string>

namespace storm {
    namespace modelchecker {
        class SmtConstraint {
        public:
            virtual ~SmtConstraint() {
            }

            virtual std::string toSmtlib2(std::vector<std::string> const &varNames) const = 0;

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