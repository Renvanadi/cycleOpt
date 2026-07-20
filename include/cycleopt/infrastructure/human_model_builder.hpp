#pragma once

#include <memory>
#include <string>
#include <vector>

#include <OpenSim/Simulation/Model/Model.h>

namespace cycleopt {

class HumanModelBuilder {
public:
    HumanModelBuilder();

    std::unique_ptr<OpenSim::Model> build() const;

private:
    std::string resolveModelPath(const std::string& relative_path) const;
    std::unique_ptr<OpenSim::Model> loadModel(const std::string& relative_path) const;
    void mergeModels(const std::vector<std::unique_ptr<OpenSim::Model>>& models, OpenSim::Model& target) const;
    void mergeBodies(const std::vector<std::unique_ptr<OpenSim::Model>>& models, OpenSim::Model& target,
                     std::unordered_map<std::string, OpenSim::PhysicalFrame*>& frame_map) const;
    void mergeJoints(const std::vector<std::unique_ptr<OpenSim::Model>>& models, OpenSim::Model& target,
                     const std::unordered_map<std::string, OpenSim::PhysicalFrame*>& frame_map) const;
    void mergeMuscles(const std::vector<std::unique_ptr<OpenSim::Model>>& models, OpenSim::Model& target,
                      const std::unordered_map<std::string, OpenSim::PhysicalFrame*>& frame_map) const;
    void mergeMarkers(const std::vector<std::unique_ptr<OpenSim::Model>>& models, OpenSim::Model& target,
                      const std::unordered_map<std::string, OpenSim::PhysicalFrame*>& frame_map) const;
    void mergeConstraints(const std::vector<std::unique_ptr<OpenSim::Model>>& models, OpenSim::Model& target,
                          const std::unordered_map<std::string, OpenSim::PhysicalFrame*>& frame_map) const;
    void repairReferences(OpenSim::Model& target,
                          const std::unordered_map<std::string, OpenSim::PhysicalFrame*>& frame_map) const;
    void finalizeModel(OpenSim::Model& target) const;
};

}  // namespace cycleopt
