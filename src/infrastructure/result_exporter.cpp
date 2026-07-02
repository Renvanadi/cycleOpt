#include "cycleopt/infrastructure/result_exporter.hpp"

#include <fstream>

namespace cycleopt {

void ResultExporter::exportResult(const std::string& output_path, const std::string& content) const {
    std::ofstream output(output_path);
    output << content;
}

}  // namespace cycleopt
