#ifndef STORM_SOLVER_SMT2SMTSOLVER
#define STORM_SOLVER_SMT2SMTSOLVER

#include <fstream>
#include <iostream>

#include "storm-config.h"
#include "storm/adapters/RationalFunctionAdapter.h"
#include "storm/adapters/Smt2ExpressionAdapter.h"
#include "storm/solver/SmtSolver.h"

namespace storm {
namespace solver {
/*!
 * This class represents an SMT-LIBv2 conforming solver.
 * Any SMT-LIBv2 conforming solver can be called and will be opened as a child process.
 * It is also possible to export the SMT2 script for later use.
 * @note The parsing of the solver responses is a little bit crude and might cause bugs (e.g., if a variable name has the infix "error")
 */
class SmtlibSmtSolver : public SmtSolver {
   public:
    class SmtlibModelReference : public SmtSolver::ModelReference {
       public:
        SmtlibModelReference(storm::expressions::ExpressionManager const& manager);
        virtual bool getBooleanValue(storm::expressions::Variable const& variable) const override;
        virtual int_fast64_t getIntegerValue(storm::expressions::Variable const& variable) const override;
        virtual double getRationalValue(storm::expressions::Variable const& variable) const override;
        virtual std::string toString() const override;
    };

   public:
    /*!
     * Creates a new solver with the given manager.
     * In addition to storm expressions, this solver also allows carl expressions (but not both to not confuse variables).
     * Hence, there is a flag to chose between the two
     */
    SmtlibSmtSolver(storm::expressions::ExpressionManager& manager, bool useCarlExpressions = false);
    virtual ~SmtlibSmtSolver();

    virtual void push() override;

    virtual void pop() override;

    virtual void pop(uint_fast64_t n) override;

    virtual void reset() override;

    virtual void add(storm::expressions::Expression const& assertion) override;
#ifdef STORM_HAVE_CARL
    // adds the constraint "leftHandSide relation rightHandSide"
    virtual void add(storm::RationalFunction const& leftHandSide, storm::CompareRelation const& relation,
                     storm::RationalFunction const& rightHandSide = storm::RationalFunction(0));

    // asserts that the given variable has the given value. The variable should have type 'bool'
    void add(storm::RationalFunctionVariable const& variable, bool value);
#endif

    virtual CheckResult check() override;

    virtual CheckResult checkWithAssumptions(std::set<storm::expressions::Expression> const& assumptions) override;

    virtual CheckResult checkWithAssumptions(std::initializer_list<storm::expressions::Expression> const& assumptions) override;

    bool isNeedsRestart() const;

    // Todo: some of these might be added in the future
    // virtual storm::expressions::SimpleValuation getModelAsValuation() override;

    // virtual std::shared_ptr<SmtSolver::ModelReference> getModel() override;

    //	virtual std::vector<storm::expressions::SimpleValuation> allSat(std::vector<storm::expressions::Variable> const& important) override;

    //	virtual uint_fast64_t allSat(std::vector<storm::expressions::Variable> const& important, std::function<bool(storm::expressions::SimpleValuation&)>
    // const& callback) override;

    //	virtual uint_fast64_t allSat(std::vector<storm::expressions::Variable> const& important, std::function<bool(ModelReference&)> const& callback) override;

    //	virtual std::vector<storm::expressions::Expression> getUnsatAssumptions() override;

   private:
    /*!
     * Initializes the solver, i.e. opens a new process for it and creates a file stream for the script file  (if demanded)
     * Moreover, some initial commands are send to the solver
     */
    void init();

    /*!
     * Writes the given command to the solver
     * @param smt2Command the command that the solver will receive
     * @param expectSuccess set this flag to true whenever a success response from the solver is expected.
     */
    void writeCommand(std::string smt2Command, bool expectSuccess);

    /*!
     * Reads from the solver. The output is checked for an error message and an exception is thrown in that case.
     * @param waitForOutput if this is true and there is currently no output, we will wait until there is output.
     * @return the output of the solver. Every entry of the vector corresponds to one output line
     */
    std::vector<std::string> readSolverOutput(bool waitForOutput = true);

    /*!
     * Checks if the given message contains an error message and throws an exception.
     * More precisely, an exception is thrown whenever the word "error" is contained in the message.
     * This function is directly called when reading the solver output via readSolverOutput()
     * We will try to parse the message in the SMT-LIBv2 format, i.e.,
     * ( error "this is the error message from the solver" ) to give some debug information
     * However, the whole message is always written to the debug log (providing there is an error)
     * @param message the considered message which should be an output of the solver.
     */
    void checkForErrorMessage(const std::string message);

#ifndef WINDOWS
    // descriptors for the pipe from and to the solver
    int toSolver;
    int fromSolver;
    // A flag storing the Process ID of the solver. If this is zero, then the solver is not running
    pid_t processIdOfSolver;
#endif

    // a filestream where the commands that we send to the solver will be stored (can be used for debugging purposes)
    std::ofstream commandFile;

    bool isCommandFileOpen;

    // An expression adapter that is used for translating the expression into Smt2's format.
    std::unique_ptr<storm::adapters::Smt2ExpressionAdapter> expressionAdapter;

    // A flag storing whether the last call to a check method provided aussumptions.
    // bool lastCheckAssumptions;

    // The last result that was returned by any of the check methods.
    // CheckResult lastResult;

    // A flag that states whether we want to use carl expressions.
    bool useCarlExpressions;

    // A flag that states whether to use readable variable names
    bool useReadableVarNames = true;

    // A flag that states whether some error has occured
    bool needsRestart = false;
};
}  // namespace solver
}  // namespace storm
#endif  // STORM_SOLVER_SMT2SMTSOLVER
