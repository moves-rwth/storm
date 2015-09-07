#include "gtest/gtest.h"

#include "src/solver/MinMaxLinearEquationSolver.h"

TEST( MinMaxTechnique, Simple ) {
    storm::solver::MinMaxTechniqueSelection ts = storm::solver::MinMaxTechniqueSelection::PolicyIteration;
    storm::solver::MinMaxTechnique t = storm::solver::MinMaxTechnique::PolicyIteration;
       ASSERT_EQ(convert(ts), t);
    
    
}