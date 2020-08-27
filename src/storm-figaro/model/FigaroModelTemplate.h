#pragma once

#include <array>
#include <map>
#include <vector>
#include <sstream>
#include<math.h>
#include <set>

namespace storm {
    namespace figaro {

        class FigaroProgram {

        protected:
            //constructor to initialize all variables through derived class
            FigaroProgram(
                    std::map<std::string, size_t> mFigaroboolelementindex,
            std::map<std::string, size_t> mFigaroelementfailureindex,
            std::map<std::string, size_t> mFigarofloatelementindex,

            std::map<std::string, size_t> mFigarointelementindex,

            std::map<std::string, size_t> mFigaroenumelementindex ,

            std::set<std::string> enum_variables_names ,

            std::set<std::string> float_variables_names,
            std::string topevent,
             int  numBoolState,
             int  numFloatState,
             int  numIntState,
             int  numEnumState,
             bool ins_transition_found
                    ):
            mFigaroboolelementindex(mFigaroboolelementindex),
                    mFigaroelementfailureindex(mFigaroelementfailureindex),
                    mFigarofloatelementindex(mFigarofloatelementindex),
                    mFigaroenumelementindex(mFigaroenumelementindex),
                    enum_variables_names(enum_variables_names),
                    float_variables_names(float_variables_names),
                    topevent(topevent),
                    numBoolState(numBoolState),
                    numFloatState(numFloatState),
                    numIntState(numIntState),
                    ins_transition_found(ins_transition_found)
                    {
                            boolState.resize(numBoolState,0);
                          backupBoolState.resize(numBoolState,0);
                                floatState.resize(numFloatState,0);
                                backupFloatState.resize(numFloatState,0);
                                intState.resize(numIntState,0);
                                backupIntState.resize(numIntState,0);
                                enumState.resize(numEnumState,0);
                                backupEnumState.resize(numEnumState,0);
                    }

        public:


            std::map<std::string, size_t> mFigaroboolelementindex ;
            std::map<std::string, size_t> mFigaroelementfailureindex ;
            std::map<std::string, size_t> mFigarofloatelementindex ;

            std::map<std::string, size_t> mFigarointelementindex ;

            std::map<std::string, size_t> mFigaroenumelementindex ;

            std::set<std::string> enum_variables_names ;

            std::set<std::string> float_variables_names ;

            std::string topevent;
            int  numBoolState;
            int  numFloatState;
            int  numIntState;
            int  numEnumState;
            bool ins_transition_found;
            std::vector<bool> boolState;
            std::vector<bool> backupBoolState;
            std::vector<float> floatState;
            std::vector<float> backupFloatState;
            std::vector<int> intState;
            std::vector<int> backupIntState;
            std::vector<int> enumState;
            std::vector<int> backupEnumState;

//            std::array<bool, numBoolState> boolState;
//            std::array<bool, numBoolState> backupBoolState;
//            std::array<float, numFloatState> floatState;
//            std::array<float, numFloatState> backupFloatState;
//            std::array<int, numIntState> intState;
//            std::array<int, numIntState> backupIntState;
//            std::array<int, numEnumState> enumState;
//            std::array<int, numEnumState> backupEnumState;

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

        };
    }
}


