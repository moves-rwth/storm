#pragma once

#include<memory>

namespace storm {
    
    
    template<typename EnvironmentType>
    class SubEnvironment {
    public:
        
        SubEnvironment() : subEnv(std::make_unique<EnvironmentType>()) {
            // Intentionally left empty
        }
        
        SubEnvironment(SubEnvironment const& other) : subEnv(new EnvironmentType(*other.subEnv)) {
            // Intentionally left empty
        }
        
        SubEnvironment<EnvironmentType>& operator=(SubEnvironment const& other) {
            subEnv = std::make_unique<EnvironmentType>(*other.subEnv);
            return *this;
        }
        
        EnvironmentType const& get() const {
            return *subEnv;
        }
        
        EnvironmentType& get() {
            return *subEnv;
        }
        
    private:
        std::unique_ptr<EnvironmentType> subEnv;
        
    };
}

