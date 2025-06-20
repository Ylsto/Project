#pragma once
#include <string>
#include<vector>
#include<any>
//#include<set>
#include "json.hpp" //由于此Jason不能把关键字设置为大写或者小写，我已经修改了

class CCmdData /*: public Referenced*/
{
protected:
    unsigned int    m_uCmd;         //执行任务的命令编号
    int             m_nType;        //任务的命令类型
    unsigned long   m_ulSeq;        //请求或者应答序列号
    int             m_nLastStatus=0;  //上一次结束完成状态
public:
    unsigned GetCmd()const { return m_uCmd; }
    void SetCmd(unsigned val) { m_uCmd = val; }
    void SetType(int val) { m_nType = val; }
    int GetType()const { return m_nType; }
          
    void SetSeq(unsigned long val) { m_ulSeq = val; }
    unsigned long GetSeq()const { return m_ulSeq; }
    void SetStatus(int val) { m_nLastStatus = val; }
    int GetStatus()const { return m_nLastStatus; }

protected:
    virtual bool AnalyzeData(nlohmann::json& data);
    virtual std::string  ToJsonString()const;
public:
    CCmdData() = default;
    CCmdData(unsigned int m_uCmd, int m_nType, unsigned long m_ulSeq, int m_nLastStatus)
        : m_uCmd(m_uCmd), m_nType(m_nType), m_ulSeq(m_ulSeq), m_nLastStatus(m_nLastStatus)
    {
    }
    virtual void  Print(std::ostream&)const{}
};

// 小车转运作业类
class CartTransportJob : public CCmdData
{
private:
    // 请求命令参数
    const unsigned int m_uCmd = 80; // 命令编号
    const int m_nType = 1;          // 命令类型
    unsigned long m_ulSeq = 0; // 请求序列号
    int m_nLastStatus = 0;    // 上一次结束状态
    std::string m_strTaskID;        // 任务ID
    unsigned int m_nkType;      // 任务类型
    std::string m_strStartPos;    // 起始位置坐标
    std::string m_strEndPos;      // 结束位置坐标
    unsigned int m_nStayTime;        // 转运停留时间，单位秒
    unsigned int m_nNum;             // 物料数目（仅类型1有效）

    // 回应命令参数
    std::string  m_strResponseTaskID ; // 回应的任务ID

public:
    CartTransportJob(){};

    // 方法声明
    virtual std::string ToJsonString() const override;   // 生成作业请求的 JSON 字符串
    bool AnalyzeData(nlohmann::json& data) override;     // 解析作业回应命令
};


// 作业状态反馈类
class JobStatusReport : public CCmdData
{
private:
    // 请求命令参数
    std::string m_strTaskID; // 任务执行ID
    unsigned int m_nReportType; // 反馈类型
    std::string m_strRemark;      // 附加信息

public:
    JobStatusReport(){};

    // 方法声明
     bool AnalyzeData(nlohmann::json& data) override; // 解析作业状态反馈命令
};

// 任务执行控制类
class TaskControl : public CCmdData
{
private:
    // 请求命令参数
    const unsigned int m_uCmd = 82; // 命令编号
    const int m_nType = 2;          // 命令类型
    unsigned long m_ulSeq = 0;      // 请求序列号
    int m_nLastStatus = 0;          // 上一次结束状态
    std::string m_strTaskID; // 任务ID
    unsigned int m_nControlCode;    // 控制类型

    // 回应命令参数
    std::string m_strResponseTaskID ;  // 回应的任务ID

public:
    TaskControl() {};

    // 方法声明
    virtual std::string ToJsonString() const override;    // 生成任务控制请求的 JSON 字符串
    bool AnalyzeData(nlohmann::json& data) override;      // 解析任务控制回应命令
};

// 仓储作业完成通知类
class WarehouseJobCompletion : public CCmdData
{
private:
    // 请求命令参数
    std::vector<nlohmann::json> m_vecBOM;      // 物料信息数组
    unsigned int m_nTotal;                   // 物料总数量

    // 回应命令参数
    unsigned int m_nResponseMessage = 83;     // 回应消息

public:
    WarehouseJobCompletion() {};

    // 方法声明
    bool AnalyzeData(nlohmann::json& data) override;          // 解析仓储作业完成通知请求
    virtual std::string ToJsonString() const override;        // 生成仓储作业完成通知回应的 JSON 字符串
     // 获取和设置物料信息数组
    const std::vector<nlohmann::json>& GetBOM() const { return m_vecBOM; }
    void SetBOM(const std::vector<nlohmann::json>& bom) { m_vecBOM = bom; }

    // 获取和设置物料总数量
    unsigned int GetTotal() const { return m_nTotal; }
    void SetTotal(unsigned int total) { m_nTotal = total; }

    // 获取和设置回应消息
    unsigned int GetResponseMessage() const { return m_nResponseMessage; }
    void SetResponseMessage(unsigned int msg) { m_nResponseMessage = msg; }
};