#include "stormParametric.h"
#include "adapters/ExplicitModelAdapter.h"

namespace storm
{


void ParametricStormEntryPoint::createModel()
{
    std::shared_ptr<storm::models::AbstractModel < Polynomial>> model = storm::adapters::ExplicitModelAdapter<Polynomial>::translateProgram(mProgram, mConstants);
    model->printModelInformationToStream(std::cout);
}

void storm_parametric(const std::string& constants, const storm::prism::Program& program)
{
    ParametricStormEntryPoint entry(constants, program);
    entry.createModel();
}

}
