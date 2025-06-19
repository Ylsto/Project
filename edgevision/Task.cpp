#include"Task.h"
#include <sstream>
#include<iostream>
#include <iomanip>

using namespace nlohmann;

template<typename T>
T GetValue(const nlohmann::json& data)
{
    if (data.is_string())
    {
        std::string value = data.get<std::string>();
        if (value.empty())
            return T();

        if constexpr (std::is_same_v<T, std::string>)
            return value;
        else  if constexpr (std::is_floating_point_v<T>)
            return static_cast<T>(std::stod(value));
        else
            return static_cast<T>(std::stoll(value));
    }
    else if (data.is_number() || data.is_boolean())
    {
        if constexpr (std::is_same_v<T, std::string>)
        {
            if (data.is_number_float())
                return std::to_string(data.get<double>());
            else if (data.is_boolean())
                return std::to_string(data.get<bool>());
            else
                return std::to_string(data.get<long long>());
        }
        else
        {
            if (data.is_number_float())
                return static_cast<T>(data.get<double>());
            else if (data.is_boolean())
                return static_cast<T>(data.get<bool>());
            else
                return static_cast<T>(data.get<long long>());
        }
    }
    else
        return T();
}

bool CCmdData::AnalyzeData(nlohmann::json& data)
{
    m_uCmd = GetValue<decltype(m_uCmd)>(data["CMD"]);
    m_nType = GetValue<decltype(m_nType)>(data["TYPE"]);
    m_ulSeq = GetValue<decltype(m_ulSeq)>(data["SEQ"]);
    m_nLastStatus = GetValue<decltype(m_nLastStatus)>(data["STATUS"]);

    return false;
}


// 生成作业请求的 JSON 字符串
std::string ToJsonString() const override;  
 // 解析作业回应命令
bool AnalyzeData(nlohmann::json& data) override;  

// 解析作业状态反馈命令
bool AnalyzeData(nlohmann::json& data) override; 

// 生成任务控制请求的 JSON 字符串
std::string ToJsonString() const override; 
// 解析任务控制回应命令
bool AnalyzeData(nlohmann::json& data) override;  

 // 解析仓储作业完成通知请求
bool AnalyzeData(nlohmann::json& data) override;
{


}

 // 生成仓储作业完成通知回应的 JSON 字符串       
std::string ToJsonString() const override;        