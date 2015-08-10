#include "src/solver/Smt2SmtSolver.h"

#include "src/settings/SettingsManager.h"
#include "src/exceptions/NotSupportedException.h"
#include "src/exceptions/NotImplementedException.h"
#include "src/exceptions/InvalidStateException.h"
#include "src/exceptions/IllegalArgumentException.h"
#include "src/exceptions/IllegalFunctionCallException.h"
#include "utility/macros.h"
#include "adapters/CarlAdapter.h"
#include "exceptions/UnexpectedException.h"

namespace storm {
    namespace solver {

        Smt2SmtSolver::Smt2ModelReference::Smt2ModelReference(storm::expressions::ExpressionManager const& manager, storm::adapters::Smt2ExpressionAdapter& expressionAdapter) : ModelReference(manager), expressionAdapter(expressionAdapter){
            // Intentionally left empty.
        }

        bool Smt2SmtSolver::Smt2ModelReference::getBooleanValue(storm::expressions::Variable const& variable) const {
            STORM_LOG_THROW(false, storm::exceptions::NotImplementedException, "functionality not (yet) implemented");
        }

        int_fast64_t Smt2SmtSolver::Smt2ModelReference::getIntegerValue(storm::expressions::Variable const& variable) const {
            STORM_LOG_THROW(false, storm::exceptions::NotImplementedException, "functionality not (yet) implemented");
        }

        double Smt2SmtSolver::Smt2ModelReference::getRationalValue(storm::expressions::Variable const& variable) const {
            STORM_LOG_THROW(false, storm::exceptions::NotImplementedException, "functionality not (yet) implemented");
        }

        Smt2SmtSolver::Smt2SmtSolver(storm::expressions::ExpressionManager& manager, bool useCarlExpressions) : SmtSolver(manager), isCommandFileOpen(false), expressionAdapter(nullptr), lastCheckAssumptions(false), lastResult(CheckResult::Unknown), useCarlExpressions(useCarlExpressions) {
#ifndef STORM_HAVE_CARL
            STORM_LOG_THROW(!useCarlExpressions, storm::exceptions::IllegalArgumentException, "Tried to use carl expressions but storm is not linked with CARL");
#endif
#ifndef WINDOWS
            processIdOfSolver=0;
#endif
            this->expressionAdapter = std::unique_ptr<storm::adapters::Smt2ExpressionAdapter>(new storm::adapters::Smt2ExpressionAdapter(this->getManager(), this->useReadableVarNames));
            init();
        }

        Smt2SmtSolver::~Smt2SmtSolver() {
            writeCommand("( exit )", false); //do not wait for success because it does not matter at this point and may cause problems if the solver is not running properly
#ifndef WINDOWS
            if(processIdOfSolver!=0){
                //Since the process has been open successfully, it means that we have to close our fds   
                close(fromSolver);
                close(toSolver);
                //todo make sure that the process exits  waitpid kill(pid, ..);
            }
#endif
        }

        void Smt2SmtSolver::push() {
            expressionAdapter->increaseScope();
            writeCommand("( push 1 ) ", true);
        }

        void Smt2SmtSolver::pop() {
            expressionAdapter->decreaseScope();
            writeCommand("( pop 1 ) ", true);
        }

        void Smt2SmtSolver::pop(uint_fast64_t n) {
            expressionAdapter->decreaseScope(n);
            writeCommand("( pop " + std::to_string(n) + " ) ", true);
        }

        void Smt2SmtSolver::reset() {
            STORM_LOG_THROW(false, storm::exceptions::NotImplementedException, "functionality not (yet) implemented");
        }

        void Smt2SmtSolver::add(storm::expressions::Expression const& assertion) {
            STORM_LOG_THROW(!useCarlExpressions, storm::exceptions::IllegalFunctionCallException, "This solver was initialized without allowing carl expressions");
            STORM_LOG_THROW(false, storm::exceptions::NotImplementedException, "functionality not (yet) implemented");
        }

#ifdef STORM_HAVE_CARL        
        void Smt2SmtSolver::add(storm::RationalFunction const& leftHandSide, storm::CompareRelation const& relation, storm::RationalFunction const& rightHandSide) {
            STORM_LOG_THROW(useCarlExpressions, storm::exceptions::IllegalFunctionCallException, "This solver was initialized without allowing carl expressions");
            //if some of the occurring variables are not declared yet, we will have to.
            std::set<storm::Variable> variables;
            leftHandSide.gatherVariables(variables);
            rightHandSide.gatherVariables(variables);
            std::vector<std::string> const varDeclarations = expressionAdapter->checkForUndeclaredVariables(variables);
            for (auto declaration : varDeclarations){
                writeCommand(declaration, true);
            }
            writeCommand("( assert " + expressionAdapter->translateExpression(leftHandSide, relation, rightHandSide) + " )", true);
        }
        
