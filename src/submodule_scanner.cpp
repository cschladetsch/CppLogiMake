#include <filesystem>
#include <fstream>
#include <string>
#include <vector>
#include <regex>

namespace fs = std::filesystem;

struct SubmoduleInfo {
    std::string name;
    std::string path;
    std::string url;
};

inline std::vector<SubmoduleInfo> DiscoverSubmodules(const fs::path& repoRoot) {
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
        }
    }
    if (!current.path.empty()) {
        submodules.push_back(current);
    }

    return submodules;
}

