#pragma once

#include <memory>

namespace storm {

template<typename EnvironmentType>
class SubEnvironment {
   public:
    SubEnvironment();
    SubEnvironment(SubEnvironment const& other);
    SubEnvironment<EnvironmentType>& operator=(SubEnvironment const& other);
    EnvironmentType const& get() const;
    EnvironmentType& get();

   private:
    void assertInitialized() const;
    mutable std::unique_ptr<EnvironmentType> subEnv;
};
}  // namespace storm
