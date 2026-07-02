#pragma once

#include <string>

namespace cycleopt {

class ResultExporter {
public:
    void exportResult(const std::string& output_path, const std::string& content) const;
};

}  // namespace cycleopt
