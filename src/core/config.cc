/*
 * PROJECT : TUNA
 * FILE    : config.cc
 * AUTHOR  : bitofux
 * DATE    : 2026-06-10
 * BRIEF   : tuna 配置模块源文件
 */
#include "tuna/core/config.hpp"

#include <cctype>
#include <cstdlib>
#include <fstream>
#include <string>

namespace tuna {
namespace core {

/*
 * Trim
 *
 * @brief 去除字符串两端空格
 * @param text 常引用的std::string对象
 * @return 去除字符串两端空格后的std::string对象
 */
std::string Config::Trim(const std::string& text) {
    // 设置初始字符下标
    std::string::size_type begin = 0;

    // 从字符串下标为0开始循环判断当前下标的字符是否为空白字符
    // 若不是则退出循环，此时begin是有效字符的下标
    while ((begin < text.size()) && std::isspace(static_cast<unsigned char>(text[begin]))) {
        // 若是则下标自增1
        ++begin;
    }

    // 设置尾部字符下标
    std::string::size_type end = text.size();

    // 从字符串尾部下标开始循环判断当前下标的字符是否为空白字符
    // 若不是则退出循环，此时begin是有效字符的下标
    while (end > begin && std::isspace(static_cast<unsigned char>(text[end - 1]))) {
        --end;
    }

    // 此时begin是有效字符的起始下标，end - begin + 1是有效字符个数(包括begin)
    return text.substr(begin, end - begin);
}

/*
 * Load
 *
 * @brief 读取文件，并将读取到的所有配置项存储到items_
 * @param files 指向配置文件对象的常string引用
 * @return 成功为true，失败为false
 */
bool Config::Load(const std::string& file) {
    // 若files里为空，则失败
    if (file.empty()) {
        return false;
    }

    // 打开这个文件，并判断是否打开成功
    std::ifstream is{file};
    if (!is.is_open()) {
        return false;
    }

    // 清空容器
    items_.clear();

    // 定义保存节区数据的string对象
    std::string section;
    // 保存当前行的数据
    std::string line;

    // 使用getline获取文件中的一行数据，默认'\n'作为终止符
    while (std::getline(is, line, '\n')) {
        // 去除这一行字符数据两边的空格字符
        line = Trim(line);

        // 若去除两边空格字符的string对象中的字符个数为0 则继续读取下一行
        if (line.empty()) {
            continue;
        }

        // 若string中的第一个字符为'#'，继续读取下一行
        if (line.front() ==  '#') {
            continue;
        }

        // 若读取到的一行中的数据是节区数据
        if (line.front() == '[' && line.back() == ']') {
            section = Trim(line.substr(1, line.size() - 2));
            continue;
        }

        // 获取字符'='的下标
        std::string::size_type pos = line.find('=');
        if (pos == std::string::npos) {
            continue;
        }

        // 获取非节区的key和value
        std::string key = Trim(line.substr(0, pos));
        std::string value = Trim(line.substr(pos + 1));

        // 判断key是否为空
        if (key.empty()) {
            continue;
        }

        // 判断section是否为空，不为空则将section与字符.和key进行拼接
        if (!section.empty()) {
            key = section + "." + key;
        }

        // 插入数据到容器
        items_[key] = value;
    }

    return true;
}
/*
 * GetString
 *
 * @brief 获取配置项的整型数据
 * @param key 要查找的string对象
 * @param default_value 找不到即使用默认值
 * @return 整型数据
 */
int Config::GetInt(const std::string& key, int default_value) const {
    // 执行容器查找
    const auto iter = items_.find(key);

    // 若找不到，则返回默认值
    if (iter == items_.end()) {
        return default_value;
    }

    // 找到了，执行字符串到整数的转换啊
    char* end = nullptr;

    // 按照十进制的形式转换，遇到不是数字的字符，则保存到end中
    const long value = std::strtol(iter->second.c_str(), &end, 10);

    /*
     * 1. 若end等于iter->second.c_str()，等于第一个就不是数字，是非法字符
     * 2. 若end等于'\0'，则表示要转换的字符串读取成功，指针一直走到了末尾遇到了'\0'才停止，前面
     *    的有效数字字符得到了正确的转换啊
     * 3. 若end > iter->second.c_str()且*end != '\0'则在解析字符串过程中，还未到'\0'
     *    就遇到了错误
     * 参考使用stoi也可
     */
    if (end == iter->second.c_str() || *end != '\0') {
        return default_value;
    }

    return static_cast<int>(value);
}

/*
 * GetString
 *
 * @brief 获取配置项的字符串数据
 * @param key 要查找的string对象
 * @param default_value 若查不到，则返回此值
 * @return 返回的字符串数据
 */
std::string Config::GetString(const std::string& key, const std::string& default_value) const {
    // 执行容器的key查找
    const auto iter = items_.find(key);
    if (iter == items_.end()) {
        return default_value;
    }

    return iter->second;
}

/*
 * GetBool
 *
 * @brief 获取配置项的bool值
 * @param key 要查找的string对象
 * @param default_value 若查不到，则返回此值
 * @return 返回的bool数据
 */
bool Config::GetBool(const std::string& key, bool default_value) const {
    // 执行容器的key查找
    const auto iter = items_.find(key);
    if (iter == items_.end()) {
        return default_value;
    }

    const std::string value = Trim(iter->second);
    // 判断获取的值的信息
    if (value == "true" || value == "1" || value == "yes" || value == "on") {
        return true;
    }

    if (value == "false" || value == "0" || value == "no" || value == "off") {
        return false;
    }

    return default_value;
}

/*
 * Has
 *
 * @brief 判断配置项是否存在
 * @param key 要查找的string对象
 * @return 存在为true，不存在为false
 */
bool Config::Has(const std::string& key) const { return items_.find(key) != items_.end(); }

}  // namespace core
}  // namespace tuna
