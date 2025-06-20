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
std::string CartTransportJob::ToJsonString() const
{
    nlohmann::json tempSend = {{"CMD", m_uCmd},
                                {"TYPE", m_nType},
                                {"SEQ", m_ulSeq},
                                {"STATUS", m_nLastStatus},
                                {"ZEOF",""}};
    tempSend["TaskID"] = m_strTaskID; // 添加 TaskID 字段
    tempSend["TaskType"] = m_nkType; // 添加 TaskType 字段
    tempSend["StartPos"] = m_strStartPos; // 添加 StartPos 字段
    tempSend["EndPos"] = m_strEndPos; // 添加 EndPos 字段
    tempSend["StayTime"] = m_nStayTime; // 添加 StayTime 字段
    tempSend["Num"] = m_nNum; // 添加 Num 字段

    return tempSend.dump(); // 使用 dump() 方法将 JSON 对象转换为字符串
}  
 // 解析作业回应命令
bool CartTransportJob::AnalyzeData(nlohmann::json& fields) 
{
    CCmdData::AnalyzeData(fields);

    if(fields.contains("TaskID") )
    {
        m_strResponseTaskID = GetValue<decltype(m_strResponseTaskID)>(fields["TaskID"]);
    }
    else
    {    
        return false; // 如果 TaskID 不存在返回 false
    }

    return true; // 成功解析数据
} 

// 解析作业状态反馈命令
bool JobStatusReport::AnalyzeData(nlohmann::json& fields) 
{
    CCmdData::AnalyzeData(fields);

    if (fields.contains("TaskID"))
    {
        m_strTaskID = GetValue<decltype(m_strTaskID)>(fields["TaskID"]);
    }
    else
    {
        return false; // 如果 TaskID 不存在或不是字符串，返回 false
    }   

    if(fields.contains("ReportType"))
    {
        m_nReportType = GetValue<decltype(m_nReportType)>(fields["ReportType"]);
    }
    else
    {
        return false; // 如果 ReportType 不存在或不是无符号整数，返回 false
    }

    if(fields.contains("Remark"))
    {
        m_strRemark = GetValue<decltype(m_strRemark)>(fields["Remark"]);
    }
    else
    {
        return false; // 如果 Remark 不存在或不是字符串，返回 false
    }

    return true; // 成功解析数据
}

// 生成任务控制请求的 JSON 字符串
std::string TaskControl::ToJsonString() const 
{
    nlohmann::json tempSend = {{"CMD", m_uCmd},
                                {"TYPE", m_nType},
                                {"SEQ", m_ulSeq},
                                {"STATUS", m_nLastStatus},
                                {"ZEOF",""}};

    tempSend["TaskID"] = m_strTaskID; // 添加 TaskID 字段
    tempSend["ControlCode"] = m_nControlCode; // 添加 ControlCode 字段
    return tempSend.dump(); // 使用 dump() 方法将 JSON 对象转换为字符串
}
// 解析任务控制回应命令
bool TaskControl::AnalyzeData(nlohmann::json& fields) 
{
    CCmdData::AnalyzeData(fields);

    if(fields.contains("TaskID") )
    {
        m_strResponseTaskID = GetValue<decltype(m_strResponseTaskID)>(fields["TaskID"]);
    }
    else
    {
        return false; // 如果 TaskID 不存在返回 false
    }

    if(fields.contains("ControlCode") )
    {
        m_nControlCode = GetValue<decltype(m_nControlCode)>(fields["ControlCode"]);
    }
    else
    {
        return false; // 如果 ControlCode 不存在或不是无符号整数，返回 false
    }

    return true; // 成功解析数据
}
 // 解析仓储作业完成通知请求
bool WarehouseJobCompletion::AnalyzeData(nlohmann::json& fields) 
{
    CCmdData::AnalyzeData(fields);
    if(fields.contains("BOM") )
    {
        m_vecBOM.clear(); // 清空之前的 BOM 数据
        // 遍历 BOM 数组并将每个元素添加到 m_vecBOM 中
        // 假设 BOM 数组中的每个元素都是一个 JSON 对象
    
        for (const auto& item : fields["BOM"])
        {
            m_vecBOM.push_back(item);
        }
    }
    else
    {
        return false; // 如果 BOM 不是数组，返回 false
    }

    if(fields.contains("Total") )
    {
        m_nTotal = GetValue<decltype(m_nTotal)>(fields["Total"]);
    }
    else
    {
        return false; // 如果 Total 不是无符号整数，返回 false
    }
    return true; // 成功解析数据
}

 // 生成仓储作业完成通知回应的 JSON 字符串       
std::string WarehouseJobCompletion::ToJsonString() const 
{
    nlohmann::json tempSend = {{"CMD", m_uCmd},
                                {"TYPE", m_nType},
                                {"SEQ", m_ulSeq},
                                {"STATUS", m_nLastStatus},
                                {"ZEOF",""}};

    return tempSend.dump(); // 使用 dump() 方法将 JSON 对象转换为字符串
}     