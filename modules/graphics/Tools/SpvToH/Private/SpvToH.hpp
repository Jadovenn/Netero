/**
 * Netero sources under BSD-3-Clause
 * see LICENSE.txt
 */

#pragma once

#include <fstream>
#include <string>
#include <vector>

namespace SpvToH {

void        PrintNeteroHeader(std::ofstream& aFile);
void        PrintHeaderGuardPrefix(std::ofstream& aFile, const std::string& aName);
void        PrintHeaderGuardSuffix(std::ofstream& aFile, const std::string& aName);
std::string ExtractFileNameWithoutExtension(const std::string& aFilePath);
void        ExtractFileDataAsChar(std::ifstream& aFile, std::vector<char>& someDataOut);
unsigned    ExtractArrayCount(std::vector<char>& someData);
void        EmitSymbols(std::ofstream&     aFile,
                        const std::string& aSymbolSuffix,
                        std::vector<char>& someData,
                        unsigned           aWordCount);

} // namespace SpvToH
