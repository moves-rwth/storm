#include "../models/ModelBase.h"
#include "prism/Program.h"


namespace storm {
    namespace storage {
        struct ModelProgramPair {
            std::shared_ptr<storm::models::ModelBase> model;
            storm::prism::Program program;
        };
    }
}