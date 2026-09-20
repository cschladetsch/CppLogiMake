#include "submodule-scanner.hpp"
#include <ostream>

namespace LogiMake {

void EmitTargetCMake(std::ostream& out, const std::string& targetName, const std::vector<std::string>& sources, const fs::path& repoRoot) {
    // Write standard executable/library definition
    out << "add_executable(" << targetName;
    for (const auto& src : sources) {
        out << " " << src;
    }
    out << ")\n\n";
    
    // Automatically inject discovered submodule include paths
    auto submoduleIncludes = SubmoduleScanner::GetIncludeDirectories(repoRoot);
    if (!submoduleIncludes.empty()) {
        out << "# Automatically injected submodule include directories\n";
        out << "target_include_directories(" << targetName << " PRIVATE\n";
        for (const auto& inc : submoduleIncludes) {
            out << "    \"" << fs::absolute(inc).generic_string() << "\"\n";
        }
        out << ")\n\n";
    }
}

} // namespace LogiMake

