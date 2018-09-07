//
// Created by Jip Spel on 05.09.18.
//

#include "MonotonicityChecker.h"
#include "storm/exceptions/UnexpectedException.h"
#include "storm/exceptions/NotSupportedException.h"

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
                    auto second = (*(++row.begin()));
                    std::string color = "";
                    auto val = first.getValue();
                    auto vars = val.gatherVariables();
                    for (auto itr = vars.begin(); itr != vars.end(); ++itr) {
                        auto derivative = val.derivative(*itr);
                        STORM_LOG_THROW(derivative.isConstant(), storm::exceptions::NotSupportedException, "Expecting derivative to be constant");

                        if (varsMonotone.find(*itr) == varsMonotone.end()) {
                            varsMonotone[*itr].first = true;
                            varsMonotone[*itr].second = true;
                        }

                        auto compare = lattice->compare(first.getColumn(), second.getColumn());
                        std::pair<bool, bool>* value = &varsMonotone.find(*itr)->second;
                        std::pair<bool, bool> old = *value;
                        if (compare == storm::analysis::Lattice::ABOVE) {
                            value->first &=derivative.constantPart() >= 0;
                            value->second &=derivative.constantPart() <= 0;
                        } else if (compare == storm::analysis::Lattice::BELOW) {
                            value->first &=derivative.constantPart() <= 0;
                            value->second &=derivative.constantPart() >= 0;
                        } else if (compare == storm::analysis::Lattice::SAME) {
                            // Behaviour doesn't matter, as they are at the same level
                        } else {
                            value->first = false;
                            value->second = false;
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

                    myfile << "\t" << i << " -> " << first.getColumn() << "[" << color << "label=\"" << first.getValue() << "\"];"
                           << std::endl;
                    myfile << "\t" << i << " -> " << second.getColumn() << "[" << color << "label=\"" << second.getValue() << "\"];"
                           << std::endl;
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
        };
        template class MonotonicityChecker<storm::RationalFunction>;
    }

}

