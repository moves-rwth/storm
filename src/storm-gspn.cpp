#include "src/builder/ExplicitGspnModelBuilder.h"
#include "src/exceptions/BaseException.h"
#include "src/parser/GspnParser.h"
#include "src/storage/gspn/GSPN.h"
#include "src/storage/gspn/GspnBuilder.h"
#include "src/utility/macros.h"
#include "src/utility/initialize.h"

#include "src/storage/expressions/ExpressionManager.h"
#include "src/storage/jani/Model.h"
#include "src/storage/jani/JsonExporter.h"
#include "src/builder/JaniGSPNBuilder.h"
#include <fstream>
#include <iostream>
#include <string>
// TODO clean-up includes after doing all other todos

/*!
 * Parses the arguments to storm-gspn
 * The read data is stored in the different arguments (e.g., inputFile, formula, ...)
 *
 * @param argc number of arguments passed to storm-gspn
 * @param argv array of arguments
 * @param inputFile the input file is stored in this object
 * @param formula the formula is stored in this object
 * @param outputFile the output file is stored in this object
 * @param outputType the output type is stored in this object
 * @return false if the help flag is set or the input file is missing
 */
bool parseArguments(const int argc, const char **argv, std::string &inputFile, std::string &formula,
                    std::string &outputFile, std::string &outputType) {
    auto end = argv + argc;
    bool result = false;

    for (auto it = argv; it != end; ++it) {
        std::string currentArg = *it;

        // parse input file argument
        if (currentArg == "--input_file" || currentArg == "-i") {
            auto next = it + 1;
            if (next == end) {
                return -1;
            } else {
                inputFile = *next;
                result = true;
            }
            break;
        }

        // parse formula argument
        if (currentArg == "--formula" || currentArg == "-f") {
            auto next = it + 1;
            if (next != end) {
                return -1;
            } else {
                formula = *next;
            }
            break;
        }

        // parse output file argument
        if (currentArg == "--output_file" || currentArg == "-o") {
            auto next = it + 1;
            if (next != end) {
                return -1;
            } else {
                outputFile = *next;
            }
            break;
        }

        // parse output file type argument
        if (currentArg == "--output_type" || currentArg == "-ot") {
            auto next = it + 1;
            if (next != end) {
                return -1;
            } else {
                outputType = *next;
            }
            break;
        }

        // parse help argument
        if (currentArg == "--help" || currentArg == "-h") {
            return false;
        }
    }

    return result;
}

/*!
 * Print the manual of storm-gspn
 */
void printHelp() {
    std::cout << "storm-gspn -i input_file [-f formula] [-o output_file] [-ot output_type] [-h]" << std::endl;
    std::cout << std::endl;
    std::cout << "-i, --input_file:   file which contains the gspn" << std::endl;
    std::cout << "-f, --formula:      formula which should be checked on the gspn" << std::endl;
    std::cout << "-o, -output_file:   file in which the gspn/markov automaton should be stored" << std::endl
              << "                    requires the option -ot to be set" << std::endl;
    std::cout << "-ot, --output_type: possible output types are: pnml, pnpro or ma" << std::endl;
}

void handleJani(storm::gspn::GSPN const& gspn) {
    std::shared_ptr<storm::expressions::ExpressionManager> exprManager(new storm::expressions::ExpressionManager());
    storm::builder::JaniGSPNBuilder builder(gspn, exprManager);
    storm::jani::Model* model = builder.build();
    storm::jani::JsonExporter::toFile(*model, "gspn.jani");
    delete model;
}

int main(const int argc, const char **argv) {
    std::string inputFile, formula, outputFile, outputType;
    if (!parseArguments(argc, argv, inputFile, formula, outputFile, outputType)) {
        printHelp();
        return 1;
    }

    try {
        storm::utility::setUp();

        // parse GSPN from file
        auto parser = storm::parser::GspnParser();
        auto gspn = parser.parse(inputFile);

        // todo ------[marker]
        gspn.isValid();

        handleJani(gspn);
        //storm::gspn::GspnBuilder builder2;
        //builder2.addPlace(2);

        //
        //std::ofstream file;
        //file.open("/Users/thomas/Desktop/storm.dot");
        //gspn.writeDotToStream(file);
        //file.close();

        //std::ofstream file;
        //file.open("/Users/thomas/Desktop/gspn.pnpro");
        //gspn.toPnpro(file);
        //file.close();

        std::cout << "Parsing complete!" << std::endl;


        // Construct MA
        //auto builder = storm::builder::ExplicitGspnModelBuilder<>();
        //auto ma = builder.translateGspn(gspn, argv[2]);
        //std::cout << "Markov Automaton: " << std::endl;
        //std::cout << "number of states: " << ma.getNumberOfStates() << std::endl;
        //std::cout << "number of transitions: " << ma.getNumberOfTransitions() << std::endl << std::endl;





        // All operations have now been performed, so we clean up everything and terminate.
        storm::utility::cleanUp();
        return 0;
    } catch (storm::exceptions::BaseException const& exception) {
        STORM_LOG_ERROR("An exception caused StoRM to terminate. The message of the exception is: " << exception.what());
    } catch (std::exception const& exception) {
        STORM_LOG_ERROR("An unexpected exception occurred and caused StoRM to terminate. The message of this exception is: " << exception.what());
    }
}
