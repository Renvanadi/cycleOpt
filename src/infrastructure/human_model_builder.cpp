#include "cycleopt/infrastructure/human_model_builder.hpp"

#include <OpenSim/OpenSim.h>
#include <OpenSim/Simulation/Model/AbstractPathPoint.h>
#include <OpenSim/Simulation/Model/GeometryPath.h>
#include <OpenSim/Simulation/Model/Marker.h>
#include <OpenSim/Simulation/Model/Model.h>
#include <OpenSim/Simulation/Model/Muscle.h>
#include <OpenSim/Simulation/Model/PhysicalFrame.h>
#include <OpenSim/Simulation/Model/Point.h>
#include <OpenSim/Simulation/Model/Station.h>
#include <OpenSim/Simulation/Model/ModelVisualizer.h>
#include <OpenSim/Simulation/Wrap/WrapObjectSet.h>
#include <OpenSim/Simulation/Wrap/PathWrapPoint.h>
#include <OpenSim/Simulation/Wrap/WrapObject.h>
#include <OpenSim/Simulation/Wrap/PathWrap.h>
#include <OpenSim/Simulation/SimbodyEngine/Joint.h>
#include <OpenSim/Simulation/SimbodyEngine/PinJoint.h>
#include <OpenSim/Simulation/SimbodyEngine/WeldJoint.h>

#include <algorithm>
#include <cctype>
#include <filesystem>
#include <stdexcept>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

namespace cycleopt
{

    namespace
    {

        std::string normalizeName(const std::string &name)
        {
            std::string normalized = name;
            std::transform(normalized.begin(), normalized.end(), normalized.begin(), [](unsigned char c)
                           { return static_cast<char>(std::tolower(c)); });
            return normalized;
        }

        bool isLowerBodyComponent(const std::string &name)
        {
            const std::string normalized = normalizeName(name);
            return normalized.find("pelvis") != std::string::npos ||
                   normalized.find("femur") != std::string::npos ||
                   normalized.find("tibia") != std::string::npos ||
                   normalized.find("fibula") != std::string::npos ||
                   normalized.find("patella") != std::string::npos ||
                   normalized.find("foot") != std::string::npos ||
                   normalized.find("toe") != std::string::npos ||
                   normalized.find("hip") != std::string::npos ||
                   normalized.find("knee") != std::string::npos ||
                   normalized.find("ankle") != std::string::npos;
        }

        bool isUpperBodyComponent(const std::string &name)
        {
            const std::string normalized = normalizeName(name);
            return normalized.find("torso") != std::string::npos ||
                   normalized.find("trunk") != std::string::npos ||
                   normalized.find("clavicle") != std::string::npos ||
                   normalized.find("scap") != std::string::npos ||
                   normalized.find("humer") != std::string::npos ||
                   normalized.find("radius") != std::string::npos ||
                   normalized.find("ulna") != std::string::npos ||
                   normalized.find("shoulder") != std::string::npos ||
                   normalized.find("elbow") != std::string::npos ||
                   normalized.find("wrist") != std::string::npos ||
                   normalized.find("hand") != std::string::npos;
        }

        bool shouldPreferSource(const std::string &source_name, const std::string &current_name)
        {
            const std::string source = normalizeName(source_name);
            const std::string current = normalizeName(current_name);

            if (source.find("rajagopal") != std::string::npos && current.find("rajagopal") == std::string::npos)
            {
                return true;
            }
            if (current.find("rajagopal") != std::string::npos && source.find("rajagopal") == std::string::npos)
            {
                return false;
            }

            if (isLowerBodyComponent(source_name) && !isLowerBodyComponent(current_name))
            {
                return true;
            }
            if (isLowerBodyComponent(current_name) && !isLowerBodyComponent(source_name))
            {
                return false;
            }

            if (isUpperBodyComponent(source_name) && !isUpperBodyComponent(current_name))
            {
                return true;
            }
            if (isUpperBodyComponent(current_name) && !isUpperBodyComponent(source_name))
            {
                return false;
            }

            return source < current;
        }

        bool hasBody(const OpenSim::Model &model, const std::string &body_name)
        {
            try
            {
                (void)model.getBodySet().get(body_name);
                return true;
            }
            catch (...)
            {
                return false;
            }
        }

        bool hasJoint(const OpenSim::Model &model, const std::string &joint_name)
        {
            try
            {
                (void)model.getJointSet().get(joint_name);
                return true;
            }
            catch (...)
            {
                return false;
            }
        }

        // OpenSim 4.5: Model has no standalone getFrameSet(). Use findComponent instead.
        bool hasFrame(const OpenSim::Model &model, const std::string &frame_name)
        {
            return model.findComponent<OpenSim::Frame>(frame_name) != nullptr;
        }

