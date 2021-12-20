#ifndef STORM_SOLVER_SMTRATSMTSOLVER
#define STORM_SOLVER_SMTRATSMTSOLVER
#include "storm-config.h"
#include "storm/solver/SmtSolver.h"

#ifdef STORM_HAVE_SMTRAT
#ifdef SMTRATDOESNTWORK  // Does not compile with current version of smtrat.

#include "../adapters/RationalFunctionAdapter.h"
#include "lib/smtrat.h"

namespace storm {
namespace solver {
class SmtratSmtSolver : public SmtSolver {
   private:
    smtrat::RatOne* solver;
    unsigned exitCode;

   public:
    SmtratSmtSolver(storm::expressions::ExpressionManager& manager);
    virtual ~SmtratSmtSolver();

    virtual void push() override;

    virtual void pop() override;

    virtual void pop(uint_fast64_t n) override;

    virtual CheckResult check() override;

    void add(storm::RawPolynomial const&, storm::CompareRelation);

    template<typename ReturnType>
    ReturnType getModel() const;

    std::vector<smtrat::FormulasT> const& getUnsatisfiableCores() const;

    // The last result that was returned by any of the check methods.
    CheckResult lastResult;
};
}  // namespace solver
}  // namespace storm
#endif
#endif

#endif  // STORM_SOLVER_SMTRATSMTSOLVER
