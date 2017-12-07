#ifndef moxie_UTILSTRING_H
#define moxie_UTILSTRING_H
#include <vector>
#include <string>

namespace moxie {

namespace utils {

bool StringSplitBychar(const std::string& src, const char delimter,
                       std::vector<std::string>& ret, std::size_t startpos);

std::string StringTrimLeft(const std::string& src);
std::string StringTrimRight(const std::string& src);
std::string StringTrim(const std::string& src);
std::string StringRemoveRepeatChar(const std::string& src, const std::string& op);
std::string StringGetSuffix(const std::string& src);
}

}

#endif //MOXIE_UTILSTRING_H
