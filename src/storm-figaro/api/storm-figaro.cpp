
#include <stdlib.h>
#include <string>
#include "storm-figaro/model/FigaroModel.h"
#include "storm-figaro/model/FigaroModel2.h"
//#include "storm-figaro/model/FigaroModelTemplate.h"
#include "storm-figaro/api/storm-figaro.h"
#include <random>
#include "storm/builder/DdJaniModelBuilder.h"
#include "storm/api/export.h"
#include "storm/api/properties.h"
#include "storm-parsers/api/properties.h"


namespace storm {
    namespace figaro {
        namespace api{


            std::shared_ptr<storm::figaro::FigaroProgram> loadFigaroProgram(){
                std::shared_ptr<storm::figaro::FigaroProgram> figaromodel=  std::make_shared<storm::figaro::FigaroProgram1> (storm::figaro::FigaroProgram1());
                return figaromodel;
            }
//pare properties form teh xml file
            std::vector<std::string> parseXmlProperties(std::string filepath, std::vector<std::string> properties){
                std::ofstream contents;
                storm::utility::openFile(filepath, contents);
                //    ParseXML file and interpret properties
                storm::utility::closeFile(contents);
                //I set only two prperties here

                //TODO use the following xml support
                //#ifdef STORM_HAVE_XERCES
                //#include <xercesc/parsers/XercesDOMParser.hpp>
                //#include <xercesc/util/XMLString.hpp>

                int missiontime = 100;
                properties.push_back("Pmax=?  [F<="+ std::to_string(missiontime) + " \"failed\"]");

                std::stringstream stream;
                stream << "Pmax=? [F [" << missiontime<<"," << missiontime<< "] \"failed\"]";
                properties.push_back(stream.str());
                return properties;
            }

//build properties

            std::vector<std::shared_ptr<storm::logic::Formula const>> buildProperties(std::vector<std::string> properties)
        {
            std::vector<std::shared_ptr<storm::logic::Formula const>> props;
            if (!properties.empty()) {
                std::string propString;
                for (size_t i = 0; i < properties.size(); ++i) {
                    propString += properties[i];
                    if (i + 1 < properties.size()) {
                        propString += ";";
                    }
                }
                props = storm::api::extractFormulasFromProperties(storm::api::parseProperties(propString));
            }
            return props;
        }



            std::vector<storm::expressions::Variable>  getFigaroBooleanVariables(storm::figaro::FigaroProgram & figaroprogram)
            {
                std::vector<storm::expressions::Variable> boolean_variables;
                auto manager = std::make_shared<storm::expressions::ExpressionManager>();

                for (auto & bool_var : figaroprogram.mFigaroboolelementindex) {
                    boolean_variables.emplace_back(manager->declareBooleanVariable(bool_var.first));
                    std::cout<<bool_var.first;
                }
                for (auto & bool_var : figaroprogram.mFigarofailureelementindex) {
                    boolean_variables.emplace_back(manager->declareBooleanVariable(bool_var.first));
                    std::cout<<bool_var.first;
                }
                return boolean_variables;
            }
            std::vector<storm::expressions::Variable>  getFigaroIntegerVariables(storm::figaro::FigaroProgram & figaroprogram)
            {
                std::vector<storm::expressions::Variable> integer_variables;


                auto manager = std::make_shared<storm::expressions::ExpressionManager>();


                for (auto & float_var : figaroprogram.mFigarofloatelementindex) {

                    std::cout<<float_var.first;
                    integer_variables.emplace_back(manager->declareIntegerVariable(float_var.first));
                }

                for (auto & int_var : figaroprogram.mFigarointelementindex) {
                    std::cout<<int_var.first;
                    integer_variables.emplace_back(manager->declareIntegerVariable(int_var.first));
                }
                for (auto & enum_var : figaroprogram.mFigaroenumelementindex) {
                    integer_variables.emplace_back(manager->declareIntegerVariable(enum_var.first));
                }
                return integer_variables;
            }

            std::shared_ptr<storm::expressions::ExpressionManager>  getFigaroExpresseionManager(storm::figaro::FigaroProgram & figaroprogram) {
                std::vector<storm::expressions::Variable> boolean_variables;
                std::vector<storm::expressions::Variable> integer_variables;


                auto manager = std::make_shared<storm::expressions::ExpressionManager>();

                for (auto & bool_var : figaroprogram.mFigaroboolelementindex) {
                    boolean_variables.emplace_back(manager->declareBooleanVariable(bool_var.first));
                }
                for (auto & bool_var : figaroprogram.mFigarofailureelementindex) {
                    boolean_variables.emplace_back(manager->declareBooleanVariable(bool_var.first));
                    std::cout<<bool_var.first;
                }
                for (auto & float_var : figaroprogram.mFigarofloatelementindex) {

                    integer_variables.emplace_back(manager->declareIntegerVariable(float_var.first));
                }

                for (auto & int_var : figaroprogram.mFigarointelementindex) {
                    integer_variables.emplace_back(manager->declareIntegerVariable(int_var.first));
                }
                for (auto & enum_var : figaroprogram.mFigaroenumelementindex) {
                    integer_variables.emplace_back(manager->declareIntegerVariable(enum_var.first));
                }

                return manager;

            }


    }//namespace figaro
}//namespace storm

}
