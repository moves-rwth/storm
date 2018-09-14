//
// Created by Jip Spel on 05.09.18.
//

#include "MonotonicityChecker.h"
#include "storm/exceptions/NotSupportedException.h"
#include "storm/exceptions/UnexpectedException.h"   

namespace storm {
    namespace analysis {
        template <typename ValueType>
        void MonotonicityChecker<ValueType>::checkMonotonicity(std::map<storm::analysis::Lattice*, std::set<std::shared_ptr<storm::expressions::BinaryRelationExpression>>> map, storm::storage::SparseMatrix<ValueType> matrix) {
            auto i = 0;
            for (auto itr = map.begin(); itr != map.end(); ++itr) {
                auto lattice = itr->first;
                auto assumptions = itr->second;
                std::ofstream myfile;
                std::string filename = "lattice" + std::to_string(i) + ".dot";
                myfile.open (filename);
                lattice->toDotFile(myfile);
                myfile.close();

                if (assumptions.size() > 0) {
                    STORM_PRINT("Given assumptions: " << std::endl);
                    bool first = true;
                    for (auto itr = assumptions.begin(); itr != assumptions.end(); ++itr) {
                        if (!first) {
                            STORM_PRINT(" ^ ");
                        } else {
                            STORM_PRINT("    ");
                        }
                        first = false;
                        std::shared_ptr<storm::expressions::BinaryRelationExpression> expression = *itr;
                        auto var1 = expression->getFirstOperand();
                        auto var2 = expression->getSecondOperand();
                        STORM_PRINT("(" << var1->getIdentifier() << " > " << var2->getIdentifier() << ")");
                    }
                    STORM_PRINT(std::endl);
                }

                std::map<carl::Variable, std::pair<bool, bool>> varsMonotone = analyseMonotonicity(i, lattice, matrix);
                for (auto itr2 = varsMonotone.begin(); itr2 != varsMonotone.end(); ++itr2) {
                    if (itr2->second.first) {
                        STORM_PRINT("  - Monotone increasing in: " << itr2->first << std::endl);
                    } else {
                        STORM_PRINT("  - Do not know if monotone increasing in: " << itr2->first << std::endl);
                    }
                    if (itr2->second.second) {
                        STORM_PRINT("  - Monotone decreasing in: " << itr2->first << std::endl);
                    } else {
                        STORM_PRINT("  - Do not know if monotone decreasing in: " << itr2->first << std::endl);
                    }
                }
                ++i;
            }
        }

        template <typename ValueType>
        std::map<carl::Variable, std::pair<bool, bool>> MonotonicityChecker<ValueType>::analyseMonotonicity(uint_fast64_t i, storm::analysis::Lattice* lattice, storm::storage::SparseMatrix<ValueType> matrix) {
            std::map<carl::Variable, std::pair<bool, bool>> varsMonotone;
            std::ofstream myfile;
            std::string filename = "mc" + std::to_string(i) + ".dot";
            myfile.open (filename);
            myfile << "digraph \"MC\" {" << std::endl;
            myfile << "\t" << "node [shape=ellipse]" << std::endl;

            // print all nodes
            for (uint_fast64_t i = 0; i < matrix.getColumnCount(); ++i) {
                myfile << "\t\"" << i << "\" [label = \"" << i << "\"]" << std::endl;
            }

            for (uint_fast64_t i = 0; i < matrix.getColumnCount(); ++i) {
                // go over all rows
                auto row = matrix.getRow(i);

                auto first = (*row.begin());
                if (first.getValue() != ValueType(1)) {
                    std::map<uint_fast64_t, ValueType> transitions;

                    for (auto itr = row.begin(); itr != row.end(); ++itr) {
                        transitions.insert(std::pair<uint_fast64_t, ValueType>((*itr).getColumn(), (*itr).getValue()));
                    }

                    std::string color = "";
                    auto val = first.getValue();
                    auto vars = val.gatherVariables();
                    for (auto itr = vars.begin(); itr != vars.end(); ++itr) { if (varsMonotone.find(*itr) == varsMonotone.end()) {
                            varsMonotone[*itr].first = true;
                            varsMonotone[*itr].second = true;
                        }
                        std::pair<bool, bool>* value = &varsMonotone.find(*itr)->second;
                        std::pair<bool, bool> old = *value;

                        for (auto itr2 = transitions.begin(); itr2 != transitions.end(); ++itr2) {
                            for (auto itr3 = transitions.begin(); itr3 != transitions.end(); ++itr3) {
                                auto derivative2 = (*itr2).second.derivative(*itr);
                                auto derivative3 = (*itr3).second.derivative(*itr);
                                STORM_LOG_THROW(derivative2.isConstant() && derivative3.isConstant(), storm::exceptions::NotSupportedException, "Expecting derivative to be constant");

                                auto compare = lattice->compare((*itr2).first, (*itr3).first);

                                if (compare == storm::analysis::Lattice::ABOVE) {
                                    // As the first state (itr2) is above the second state (itr3) it is sufficient to look at the derivative of itr2.
                                    value->first &=derivative2.constantPart() >= 0;
                                    value->second &=derivative2.constantPart() <= 0;
                                } else if (compare == storm::analysis::Lattice::BELOW) {
                                    // As the second state (itr3) is above the first state (itr2) it is sufficient to look at the derivative of itr3.
                                    value->first &=derivative3.constantPart() >= 0;
                                    value->second &=derivative3.constantPart() <= 0;
                                } else if (compare == storm::analysis::Lattice::SAME) {
                                    // Behaviour doesn't matter, as the states are at the same level.
                                } else {
                                    // As the relation between the states is unknown, we can't claim anything about the monotonicity.
                                    value->first = false;
                                    value->second = false;
                                }
                            }
                        }


                        if ((value->first != old.first) && (value->second != old.second)) {
                            color = "color = red, ";
                        } else if ((value->first != old.first)) {
                            myfile << "\t edge[style=dashed];" << std::endl;
                            color = "color = blue, ";
                        } else if ((value->second != old.second)) {
                            myfile << "\t edge[style=dotted];" << std::endl;
                            color = "color = blue, ";
                        }
                    }

                    for (auto itr = transitions.begin(); itr != transitions.end(); ++itr) {
                        myfile << "\t" << i << " -> " << itr->first << "[" << color << "label=\"" << itr->second << "\"];"
                               << std::endl;
                    }

                    myfile << "\t edge[style=\"\"];" << std::endl;
                } else {
                    myfile << "\t" << i << " -> " << first.getColumn() << "[label=\"" << first.getValue() << "\"];"
                           << std::endl;
                }
            }

            myfile << "\tsubgraph legend {" << std::endl;
            myfile << "\t\tnode [color=white];" << std::endl;
            myfile << "\t\tedge [style=invis];" << std::endl;
            myfile << "\t\tt0 [label=\"incr? and decr?\", fontcolor=red];" << std::endl;
            myfile << "\t\tt1 [label=\"incr? (dashed)\", fontcolor=blue];" << std::endl;
            myfile << "\t\tt2 [label=\"decr? (dotted)\", fontcolor=blue];" << std::endl;

            myfile << "\t}" << std::endl;
            myfile << "}" << std::endl;
            myfile.close();
            return varsMonotone;
        }

        template class MonotonicityChecker<storm::RationalFunction>;
    }
}
