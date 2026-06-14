/*
 * PROJECT : TUNA
 * FILE    : config.hpp
 * AUTHOR  : bitofux
 * DATE    : 2026-06-10
 * BRIEF   : tuna 配置模块头文件
 */

#ifndef CONFIG_HPP
#define CONFIG_HPP

#include <unordered_map>
#include <string>

namespace tuna {
namespace core {
class Config {
public:
    Config() = default;
    ~Config() = default;

    // 禁止拷贝和赋值
    Config(const Config&) = delete;
    Config& operator=(const Config&) = delete;

    bool Load(const std::string& files);
    int GetInt(const std::string& key, int default_value) const;
    std::string GetString(const std::string& key, const std::string& default_value) const;
    bool GetBool(const std::string& key, bool default_value) const;
    bool Has(const std::string& key) const;

    std::unordered_map<std::string, std::string> items_;
private:
    static std::string Trim(const std::string& text);

};
}  // namespace core
}  // namespace tuna

#endif  // CONFIG_HPP
