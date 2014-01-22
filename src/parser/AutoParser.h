#ifndef STORM_PARSER_AUTOPARSER_H_
#define STORM_PARSER_AUTOPARSER_H_

#include "src/models/AbstractModel.h"

#include <string>

namespace storm {
	namespace parser {

		class AutoParser {
		public:

			/*!
			 *	Checks the given files and parses the model within these files.
			 *
			 *	This parser analyzes the format hint in the first line of the transition
			 *	file. If this is a valid format, it will use the parser for this format,
			 *	otherwise it will throw an exception.
			 *
			 *	When the files are parsed successfully, a shared pointer owning the resulting model is returned.
			 *	The concrete model can be obtained using the as<Type>() member of the AbstractModel class.
			 *
			 * @param transitionsFilename The name of the file containing the transitions of the Markov automaton.
			 * @param labelingFilename The name of the file containing the labels for the states of the Markov automaton.
			 * @param stateRewardFilename The name of the file that contains the state reward of the Markov automaton.
			 * @param transitionRewardFilename The name of the file that contains the transition rewards of the Markov automaton.
			 * @return A shared_ptr containing the resulting model.
			 */
			static std::shared_ptr<storm::models::AbstractModel<double>> parseModel(std::string const & transitionSystemFile,
																   std::string const & labelingFile,
																   std::string const & stateRewardFile = "",
																   std::string const & transitionRewardFile = "");

		private:

			/*!
			 *	Opens the given file and parses the file format hint.
			 *
			 *	@param filename The path and name of the file that is to be analysed.
			 *	@return The type of the model as an enum value.
			 */
			static storm::models::ModelType analyzeHint(const std::string& filename);
		};
		
	} // namespace parser
} // namespace storm

#endif /* STORM_PARSER_AUTOPARSER_H_ */
