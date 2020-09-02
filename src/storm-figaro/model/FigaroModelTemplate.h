#pragma once

#include <array>
#include <map>
#include <vector>
#include <sstream>
#include <math.h>
#include <set>
#include "storm/storage/expressions/ExpressionManager.h"
#include "storm/generator/VariableInformation.h"
namespace storm {
    namespace figaro {

        class FigaroProgram {

        protected:
            //constructor to initialize all variables through derived class
            FigaroProgram(
                    std::map<std::string, size_t> mFigaroboolelementindex,
<<<<<<< HEAD
            std::map<std::string, size_t> mFigarofailureelementindex,
            std::map<std::string, size_t> mFigarofloatelementindex,
            std::map<std::string, size_t> mFigarointelementindex,
            std::map<std::string, size_t> mFigaroenumelementindex ,
            std::set<std::string> failure_variable_names ,
            std::set<std::string> enum_variables_names ,
            std::set<std::string> float_variables_names,
            std::string topevent,
             int  numBoolState,
             int numBoolFailureState,
             int  numFloatState,
             int  numIntState,
             int  numEnumState,
             bool ins_transition_found
                    ):
            mFigaroboolelementindex(mFigaroboolelementindex),
            mFigarofailureelementindex(mFigarofailureelementindex),
=======
                    std::map<std::string, size_t> mFigaroelementfailureindex,
                    std::map<std::string, size_t> mFigarofloatelementindex,
                    std::map<std::string, size_t> mFigarointelementindex,
                    std::map<std::string, size_t> mFigaroenumelementindex ,
                    std::set<std::string> enum_variables_names ,
                    std::set<std::string> float_variables_names,
                    std::string topevent,
                    int numBoolState,
                    int numFloatState,
                    int numIntState,
                    int numEnumState,
                    bool ins_transition_found
                ):  mFigaroboolelementindex(mFigaroboolelementindex),
                    mFigaroelementfailureindex(mFigaroelementfailureindex),
>>>>>>> 2dddc29ea061c2a0b33493dccd8ece2b3a1211bf
                    mFigarofloatelementindex(mFigarofloatelementindex),
                    mFigarointelementindex(mFigarointelementindex),
                    mFigaroenumelementindex(mFigaroenumelementindex),
                    failure_variable_names(failure_variable_names),
                    enum_variables_names(enum_variables_names),
                    float_variables_names(float_variables_names),
                    topevent(topevent),
                    numBoolState(numBoolState),
                    numBoolFailureState(numBoolFailureState),
                    numFloatState(numFloatState),
                    numIntState(numIntState),
            numEnumState(numEnumState),
                    ins_transition_found(ins_transition_found)
<<<<<<< HEAD
                    {
                            boolState.resize(numBoolState,0);
                            boolFailureState.resize(numBoolFailureState,0);
                          backupBoolState.resize(numBoolState,0);
                                floatState.resize(numFloatState,0);
                                backupFloatState.resize(numFloatState,0);
                                intState.resize(numIntState,0);
                                backupIntState.resize(numIntState,0);
                                enumState.resize(numEnumState,0);
                                backupEnumState.resize(numEnumState,0);
                    }
=======
            {
                boolState.resize(numBoolState, 0);
                backupBoolState.resize(numBoolState, 0);
                floatState.resize(numFloatState, 0);
                backupFloatState.resize(numFloatState, 0);
                intState.resize(numIntState, 0);
                backupIntState.resize(numIntState, 0);
                enumState.resize(numEnumState, 0);
                backupEnumState.resize(numEnumState, 0);
            }
>>>>>>> 2dddc29ea061c2a0b33493dccd8ece2b3a1211bf


        public:

            std::map<std::string, size_t> mFigaroboolelementindex ;

            std::map<std::string, size_t> mFigarofailureelementindex ;
            std::map<std::string, size_t> mFigarofloatelementindex ;
            std::map<std::string, size_t> mFigarointelementindex ;
            std::map<std::string, size_t> mFigaroenumelementindex ;
<<<<<<< HEAD
            std::set<std::string> failure_variable_names;
=======
>>>>>>> 2dddc29ea061c2a0b33493dccd8ece2b3a1211bf
            std::set<std::string> enum_variables_names ;
            std::set<std::string> float_variables_names ;

            std::string topevent;
            int  numBoolState;
            int  numBoolFailureState;
            int  numFloatState;
            int  numIntState;
            int  numEnumState;
            bool ins_transition_found;

            std::vector<bool> boolState;
            std::vector<bool> boolFailureState;
            std::vector<bool> backupBoolState;
            std::vector<double> floatState;
            std::vector<double> backupFloatState;
            std::vector<int> intState;
            std::vector<int> backupIntState;
            std::vector<int> enumState;
            std::vector<int> backupEnumState;

<<<<<<< HEAD

=======
>>>>>>> 2dddc29ea061c2a0b33493dccd8ece2b3a1211bf
            virtual void init() = 0;
            virtual void saveCurrentState() = 0;
            virtual void printState() = 0;
            virtual void fireOccurrence(int numFire) = 0;
            virtual std::vector<std::tuple<int, double, std::string, int>> showFireableOccurrences() = 0;
            virtual void runOnceInteractionStep_default_step() = 0;
            virtual int compareStates() = 0;
            virtual void doReinitialisations() = 0;
            virtual void runInteractions() = 0;
            virtual void printstatetuple() = 0;
            virtual void fireinsttransitiongroup(std::string) = 0;
            virtual int_fast64_t stateSize() const = 0;
            virtual bool figaromodelhasinstransitions() = 0;
<<<<<<< HEAD


=======
>>>>>>> 2dddc29ea061c2a0b33493dccd8ece2b3a1211bf
        };
    }
}