        std::string preferredFrameName(const std::string &frame_name)
        {
            if (frame_name == "pelvis")
            {
                return "pelvis";
            }
            if (frame_name == "torso")
            {
                return "torso";
            }
            return frame_name;
        }

        // OpenSim 4.5: Joint has no getCoordinateSet(). "coordinates" is a list property,
        // accessed via get_coordinates(i) / upd_coordinates(i) / numCoordinates().
        bool hasCoordinate(const OpenSim::Joint &joint, const std::string &coordinate_name)
        {
            for (int i = 0; i < joint.numCoordinates(); ++i)
            {
                if (joint.get_coordinates(i).getName() == coordinate_name)
                {
                    return true;
                }
            }
            return false;
        }

        std::string uniqueCoordinateName(const OpenSim::Model &target, const std::string &base_name)
        {
            std::string candidate = base_name;
            int suffix = 0;
            while (true)
            {
                bool exists = false;
                for (int i = 0; i < target.getJointSet().getSize(); ++i)
                {
                    const auto &existing_joint = target.getJointSet().get(i);
                    if (hasCoordinate(existing_joint, candidate))
                    {
                        exists = true;
                        break;
                    }
                }
                if (!exists)
                {
                    return candidate;
                }
                candidate = base_name + "_" + std::to_string(++suffix);
            }
        }

        void reconnectPathPoints(const OpenSim::Muscle &source_muscle,
                                 OpenSim::Muscle &cloned_muscle,
                                 const std::unordered_map<std::string, OpenSim::PhysicalFrame *> &frame_map)
        {
            auto &cloned_path = cloned_muscle.updGeometryPath();
            const auto &source_path = source_muscle.getGeometryPath();

            // 通常の経由点: 名前はsource(壊れていない側)から取得し、cloned側に反映
            for (int i = 0; i < cloned_path.getPathPointSet().getSize(); ++i)
            {
                const auto &source_point = source_path.getPathPointSet().get(i);
                auto &cloned_point = cloned_path.updPathPointSet().get(i);
                const std::string parent_name = source_point.getParentFrame().getName();
                const auto it = frame_map.find(parent_name);
                if (it != frame_map.end() && it->second != nullptr)
                {
                    cloned_point.setParentFrame(*it->second);
                }
            }

            // WrapSet: wrap objectの所有Frame名もsource側から安全に取得する
            for (int i = 0; i < cloned_path.getWrapSet().getSize(); ++i)
            {
                const auto &source_wrap = source_path.getWrapSet().get(i);
                auto &cloned_wrap = cloned_path.updWrapSet().get(i);

                const OpenSim::WrapObject *source_wrap_object = source_wrap.getWrapObject();
                if (source_wrap_object == nullptr)
                {
                    continue;
                }

                std::string owner_name;
                try
                {
                    owner_name = source_wrap_object->getFrame().getName();
                }
                catch (const OpenSim::Exception &)
                {
                    continue;
                }

                const auto owner_it = frame_map.find(owner_name);
                if (owner_it == frame_map.end() || owner_it->second == nullptr)
                {
                    continue;
                }
                OpenSim::PhysicalFrame *owner_frame = owner_it->second;

                // source側とcloned側のPathWrapPointを対応する順序でペアにして処理
                auto source_points = source_wrap.getComponentList<OpenSim::PathWrapPoint>();
                auto cloned_points = cloned_wrap.updComponentList<OpenSim::PathWrapPoint>();

                auto cloned_it = cloned_points.begin();
                for (auto source_it = source_points.begin();
                     source_it != source_points.end() && cloned_it != cloned_points.end();
                     ++source_it, ++cloned_it)
                {
                    auto &cloned_wrap_point = *cloned_it;
                    const auto &socket_names = cloned_wrap_point.getSocketNames();
                    if (std::find(socket_names.begin(), socket_names.end(), "parent_frame") == socket_names.end())
                    {
                        continue;
                    }
                    cloned_wrap_point.updSocket<OpenSim::PhysicalFrame>("parent_frame").connect(*owner_frame);
                }
            }
        }
    } // namespace

    HumanModelBuilder::HumanModelBuilder() = default;

    std::string HumanModelBuilder::resolveModelPath(const std::string &relative_path) const
    {
        const std::filesystem::path project_root = std::filesystem::current_path().parent_path(); // Assuming the executable is in a subdirectory of the project root
        const std::vector<std::filesystem::path> candidates = {
            project_root / "data" / relative_path,
            project_root / relative_path,
            std::filesystem::path(relative_path),
        };

        for (const auto &candidate : candidates)
        {
            if (std::filesystem::exists(candidate))
            {
                return std::filesystem::absolute(candidate).string();
            }
        }

        throw std::runtime_error("Unable to resolve model path: " + relative_path);
    }

