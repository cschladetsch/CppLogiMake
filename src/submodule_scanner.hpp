
#pragma once

#include <filesystem>
#include <fstream>
#include <string>
#include <vector>
#include <regex>

namespace LogiMake {

namespace fs = std::filesystem;

struct SubmoduleInfo {
    std::string name;
    std::string path;
    std::string url;
};

class SubmoduleScanner {
public:
    static std::vector<SubmoduleInfo> Discover(const fs::path& repoRoot) {
        std::vector<SubmoduleInfo> submodules;
        fs::path gitModulesPath = repoRoot / ".gitmodules";
        
        if (!fs::exists(gitModulesPath)) {
            return submodules;
        }

        std::ifstream file(gitModulesPath);
        std::string line;
        SubmoduleInfo current;
        
        std::regex pathRegex(R"(^\s*path\s*=\s*(.+$))");
        std::regex nameRegex(R"(^\[submodule\s+"(.+)"\])");
        std::smatch match;

        while (std::getline(file, line)) {
            if (std::regex_search(line, match, nameRegex)) {
                if (!current.path.empty()) {
                    submodules.push_back(current);
                    current = {};
                }
                current.name = match[1];
            } else if (std::regex_search(line, match, pathRegex)) {
                current.path = match[1];
                current.path.erase(current.path.find_last_not_of(" \n\r\t") + 1);
            }
        }
        if (!current.path.empty()) {
            submodules.push_back(current);
        }

        return submodules;
    }

    static std::vector<fs::path> GetIncludeDirectories(const fs::path& repoRoot) {
        std::vector<fs::path> includeDirs;
        auto submodules = Discover(repoRoot);
        
        for (const auto& sub : submodules) {
            fs::path subPath = repoRoot / sub.path;
            
            // Handle specific library structures (e.g., Googletest layout)
            fs::path gtestInclude = subPath / "googletest" / "include";
            fs::path standardInclude = subPath / "include";
            
            if (fs::exists(gtestInclude)) {
                includeDirs.push_back(gtestInclude);
            }
            if (fs::exists(standardInclude) && standardInclude != gtestInclude) {
                includeDirs.push_back(standardInclude);
            }
            // Fallback to the submodule root if no explicit include folder is found
            if (!fs::exists(gtestInclude) && !fs::exists(standardInclude) && fs::exists(subPath)) {
                includeDirs.push_back(subPath);
            }
        }
        
        return includeDirs;
    }
};

} // namespace LogiMake