        void Smt2SmtSolver::add(storm::ArithConstraint<storm::RationalFunction> const& constraint) {
            add(constraint.lhs(), constraint.rel());
        }
        
        void Smt2SmtSolver::add(storm::ArithConstraint<storm::RawPolynomial> const& constraint) {
            //if some of the occurring variables are not declared yet, we will have to.
            std::set<storm::Variable> variables = constraint.lhs().gatherVariables();
            std::vector<std::string> const varDeclarations = expressionAdapter->checkForUndeclaredVariables(variables);
            for (auto declaration : varDeclarations){
                writeCommand(declaration, true);
            }
            writeCommand("( assert " + expressionAdapter->translateExpression(constraint) + " )", true);
        }
        
        void Smt2SmtSolver::add(storm::Variable const& guard, typename storm::ArithConstraint<storm::Polynomial> const& constraint){
            STORM_LOG_THROW((guard.getType()==carl::VariableType::VT_BOOL), storm::exceptions::IllegalArgumentException, "Tried to add a guarded constraint, but the guard is not of type bool.");
            //if some of the occurring variables are not declared yet, we will have to (including the guard!).
            std::set<storm::Variable> variables = constraint.lhs().gatherVariables();
            variables.insert(guard);
            std::vector<std::string> const varDeclarations = expressionAdapter->checkForUndeclaredVariables(variables);
            for (auto declaration : varDeclarations){
                writeCommand(declaration, true);
            }
            std::string guardName= carl::VariablePool::getInstance().getName(guard, this->useReadableVarNames);
            writeCommand("( assert (=> " + guardName + " " + expressionAdapter->translateExpression(constraint) + " ) )", true);
        }
        
        void Smt2SmtSolver::add(const storm::Variable& variable, bool value){
            STORM_LOG_THROW((variable.getType()==carl::VariableType::VT_BOOL), storm::exceptions::IllegalArgumentException, "Tried to add a constraint that consists of a non-boolean variable.");
            std::set<storm::Variable> variableSet;
            variableSet.insert(variable);
            std::vector<std::string> const varDeclarations = expressionAdapter->checkForUndeclaredVariables(variableSet);
            for (auto declaration : varDeclarations){
                writeCommand(declaration, true);
            }
            std::string varName= carl::VariablePool::getInstance().getName(variable, this->useReadableVarNames);
            if(value){
                writeCommand("( assert " + varName + " )", true);
            }
            else{
                writeCommand("( assert (not " + varName + ") )", true);
            }
        }
        
#endif

        SmtSolver::CheckResult Smt2SmtSolver::check() {
            writeCommand("( check-sat )", false);
#ifdef WINDOWS
                STORM_LOG_WARN("SMT-LIBv2 Solver can not be started on Windows as this is not yet implemented. Assume that the check-result is \"unknown\"");
                return SmtSolver::CheckResult::Unknown;
#else
                
            if (processIdOfSolver!=0){
                auto solverOutput = readSolverOutput();
                STORM_LOG_THROW(solverOutput.size()==1, storm::exceptions::UnexpectedException, "expected a single line of output after smt2 command ( check-sat ). Got " + std::to_string(solverOutput.size()) + " lines of output instead.");
                solverOutput[0].erase(std::remove_if(solverOutput[0].begin(), solverOutput[0].end(), ::isspace), solverOutput[0].end()); //remove spaces
                if(solverOutput[0]=="sat") return SmtSolver::CheckResult::Sat;
                if(solverOutput[0]=="unsat") return SmtSolver::CheckResult::Unsat;
                if(solverOutput[0]=="unknown") return SmtSolver::CheckResult::Unknown;
                //if we reach this point, something unexpected happened. Lets return unknown and print some debug output
                STORM_LOG_DEBUG("unexpected solver output: " << solverOutput[0] << ". Returning result 'unknown'");
                return SmtSolver::CheckResult::Unknown;
            }
            else{
                STORM_LOG_WARN("No SMT-LIBv2 Solver Command specified, which means that no actual SMT solving is done... Assume that the result is \"unknown\"");
                return SmtSolver::CheckResult::Unknown;
            }
#endif
        }

        SmtSolver::CheckResult Smt2SmtSolver::checkWithAssumptions(std::set<storm::expressions::Expression> const& assumptions) {
            STORM_LOG_THROW(false, storm::exceptions::NotImplementedException, "functionality not (yet) implemented");
        }

#ifndef WINDOWS

