#include <string>
#include <unordered_map>

#include "storm/exceptions/NotSupportedException.h"
#include "storm/storage/IntegerInterval.h"
#include "storm/storage/jani/EdgeDestination.h"
#include "storm/storage/jani/Location.h"
#include "storm/storage/jani/Model.h"
#include "storm/utility/macros.h"

#include "storm-pgcl/storage/ppg/ProgramGraph.h"

namespace storm {
namespace builder {

enum class JaniProgramGraphVariableDomainMethod { Unrestricted, IntervalPropagation };

struct JaniProgramGraphBuilderSetting {
    /// Method how to obtain domain for the variables; currently only unrestricted is supported
    JaniProgramGraphVariableDomainMethod variableDomainMethod = JaniProgramGraphVariableDomainMethod::Unrestricted;
    /// If this is true, reward variables will be given special treatment, effectively removing them from the state space.
    /// Disable in order to obtain full state space.
    bool filterRewardVariables = true;
};

class JaniProgramGraphBuilder {
   public:
    static unsigned janiVersion;

    JaniProgramGraphBuilder(storm::ppg::ProgramGraph const& pg, JaniProgramGraphBuilderSetting const& pgbs = JaniProgramGraphBuilderSetting())
        : programGraph(pg), pgbs(pgbs) {
        if (pgbs.filterRewardVariables) {
            rewards = programGraph.rewardVariables();
        }
        constants = programGraph.constants();
        auto boundedVars = programGraph.constantAssigned();
        for (auto const& v : boundedVars) {
            variableRestrictions.emplace(v, programGraph.supportForConstAssignedVariable(v));
        }
    }

    virtual ~JaniProgramGraphBuilder() {
        // intentionally left empty
    }

    void restrictAllVariables(int64_t from, int64_t to) {
        restrictAllVariables(storm::storage::IntegerInterval(from, to));
    }

    void restrictAllVariables(storm::storage::IntegerInterval const& restr) {
        for (auto const& v : programGraph.getVariables()) {
            if (isConstant(v.first)) {
                continue;
            }
            if (variableRestrictions.count(v.first) > 0) {
                continue;  // Currently we ignore user bounds if we have bounded integers;
            }
            if (v.second.hasIntegerType()) {
                userVariableRestrictions.emplace(v.first, restr);
            }
        }
    }

    storm::jani::Model* build(std::string const& name = "program_graph") {
        expManager = programGraph.getExpressionManager();
        storm::jani::Model* model = new storm::jani::Model(name, storm::jani::ModelType::MDP, janiVersion, expManager);
        storm::jani::Automaton mainAutomaton("main", expManager->declareIntegerVariable("pc"));
        addProcedureVariables(*model, mainAutomaton);
        janiLocId = addProcedureLocations(*model, mainAutomaton);
        addVariableOoBLocations(mainAutomaton);
        addEdges(mainAutomaton);
        model->addAutomaton(mainAutomaton);
        model->setStandardSystemComposition();
        model->getModelFeatures().add(storm::jani::ModelFeature::DerivedOperators);
        model->finalize();
        return model;
    }

   private:
    std::string janiLocationName(storm::ppg::ProgramLocationIdentifier i) {
        return "l" + std::to_string(i);
    }

    std::string janiVariableOutOfBoundsLocationName(storm::ppg::ProgramVariableIdentifier i) {
        return "oob-" + programGraph.getVariableName(i);
    }

    storm::jani::OrderedAssignments buildOrderedAssignments(storm::jani::Automaton& automaton, storm::ppg::DeterministicProgramAction const& act);
    void addEdges(storm::jani::Automaton& automaton);
    std::vector<std::pair<uint64_t, storm::expressions::Expression>> buildDestinations(storm::jani::Automaton& automaton, storm::ppg::ProgramEdge const& edge,
                                                                                       storm::jani::TemplateEdge& templateEdge);
    /**
     * Helper for probabilistic assignments
     */
    std::vector<std::pair<uint64_t, storm::expressions::Expression>> buildProbabilisticDestinations(storm::jani::Automaton& automaton,
                                                                                                    storm::ppg::ProgramEdge const& edge,
                                                                                                    storm::jani::TemplateEdge& templateEdge);

