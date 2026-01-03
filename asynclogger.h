#pragma once

#include <cstdint>
#include <memory>
#include <string>
#include <iosfwd>
#include <type_traits>

namespace zephyrlog
{

enum class LogLevel : uint8_t
{
    DEBUG = 0,
    INFO,
    WARN,
    ERROR,
    CRITICAL
};

const char* toString(LogLevel level);

class LogLine
{
public:
    LogLine(LogLevel level, const char* file, const char* function, uint32_t line);
    ~LogLine();

    LogLine(LogLine&&) = default;
    LogLine& operator=(LogLine&&) = default;

    LogLine(const LogLine&) = delete;
    LogLine& operator=(const LogLine&) = delete;

    //格式化输出到流
    void stringify(std::ostream& os);

    //重载
    LogLine& operator<<(char arg);
    LogLine& operator<<(int32_t arg);
    LogLine& operator<<(uint32_t arg);
    LogLine& operator<<(int64_t arg);
    LogLine& operator<<(uint64_t arg);
    LogLine& operator<<(double arg);
    LogLine& operator<<(const std::string& arg);

    // 字符串字面量（编译期常量，不需要拷贝）
    template<size_t N>
    LogLine& operator<<(const char (&arg)[N])
    {
        encode(string_literal_t(arg));
        return *this;
    }

    // const char* 指针
    template<typename Arg>
    typename std::enable_if<std::is_same<Arg, const char*>::value, LogLine&>::type
    operator<<(const Arg& arg)
    {
        encode(arg);
        return *this;
    }

    // char* 指针
    template<typename Arg>
    typename std::enable_if<std::is_same<Arg, char*>::value, LogLine&>::type
    operator<<(const Arg& arg)
    {
        encode(arg);
        return *this;
    }

    // 字符串字面量包装类型
    struct stringLiteral
    {
        explicit stringLiteral(const char* str) : m_str(str) {}
        const char* m_str;
    };

private:
    //获取当前写入位置
    char* buffer();

    //编码函数
    template<typename T>
    void encode(T arg);

    template<typename T>
    void encode(T arg, uint8_t typeId);

    void encode(char* arg);
    void encode(const char* arg);
    void encode(stringLiteral arg);
    void encodeCString(const char* arg, size_t length);

    //缓冲区管理
    void resizeBufferIfNeeded(size_t additionalBytes);
    // 反序列化（解码）
    void stringify(std::ostream& os, char* start, const char* end);

private:
    size_t m_bytesUsed;
    size_t m_bufferSize;
    std::unique_ptr<char[]> m_heapBuffer;
    char m_stackBuffer[256 - 2 * sizeof(size_t) - sizeof(decltype(m_heapBuffer))];
};

}