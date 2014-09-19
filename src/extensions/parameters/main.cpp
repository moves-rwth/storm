//
//int main(const int argc, const char* argv[]) {
//    // Print an information header.
//	//printHeader(argc, argv);
//
//    // Initialize the logging engine and perform other initalizations.
//	initializeLogger();
//	setUp();
//    
//    // Program Translation Time Measurement, Start
//    std::chrono::high_resolution_clock::time_point programTranslationStart = std::chrono::high_resolution_clock::now();
//
//    // First, we build the model using the given symbolic model description and constant definitions.
//    std::string const& programFile = s->getOptionByLongName("symbolic").getArgument(0).getValueAsString();
//    std::string const& constants = s->getOptionByLongName("constants").getArgument(0).getValueAsString();
//    storm::prism::Program program = storm::parser::PrismParser::parse(programFile);
//
//    // Program Translation Time Measurement, End
//    std::chrono::high_resolution_clock::time_point programTranslationEnd = std::chrono::high_resolution_clock::now();
//    std::cout << "Parsing and translating the Symbolic Input took " << std::chrono::duration_cast<std::chrono::milliseconds>(programTranslationEnd - programTranslationStart).count() << " milliseconds." << std::endl;
//    storm_parametric(constants, program);
//}