#pragma once

#include <vector>
#include <map>
#include "storm/logic/CloneVisitor.h"

namespace storm {
    namespace logic {

        class ExtractMaximalStateFormulasVisitor : public CloneVisitor {
        public:
            typedef std::pair<std::string, std::shared_ptr<Formula const>> LabelFormulaPair;

            // Maintain a mapping from formula-strings to formula-label pairs in order to use the same label for the equivalent formulas (as strings)
            typedef std::map<std::string, LabelFormulaPair> FormulaLabelMapping;

            static std::shared_ptr<Formula> extract(PathFormula const& f, FormulaLabelMapping& extractedFormulas);

            virtual boost::any visit(BinaryBooleanPathFormula const& f, boost::any const& data) const override;
            virtual boost::any visit(BoundedUntilFormula const& f, boost::any const& data) const override;
            virtual boost::any visit(EventuallyFormula const& f, boost::any const& data) const override;
            virtual boost::any visit(GloballyFormula const& f, boost::any const& data) const override;
            virtual boost::any visit(NextFormula const& f, boost::any const& data) const override;
            virtual boost::any visit(UnaryBooleanPathFormula const& f, boost::any const& data) const override;
            virtual boost::any visit(UntilFormula const& f, boost::any const& data) const override;

            virtual boost::any visit(TimeOperatorFormula const& f, boost::any const& data) const override;
            virtual boost::any visit(LongRunAverageOperatorFormula const& f, boost::any const& data) const override;
            virtual boost::any visit(MultiObjectiveFormula const& f, boost::any const& data) const override;
            virtual boost::any visit(ProbabilityOperatorFormula const& f, boost::any const& data) const override;
            virtual boost::any visit(RewardOperatorFormula const& f, boost::any const& data) const override;

        private:
            ExtractMaximalStateFormulasVisitor(FormulaLabelMapping& extractedFormulas);

            std::shared_ptr<Formula> extract(std::shared_ptr<Formula> f) const;
            void incrementNestingLevel() const;
            void decrementNestingLevel() const;

            FormulaLabelMapping& extractedFormulas;
            std::size_t nestingLevel;
        };

    }
}