        SmtSolver::CheckResult Smt2SmtSolver::checkWithAssumptions(std::initializer_list<storm::expressions::Expression> const& assumptions) {
            STORM_LOG_THROW(false, storm::exceptions::NotImplementedException, "functionality not (yet) implemented");
        }
#endif

        void Smt2SmtSolver::init() {
            if (storm::settings::smt2SmtSolverSettings().isSolverCommandSet()){
#ifdef WINDOWS
                STORM_LOG_WARN("opening a thread for the smt solver is not implemented on Windows. Hence, no actual solving will be done")
#else
                signal(SIGPIPE, SIG_IGN);
                this->needsRestart=false;
                
                //for executing the solver we will need the path to the executable file as well as the arguments as a char* array
                //where the first argument is the path to the executable file and the last is NULL
                const std::string cmdString = storm::settings::smt2SmtSolverSettings().getSolverCommand();
                std::vector<std::string> solverCommandVec;
                boost::split(solverCommandVec, cmdString, boost::is_any_of("\t "));
                char** solverArgs = new char* [solverCommandVec.size()+1];
                solverArgs[0] = const_cast<char*>(solverCommandVec[0].substr(0, cmdString.rfind('/')+1).c_str());
                for(uint_fast64_t argumentIndex = 1; argumentIndex<solverCommandVec.size(); ++argumentIndex){
                    solverArgs[argumentIndex]=const_cast<char*>(solverCommandVec[argumentIndex].c_str());
                }
                solverArgs[solverCommandVec.size()] = NULL;
                
                //get the pipes started
                int pipeIn[2];
                int pipeOut[2];                
                const int READ=0;
                const int WRITE=1;
                STORM_LOG_THROW(pipe(pipeIn) == 0 && pipe(pipeOut)==0, storm::exceptions::UnexpectedException, "Could not open pipe to new process");
                
                //now start the child process, i.e., the solver
                pid_t pid = fork();
                STORM_LOG_THROW(pid>=0, storm::exceptions::UnexpectedException, "Could not start new process for the smt solver");
                if (pid == 0){
                    // Child process
                    // duplicate the fd so that standard input and output will be send to our pipes
                    dup2(pipeIn[READ], STDIN_FILENO); 
                    dup2(pipeOut[WRITE], STDOUT_FILENO);
                    dup2(pipeOut[WRITE], STDERR_FILENO);
                    // we can now close everything since our child process will use std in and out to address the pipes
                    close(pipeIn[READ]);
                    close(pipeIn[WRITE]); 
                    close(pipeOut[READ]); 
                    close(pipeOut[WRITE]); 
                    
                    execv(solverCommandVec[0].c_str(), solverArgs); //"-smt2 -in"
                    // if we reach this point, execl was not successful
                    STORM_LOG_THROW(false, storm::exceptions::UnexpectedException, "Could not execute the solver correctly");
                }
                // Parent Process
                toSolver=pipeIn[WRITE];
                fromSolver=pipeOut[READ];   
                close(pipeOut[WRITE]);
                close(pipeIn[READ]);
                processIdOfSolver=pid;
                
                
#endif  
            }
            else{
                STORM_LOG_WARN("No SMT-LIBv2 Solver Command specified, which means that no actual SMT solving can be done");
            }

            if (storm::settings::smt2SmtSolverSettings().isExportSmtLibScriptSet()){
                STORM_LOG_DEBUG("The SMT-LIBv2 commands are exportet to the given file");
                commandFile.open(storm::settings::smt2SmtSolverSettings().getExportSmtLibScriptPath(), std::ios::trunc);
                isCommandFileOpen=commandFile.is_open();
                STORM_LOG_THROW(isCommandFileOpen, storm::exceptions::InvalidArgumentException, "The file where the smt2commands should be written to could not be opened");
            }

            //some initial commands
            writeCommand("( set-option :print-success true )", true);
            writeCommand("( set-logic QF_NRA )", true);
            //writeCommand("( get-info :name )");
        }

        bool Smt2SmtSolver::isNeedsRestart() const {
            return this->needsRestart;
        }