    std::pair<std::vector<storm::jani::Edge>, storm::expressions::Expression> addVariableChecks(storm::jani::Automaton& automaton,
                                                                                                storm::ppg::ProgramEdge const& edge);

    bool isUserRestrictedVariable(storm::ppg::ProgramVariableIdentifier i) const {
        return userVariableRestrictions.count(i) == 1 && !isRewardVariable(i);
    }

    bool isRestrictedVariable(storm::ppg::ProgramVariableIdentifier i) const {
        // Might be different from user restricted in near future.
        return (variableRestrictions.count(i) == 1 && !isRewardVariable(i)) || isUserRestrictedVariable(i);
    }

    storm::storage::IntegerInterval const& variableBounds(storm::ppg::ProgramVariableIdentifier i) const {
        assert(isRestrictedVariable(i));
        if (userVariableRestrictions.count(i) == 1) {
            return userVariableRestrictions.at(i);
        } else {
            return variableRestrictions.at(i);
        }
    }

    bool isRewardVariable(storm::ppg::ProgramVariableIdentifier i) const {
        return std::find(rewards.begin(), rewards.end(), i) != rewards.end();
    }

    bool isConstant(storm::ppg::ProgramVariableIdentifier i) const {
        return std::find(constants.begin(), constants.end(), i) != constants.end();
    }

    void addProcedureVariables(storm::jani::Model& model, storm::jani::Automaton& automaton);

    std::map<storm::ppg::ProgramLocationIdentifier, uint64_t> addProcedureLocations(storm::jani::Model& model, storm::jani::Automaton& automaton) {
        std::map<storm::ppg::ProgramLocationIdentifier, uint64_t> result;
        std::map<std::string, storm::jani::Variable const*> labelVars;
        std::set<std::string> labels = programGraph.getLabels();
        for (auto const& label : labels) {
            auto janiVar = storm::jani::Variable::makeBooleanVariable(label, expManager->declareBooleanVariable(label), expManager->boolean(false), true);
            labelVars.emplace(label, &model.addVariable(*janiVar));
        }

        for (auto it = programGraph.locationBegin(); it != programGraph.locationEnd(); ++it) {
            storm::jani::Location janiLoc(janiLocationName(it->second.id()));
            for (auto const& label : programGraph.getLabels(it->second.id())) {
                assert(labelVars.count(label) > 0);
                janiLoc.addTransientAssignment(storm::jani::Assignment(storm::jani::LValue(*(labelVars.at(label))), expManager->boolean(true)));
            }
            result[it->second.id()] = automaton.addLocation(janiLoc);
            if (it->second.isInitial()) {
                automaton.addInitialLocation(result[it->second.id()]);
            }
        }
        return result;
    }

    void addVariableOoBLocations(storm::jani::Automaton& automaton) {
        for (auto const& restr : userVariableRestrictions) {
            if (!isRewardVariable(restr.first)) {
                storm::jani::Location janiLoc(janiVariableOutOfBoundsLocationName(restr.first));
                uint64_t locId = automaton.addLocation(janiLoc);
                varOutOfBoundsLocations[restr.first] = locId;
            }
        }
    }
    /// Transient variables
    std::vector<storm::ppg::ProgramVariableIdentifier> rewards;
    /// Variables that are constants
    std::vector<storm::ppg::ProgramVariableIdentifier> constants;
    /// Restrictions on variables (automatic)
    std::map<uint64_t, storm::storage::IntegerInterval> variableRestrictions;
    /// Restrictions on variables (provided by users)
    std::map<uint64_t, storm::storage::IntegerInterval> userVariableRestrictions;

    /// Locations for variables that would have gone out of bounds
    std::map<uint64_t, uint64_t> varOutOfBoundsLocations;
    std::map<storm::ppg::ProgramLocationIdentifier, uint64_t> janiLocId;
    std::map<storm::ppg::ProgramVariableIdentifier, std::shared_ptr<storm::jani::Variable>> variables;

    /// The expression manager
    std::shared_ptr<storm::expressions::ExpressionManager> expManager;
    /// The program graph to be translated
    storm::ppg::ProgramGraph const& programGraph;
    /// Settings
    JaniProgramGraphBuilderSetting pgbs;
};
}  // namespace builder
}  // namespace storm
