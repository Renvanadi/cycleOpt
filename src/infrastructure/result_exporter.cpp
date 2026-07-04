#include "cycleopt/infrastructure/result_exporter.hpp"
#include <fstream>
#include <iostream>

namespace cycleopt {

void ResultExporter::exportResult(const std::string& path, const std::string& result) const {
    std::ofstream outfile(path);
    if (outfile.is_open()) {
        outfile << result;
        outfile.close();
        std::cout << "Results exported to " << path << "\n";
    } else {
        std::cerr << "Failed to open " << path << " for exporting results.\n";
    }
}

// In a real implementation, we would add methods to export specific analysis data
// e.g., muscle energy consumption CSVs for plotting.

}  // namespace cycleopt