    std::unique_ptr<OpenSim::Model> HumanModelBuilder::loadModel(const std::string &relative_path) const
    {
        const std::string resolved = resolveModelPath(relative_path);
        auto model = std::make_unique<OpenSim::Model>(resolved);
        model->setName(std::filesystem::path(relative_path).stem().string());
        OpenSim::ModelVisualizer::addDirToGeometrySearchPaths(std::filesystem::path(resolved).parent_path().string());

        // Socket解決(PathWrapのwrap objectキャッシュなど)を確定させるため、
        // マージ処理で参照する前に明示的にfinalizeしておく。
        model->finalizeFromProperties();
        model->finalizeConnections();
        return model;
    }

    std::unique_ptr<OpenSim::Model> HumanModelBuilder::build() const
    {
        std::vector<std::unique_ptr<OpenSim::Model>> models;
        models.push_back(loadModel("Rajagopal2016/Rajagopal2016.osim"));
        models.push_back(loadModel("BUET/BUET.osim"));

        std::cout << "Loaded " << models.size() << " human models for merging.\n";
        std::cout << "models[0]: " << models[0]->getName() << "\n";
        std::cout << "models[1]: " << models[1]->getName() << "\n";

        auto merged = std::make_unique<OpenSim::Model>();
        merged->setName("cycleOpt_human_model");

        mergeModels(models, *merged);
        return merged;
    }

    void HumanModelBuilder::mergeModels(const std::vector<std::unique_ptr<OpenSim::Model>> &models, OpenSim::Model &target) const
    {
        if (models.empty())
        {
            return;
        }

        std::unordered_map<std::string, OpenSim::PhysicalFrame *> frame_map;
        mergeBodies(models, target, frame_map);
        mergeJoints(models, target, frame_map);
        mergeMuscles(models, target, frame_map);
        // mergeMarkers(models, target, frame_map);
        // mergeConstraints(models, target, frame_map);
        // repairReferences(target, frame_map);
        // finalizeModel(target);
    }

    void HumanModelBuilder::mergeBodies(const std::vector<std::unique_ptr<OpenSim::Model>> &models, OpenSim::Model &target,
                                        std::unordered_map<std::string, OpenSim::PhysicalFrame *> &frame_map) const
    {
        for (const auto &model : models)
        {
            for (int i = 0; i < model->getBodySet().getSize(); ++i)
            {
                const auto &body = model->getBodySet().get(i);
                const std::string preferred = preferredFrameName(body.getName());
                if (hasBody(target, preferred))
                {
                    // updBodySet() gives a non-const set so we can take a non-const pointer.
                    frame_map[body.getName()] = &target.updBodySet().get(preferred);
                    continue;
                }
                auto *cloned_body = body.clone();
                cloned_body->setName(preferred);
                target.addBody(cloned_body);
                frame_map[body.getName()] = &target.updBodySet().get(preferred);
            }
        }
    }

    void HumanModelBuilder::mergeJoints(const std::vector<std::unique_ptr<OpenSim::Model>> &models, OpenSim::Model &target,
                                        const std::unordered_map<std::string, OpenSim::PhysicalFrame *> &frame_map) const
    {
        for (const auto &model : models)
        {
            for (int i = 0; i < model->getJointSet().getSize(); ++i)
            {
                const auto &joint = model->getJointSet().get(i);
                if (hasJoint(target, joint.getName()))
                {
                    continue;
                }

                auto *cloned_joint = joint.clone();
                cloned_joint->setName(joint.getName());

                if (auto *base_joint = dynamic_cast<OpenSim::Joint *>(cloned_joint))
                {
                    const auto &original_parent = joint.getParentFrame();
                    const auto &original_child = joint.getChildFrame();
                    const auto parent_it = frame_map.find(original_parent.getName());
                    const auto child_it = frame_map.find(original_child.getName());
                    if (parent_it != frame_map.end() && parent_it->second != nullptr)
                    {
                        base_joint->updSocket<OpenSim::PhysicalFrame>("parent_frame").connect(*parent_it->second);
                    }
                    if (child_it != frame_map.end() && child_it->second != nullptr)
                    {
                        base_joint->updSocket<OpenSim::PhysicalFrame>("child_frame").connect(*child_it->second);
                    }

                    // OpenSim 4.5: coordinates are a list property; index via
                    // get_coordinates(i) / upd_coordinates(i), not getCoordinate(i).
                    if (base_joint->numCoordinates() > 0)
                    {
                        const std::string coordinate_name =
                            uniqueCoordinateName(target, base_joint->get_coordinates(0).getName());
                        if (coordinate_name != base_joint->get_coordinates(0).getName())
                        {
                            base_joint->upd_coordinates(0).setName(coordinate_name);
                        }
                    }
                }

                target.addJoint(cloned_joint);
            }
        }
    }

