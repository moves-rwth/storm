#include "src/storage/jani/ParallelComposition.h"

#include <sstream>

#include <boost/algorithm/string/join.hpp>

#include "src/utility/macros.h"
#include "src/exceptions/WrongFormatException.h"

namespace storm {
    namespace jani {
        
        const std::string SynchronizationVector::noAction = "-";
        
        SynchronizationVector::SynchronizationVector(std::vector<std::string> const& input, std::string const& output) : input(input), output(output) {
            // Intentionally left empty.
        }
        
        std::size_t SynchronizationVector::size() const {
            return input.size();
        }
        
        std::vector<std::string> const& SynchronizationVector::getInput() const {
            return input;
        }
        
        std::string const& SynchronizationVector::getInput(uint64_t index) const {
            return input[index];
        }
        
        std::string const& SynchronizationVector::getNoActionInput() const {
            return SynchronizationVector::noAction;
        }
        
        std::string const& SynchronizationVector::getOutput() const {
            return output;
        }
        
        bool SynchronizationVector::isNoActionInput(std::string const& action) const {
            return action == noAction;
        }
        
        std::ostream& operator<<(std::ostream& stream, SynchronizationVector const& synchronizationVector) {
            bool first = true;
            stream << "(";
            for (auto const& element : synchronizationVector.getInput()) {
                if (!first) {
                    stream << ", ";
                }
                stream << element;
                first = false;
            }
            stream << ") -> " << synchronizationVector.getOutput();
            return stream;
        }
        
        ParallelComposition::ParallelComposition(std::vector<std::shared_ptr<Composition>> const& subcompositions, std::vector<SynchronizationVector> const& synchronizationVectors) : subcompositions(subcompositions), synchronizationVectors(synchronizationVectors) {
            STORM_LOG_THROW(subcompositions.size() > 1, storm::exceptions::WrongFormatException, "At least two automata required for parallel composition.");
            this->checkSynchronizationVectors();
        }
        
        ParallelComposition::ParallelComposition(std::vector<std::shared_ptr<Composition>> const& subcompositions, std::set<std::string> const& synchronizationAlphabet) : subcompositions(subcompositions), synchronizationVectors() {
            STORM_LOG_THROW(subcompositions.size() > 1, storm::exceptions::WrongFormatException, "At least two automata required for parallel composition.");

            // Manually construct the synchronization vectors for all elements of the synchronization alphabet.
            for (auto const& action : synchronizationAlphabet) {
                synchronizationVectors.emplace_back(std::vector<std::string>(this->subcompositions.size(), action), action);
            }
        }
        
        ParallelComposition::ParallelComposition(std::shared_ptr<Composition> const& leftSubcomposition, std::shared_ptr<Composition> const& rightSubcomposition, std::set<std::string> const& synchronizationAlphabet) {
            subcompositions.push_back(leftSubcomposition);
            subcompositions.push_back(rightSubcomposition);
            
            // Manually construct the synchronization vectors for all elements of the synchronization alphabet.
            for (auto const& action : synchronizationAlphabet) {
                synchronizationVectors.emplace_back(std::vector<std::string>(this->subcompositions.size(), action), action);
            }
        }
        
        Composition const& ParallelComposition::getSubcomposition(uint64_t index) const {
            return *subcompositions[index];
        }
        
        std::vector<std::shared_ptr<Composition>> const& ParallelComposition::getSubcompositions() const {
            return subcompositions;
        }
        
        uint64_t ParallelComposition::getNumberOfSubcompositions() const {
            return subcompositions.size();
        }
        
        SynchronizationVector const& ParallelComposition::getSynchronizationVector(uint64_t index) const {
            return synchronizationVectors[index];
        }
        
        std::vector<SynchronizationVector> const& ParallelComposition::getSynchronizationVectors() const {
            return synchronizationVectors;
        }
        
        std::size_t ParallelComposition::getNumberOfSynchronizationVectors() const {
            return synchronizationVectors.size();
        }
        
        void ParallelComposition::checkSynchronizationVectors() const {
            for (uint_fast64_t inputIndex = 0; inputIndex < subcompositions.size(); ++ inputIndex) {
                std::set<std::string> actions;
                for (auto const& vector : synchronizationVectors) {
                    STORM_LOG_THROW(vector.size() == this->subcompositions.size(), storm::exceptions::WrongFormatException, "Synchronization vectors must match parallel composition size.");
                    std::string const& action = vector.getInput(inputIndex);
                    STORM_LOG_THROW(actions.find(action) == actions.end(), storm::exceptions::WrongFormatException, "Cannot use the same action multiple times as input in synchronization vectors.");
                    actions.insert(action);
                }
            }
        }
        
        boost::any ParallelComposition::accept(CompositionVisitor& visitor, boost::any const& data) const {
            return visitor.visit(*this, data);
        }
        
        void ParallelComposition::write(std::ostream& stream) const {
            std::vector<std::string> synchronizationVectorsAsStrings;
            for (auto const& synchronizationVector : synchronizationVectors) {
                std::stringstream tmpStream;
                tmpStream << synchronizationVector;
                synchronizationVectorsAsStrings.push_back(tmpStream.str());
            }
            
            bool first = true;
            stream << "(";
            for (auto const& subcomposition : subcompositions) {
                if (!first) {
                    stream << " || ";
                }
                stream << *subcomposition;
                first = false;
            }
            stream << ")[" << boost::algorithm::join(synchronizationVectorsAsStrings, ", ") << "]";
        }
        
    }
}
