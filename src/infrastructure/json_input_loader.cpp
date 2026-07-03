#include "cycleopt/infrastructure/json_input_loader.hpp"

#include <fstream>
#include <sstream>
#include <string>
#include <cctype>

namespace cycleopt {

namespace {

std::string trim(const std::string& text) {
    size_t start = 0;
    while (start < text.size() && std::isspace(static_cast<unsigned char>(text[start]))) {
        start++;
    }
    size_t end = text.size();
    while (end > start && std::isspace(static_cast<unsigned char>(text[end - 1]))) {
        end--;
    }
    return text.substr(start, end - start);
}

std::string extractBlock(const std::string& source, const std::string& key) {
    const std::string needle = '"' + key + '"';
    size_t pos = source.find(needle);
    if (pos == std::string::npos) {
        return {};
    }
    pos = source.find('{', pos);
    if (pos == std::string::npos) {
        return {};
    }
    int depth = 1;
    size_t start = pos + 1;
    for (size_t i = start; i < source.size(); ++i) {
        if (source[i] == '{') {
            depth++;
        } else if (source[i] == '}') {
            depth--;
            if (depth == 0) {
                return source.substr(start, i - start);
            }
        }
    }
    return {};
}

std::string findValue(const std::string& source, const std::string& key) {
    const std::string needle = '"' + key + '"';
    size_t pos = source.find(needle);
    if (pos == std::string::npos) {
        return {};
    }
    pos = source.find(':', pos + needle.size());
    if (pos == std::string::npos) {
        return {};
    }
    size_t valueStart = pos + 1;
    while (valueStart < source.size() && std::isspace(static_cast<unsigned char>(source[valueStart]))) {
        ++valueStart;
    }
    if (valueStart >= source.size()) {
        return {};
    }
    if (source[valueStart] == '"') {
        size_t end = source.find('"', valueStart + 1);
        if (end == std::string::npos) {
            return {};
        }
        return source.substr(valueStart + 1, end - valueStart - 1);
    }
    size_t end = valueStart;
    while (end < source.size() && source[end] != ',' && source[end] != '}' && !std::isspace(static_cast<unsigned char>(source[end]))) {
        ++end;
    }
    return source.substr(valueStart, end - valueStart);
}

std::string parseStringValue(const std::string& source, const std::string& key, std::string defaultValue) {
    const std::string value = trim(findValue(source, key));
    return value.empty() ? defaultValue : value;
}

double parseDoubleValue(const std::string& source, const std::string& key, double defaultValue) {
    const std::string value = trim(findValue(source, key));
    if (value.empty()) {
        return defaultValue;
    }
    try {
        return std::stod(value);
    } catch (...) {
        return defaultValue;
    }
}

int parseIntValue(const std::string& source, const std::string& key, int defaultValue) {
    const std::string value = trim(findValue(source, key));
    if (value.empty()) {
        return defaultValue;
    }
    try {
        return std::stoi(value);
    } catch (...) {
        return defaultValue;
    }
}

Point3D parsePoint3D(const std::string& source, const std::string& key, Point3D defaultValue) {
    const std::string block = extractBlock(source, key);
    if (block.empty()) {
        return defaultValue;
    }
    Point3D point;
    point.x = parseDoubleValue(block, "x", defaultValue.x);
    point.y = parseDoubleValue(block, "y", defaultValue.y);
    point.z = parseDoubleValue(block, "z", defaultValue.z);
    return point;
}

}  // namespace

SimulationScenario JsonInputLoader::load(const std::string& file_path) const {
    std::ifstream input(file_path);
    std::ostringstream buffer;
    buffer << input.rdbuf();
    const std::string content = buffer.str();

    SubjectModel subject(
        parseStringValue(content, "name", "unknown"),
        parseDoubleValue(content, "body_mass_kg", 70.0),
        parseDoubleValue(content, "height_m", 1.75),
        parseDoubleValue(content, "seated_height_m", 0.9),
        parseDoubleValue(content, "forearm_length_m", 0.3),
        parseDoubleValue(content, "upper_arm_length_m", 0.3),
        parseDoubleValue(content, "thigh_length_m", 0.45),
        parseDoubleValue(content, "shank_length_m", 0.45),
        parseDoubleValue(content, "neck_length_m", 0.15)
    );

    const std::string bicycleBlock = extractBlock(content, "bicycle");
    BicycleGeometry bicycle(
        parseDoubleValue(bicycleBlock, "crank_length_m", 0.17),
        parseDoubleValue(bicycleBlock, "saddle_height_m", 0.85),
        parseDoubleValue(bicycleBlock, "handlebar_height_m", 1.0),
        parsePoint3D(bicycleBlock, "saddle_position", Point3D{0.0, 0.0, 0.0}),
        parsePoint3D(bicycleBlock, "bottom_bracket_position", Point3D{0.0, 0.0, 0.0}),
        parsePoint3D(bicycleBlock, "handlebar_position", Point3D{0.0, 0.0, 0.0}),
        parseDoubleValue(bicycleBlock, "q_factor_m", 0.15),
        parseDoubleValue(bicycleBlock, "handlebar_width_m", 0.4)
    );

    const std::string cyclingConditionBlock = extractBlock(content, "cycling_condition");
    CyclingCondition condition(
        parseDoubleValue(cyclingConditionBlock, "power_watts", 200.0),
        parseDoubleValue(cyclingConditionBlock, "cadence_rpm", 90.0),
        parseDoubleValue(cyclingConditionBlock, "speed_mps", 10.0)
    );

    const std::string ridingStyleBlock = extractBlock(content, "riding_style");
    RidingStyle style(
        static_cast<RidingStyleType>(parseIntValue(ridingStyleBlock, "type", 0)),
        parseDoubleValue(ridingStyleBlock, "muscle_weight", 1.0),
        parseDoubleValue(ridingStyleBlock, "metabolic_weight", 1.0),
        parseDoubleValue(ridingStyleBlock, "volume_weight", 1.0)
    );

    return SimulationScenario(subject, bicycle, condition, style);
}

}  // namespace cycleopt
