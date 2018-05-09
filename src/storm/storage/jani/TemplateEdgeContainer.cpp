#include "storm/storage/jani/TemplateEdgeContainer.h"
#include "storm/storage/jani/TemplateEdge.h"

namespace storm {
    namespace jani {
        TemplateEdgeContainer::TemplateEdgeContainer(TemplateEdgeContainer const &other) {
            for (auto const& te : other) {
                this->insert(std::make_shared<TemplateEdge>(*te));
            }
        }
    }
}