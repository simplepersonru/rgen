#ifndef EXTRACTEDDATA_H
#define EXTRACTEDDATA_H

#include <clang/AST/Decl.h>
#include <filesystem>
#include <map>
#include <string>
#include <vector>

namespace rgen
{

struct ExtractedData
{
    std::vector<const clang::Decl*> declList;
    std::filesystem::path fullHeaderPath;

    std::map<std::string, std::vector<const clang::Decl*>> commandsMap;

    bool hasCommand(const std::string& command) const
    {
        return commandsMap.find(command) != commandsMap.end();
    }
};

} // namespace rgen

#endif // EXTRACTEDDATA_H