    void HumanModelBuilder::mergeMuscles(const std::vector<std::unique_ptr<OpenSim::Model>> &models, OpenSim::Model &target,
                                         const std::unordered_map<std::string, OpenSim::PhysicalFrame *> &frame_map) const
    {
        std::unordered_map<std::string, std::pair<std::string, std::string>> force_choices;
        for (const auto &model : models)
        {
            for (int i = 0; i < model->getForceSet().getSize(); ++i)
            {
                const auto &force = model->getForceSet().get(i);
                const std::string normalized = normalizeName(force.getName());
                const auto existing_it = force_choices.find(normalized);
                if (existing_it != force_choices.end())
                {
                    const std::string current_identity = model->getName() + "|" + force.getName();
                    const std::string existing_identity = existing_it->second.first + "|" + existing_it->second.second;
                    if (!shouldPreferSource(current_identity, existing_identity))
                    {
                        continue;
                    }
                }
                force_choices[normalized] = {model->getName(), force.getName()};

                if (const auto *source_muscle = dynamic_cast<const OpenSim::Muscle *>(&force))
                {
                    auto *cloned_force = force.clone();
                    target.addForce(cloned_force);
                    if (auto *cloned_muscle = dynamic_cast<OpenSim::Muscle *>(cloned_force))
                    {
                        reconnectPathPoints(*source_muscle, *cloned_muscle, frame_map);
                    }
                }
            }
        }
    }

    void HumanModelBuilder::mergeMarkers(const std::vector<std::unique_ptr<OpenSim::Model>> &models, OpenSim::Model &target,
                                         const std::unordered_map<std::string, OpenSim::PhysicalFrame *> &frame_map) const
    {
        std::unordered_map<std::string, std::string> marker_choices;
        for (const auto &model : models)
        {
            for (int i = 0; i < model->getMarkerSet().getSize(); ++i)
            {
                const auto &marker = model->getMarkerSet().get(i);
                const std::string normalized = normalizeName(marker.getName());
                const auto existing_it = marker_choices.find(normalized);
                if (existing_it != marker_choices.end())
                {
                    if (!shouldPreferSource(marker.getName(), existing_it->second))
                    {
                        continue;
                    }
                }
                marker_choices[normalized] = marker.getName();
                auto *cloned_marker = marker.clone();
                const auto parent_it = frame_map.find(marker.getParentFrameName());
                if (parent_it != frame_map.end() && parent_it->second != nullptr)
                {
                    cloned_marker->setParentFrameName(parent_it->second->getName());
                }
                target.addMarker(cloned_marker);
            }
        }
    }

    void HumanModelBuilder::mergeConstraints(const std::vector<std::unique_ptr<OpenSim::Model>> &models, OpenSim::Model &target,
                                             const std::unordered_map<std::string, OpenSim::PhysicalFrame *> &frame_map) const
    {
        (void)frame_map;
        for (const auto &model : models)
        {
            for (int i = 0; i < model->getConstraintSet().getSize(); ++i)
            {
                const auto &constraint = model->getConstraintSet().get(i);

                // OpenSim 4.5: Constraint has no getBodySet(). Body/frame references
                // are exposed as Sockets, so we inspect connectee paths generically.
                bool can_add = true;
                for (const std::string &socket_name : constraint.getSocketNames())
                {
                    const auto &socket = constraint.getSocket(socket_name);
                    const std::string connectee_path = socket.getConnecteePath();
                    const std::string leaf_name = std::filesystem::path(connectee_path).filename().string();
                    if (!leaf_name.empty() && !hasBody(target, leaf_name) && !hasFrame(target, leaf_name))
                    {
                        can_add = false;
                        break;
                    }
                }
                if (!can_add)
                {
                    continue;
                }
                auto *cloned_constraint = constraint.clone();
                target.addConstraint(cloned_constraint);
            }
        }
    }

    void HumanModelBuilder::repairReferences(OpenSim::Model &target,
                                             const std::unordered_map<std::string, OpenSim::PhysicalFrame *> &frame_map) const
    {
        (void)frame_map;
        target.finalizeConnections();
    }

    void HumanModelBuilder::finalizeModel(OpenSim::Model &target) const
    {
        target.finalizeFromProperties();
        target.finalizeConnections();
    }

} // namespace cycleopt