        void Smt2SmtSolver::writeCommand(std::string smt2Command, bool expectSuccess) {
            if (isCommandFileOpen) {
                commandFile << smt2Command << std::endl;
            }
            
#ifndef WINDOWS
            if (processIdOfSolver!=0){
                if(write(toSolver, (smt2Command+"\n").c_str(), smt2Command.length()+1) < 0){
                    STORM_LOG_DEBUG("Was not able to write " << smt2Command << "to the solver.");
                }
                if(expectSuccess){
                    auto output=readSolverOutput();
                    STORM_LOG_THROW(output.size()==1, storm::exceptions::UnexpectedException, "expected a single success response after smt2 command " + smt2Command + ". Got " + std::to_string(output.size()) + " lines of output instead.");
                    output[0].erase(std::remove_if(output[0].begin(), output[0].end(), ::isspace), output[0].end());
                    STORM_LOG_THROW(output[0]=="success", storm::exceptions::UnexpectedException, "expected <<success>> response after smt2 command " + smt2Command + ". Got <<" + output[0] + ">> instead");
                }
            }
#endif
        }
        
        std::vector<std::string> Smt2SmtSolver::readSolverOutput(bool waitForOutput){
#ifndef WINDOWS
            if (processIdOfSolver==0){
                STORM_LOG_DEBUG("failed to read solver output as the solver is not running");
                return std::vector<std::string>();
            }
            int bytesReadable;
            if(waitForOutput){
                bytesReadable=1;//just assume that there are bytes readable
            }
            else if(ioctl(fromSolver, FIONREAD, &bytesReadable)< 0){ //actually obtain the readable bytes
                STORM_LOG_ERROR("Could not check if the solver has output");
                return std::vector<std::string>();
            }
            std::string solverOutput="";
            const ssize_t MAX_CHUNK_SIZE=256;
            char chunk[MAX_CHUNK_SIZE];
            while(bytesReadable>0){
                ssize_t chunkSize = read(fromSolver, chunk, MAX_CHUNK_SIZE);
                STORM_LOG_THROW(chunkSize>=0, storm::exceptions::UnexpectedException, "failed to read solver output");
                solverOutput += std::string(chunk, chunkSize);
                if(ioctl(fromSolver, FIONREAD, &bytesReadable)< 0){//obtain the new amount of readable bytes
                    STORM_LOG_ERROR("Could not check if the solver has output");
                    return std::vector<std::string>();
                }
                if(bytesReadable==0 && solverOutput.back()!='\n'){
                    STORM_LOG_DEBUG("Solver Output '" << solverOutput << "' did not end with newline symbol (\\n). Since we assume that this should be the case, we will wait for more output from the solver");
                    bytesReadable=1; //we expect more output!
                }
            }
            checkForErrorMessage(solverOutput);
            std::vector<std::string> solverOutputAsVector;
            if(solverOutput.length()>0){
                solverOutput=solverOutput.substr(0,solverOutput.length()-1);
                boost::split(solverOutputAsVector, solverOutput, boost::is_any_of("\n"));
                //note: this is a little bit unsafe as \n can be contained within a solver response
            }
            return solverOutputAsVector;
#endif
        }
        
        void Smt2SmtSolver::checkForErrorMessage(const std::string message){
            size_t errorOccurrance = message.find("error");
            if (errorOccurrance!=std::string::npos){
                this->needsRestart=true;
                //do not throw an exception for timeout or memout errors
                if(message.find("timeout")!=std::string::npos){
                    STORM_LOG_INFO("SMT solver answered: '" << message << "' and I am interpreting this as timeout ");
                }
                else if(message.find("memory")!=std::string::npos){
                    STORM_LOG_INFO("SMT solver answered: '" << message << "' and I am interpreting this as out of memory ");
                    this->processIdOfSolver=0;
                }
                else{
                    std::string errorMsg = "An error was detected while checking the solver output. ";
                    STORM_LOG_DEBUG("Detected an error message in the solver response:\n" + message);
                    size_t firstQuoteSign = message.find('\"',errorOccurrance);
                    if(firstQuoteSign!=std::string::npos && message.find("\\\"", firstQuoteSign-1)!=firstQuoteSign-1){
                        size_t secondQuoteSign = message.find('\"', firstQuoteSign+1);
                        while(secondQuoteSign!=std::string::npos && message.find("\\\"", secondQuoteSign-1)==secondQuoteSign-1){
                            secondQuoteSign = message.find('\"',secondQuoteSign+1);
                        }
                        if(secondQuoteSign!=std::string::npos){
                            errorMsg += "The error message was: <<" + message.substr(errorOccurrance,secondQuoteSign-errorOccurrance+1) + ">>.";
                            STORM_LOG_THROW(false, storm::exceptions::UnexpectedException, errorMsg);
                            return;
                        }
                    }
                    errorMsg += "The error message could not be parsed correctly. Snippet:\n" + message.substr(errorOccurrance,200);
                    STORM_LOG_THROW(false, storm::exceptions::UnexpectedException, errorMsg);
                }
            }
        }
    }
}