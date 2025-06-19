
#include"TaskData.h"
#include"../../../base/public/ConfigBase.h"
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

std::string  CCmdData::ToJsonString() const
{
    nlohmann::json tempSend = { { "CMD",m_uCmd},{ "TYPE",m_nType },{ "SEQ",m_ulSeq},{ "STATUS",m_nLastStatus },{ "ZEOF","" } };
    return tempSend.dump();
}

bool VideoDev::AnalyzeData(nlohmann::json& fields)
{
    CCmdData::AnalyzeData(fields);

    m_nLastStatus = 0;
    m_nVideoNo = 0;
    switch (m_uCmd)
    {
    case    10:
        if (!fields.contains("VIDEOSOURCE") || !fields.contains("VIDEONAME"))  //数据不全，返回失败
        {
            m_nLastStatus = -1;
        }
        else
        {
            m_nVideoSource = GetValue<decltype(m_nVideoSource)>(fields["VIDEOSOURCE"]);
            m_strVideoName = GetValue<decltype(m_strVideoName)>(fields["VIDEONAME"]);
            m_nVideoCodec = 0;
            if (fields.contains("VIDEOCODEC"))
            {
                m_nVideoCodec = GetValue<decltype(m_nVideoCodec)>(fields["VIDEOCODEC"]);
            }
        }
        break;

    case    11:
        if (!fields.contains("VIDEONUM"))
        {
            m_nLastStatus = -1;
        }
        else
        {
            m_nVideoNo = GetValue<decltype(m_nVideoNo)>(fields["VIDEONUM"]);
            if (fields.contains("VIDEONAME"))
            {
                m_strVideoName = GetValue<decltype(m_strVideoName)>(fields["VIDEONAME"]);
            }
            else
            {
                m_strVideoName.clear();
            }

            if (fields.contains("VIDEOCODEC"))
            {
                m_nVideoCodec = GetValue<decltype(m_nVideoCodec)>(fields["VIDEOCODEC"]);
            }
            else
            {
                m_nVideoCodec = 0;
            }

        }

    case    12:
        if (!fields.contains("VIDEONUM"))
        {
            m_nLastStatus = -1;
        }
        else
        {
            m_nVideoNo = GetValue<decltype(m_nVideoNo)>(fields["VIDEONUM"]);
        }
        break;

    case    13:
        break;

    default:
        m_nLastStatus = -6;
    }
    return 0 == m_nLastStatus;
}

void  VideoDev::Print(std::ostream& of)const
{
    //of << "Trolley and Hoist Pos: " << m_fTrolley << ',' << m_fHoist << std::endl;
    //of << "Trolley and Hoist Vel: " << m_fVelTrolley << ',' << m_fVelHoist << std::endl;
    //of << "Hoist size and status: " << int(m_byHoistSize) << ',' << int(m_byHoistStatus) << std::endl;
    //of << "Gantry,trolley,hoist status: " << int(m_bMGRun) << ',' << int(m_bMTRun) << ',' << int(m_bMHRun) << std::endl;
    //of << std::endl;
}

std::string  VideoDev::ToJsonString() const
{
    nlohmann::json tempSend = { { "CMD",m_uCmd},{ "TYPE",m_nType },{ "SEQ",m_ulSeq},{ "STATUS",m_nLastStatus },{ "ZEOF","" } };
    if (m_nVideoNo != 0)
        tempSend["VIDEONUM"] = m_nVideoNo;
    switch (m_uCmd)
    {
    case    10:
        tempSend["VIDEONAME"] = m_strVideoName;
        break;

    case    13:
        tempSend["VIDEOCOUNT"] = m_vecVideos.size();
        if(!m_vecVideos.empty())
        {
            auto arrayResult = nlohmann::json::array();
            for (auto& item : m_vecVideos)
            {
                nlohmann::json itemData;
                if (item.second >= 200 && item.second < 1000)
                    itemData["VIDEOSOURCE"] = 10;   //工业相机
                else
                    itemData["VIDEOSOURCE"] = 1;    //总是认为是USB相机
                itemData["VIDEONAME"] = item.first;
                itemData["VIDEONUM"] = item.second;
                arrayResult.push_back(itemData);
            }
            tempSend["VIDEODEVS"] = arrayResult;
        }
        break;
    default:
        break;
    }
    return tempSend.dump();
}

bool ModelConf::AnalyzeData(nlohmann::json& fields)
{
    CCmdData::AnalyzeData(fields);

    if (fields.contains("MODELTYPE"))
    {
        m_nModelType = GetValue<decltype(m_nModelType)>(fields["MODELTYPE"]);
    }
    else
        m_nModelType = 0;

    if (fields.contains("MODELNAME"))
    {
        m_strModelName = GetValue<decltype(m_strModelName)>(fields["MODELNAME"]);
    }
    else
        return false;

    if (fields.contains("MODELFILE"))
    {
        m_strModelFile = GetValue<decltype(m_strModelFile)>(fields["MODELFILE"]);
    }
    else
        m_strModelFile.clear();

    if (fields.contains("MODELCLASSES"))
    {
        for (auto& element : fields["MODELCLASSES"])
        {
            m_vecClasses.emplace_back(GetValue<std::string>(element));
        }
    }
    else
        m_vecClasses.clear();

    return true;
}

void  ModelConf::Print(std::ostream& of)const
{
    //of << "Trolley and Hoist Pos: " << m_fTrolley << ',' << m_fHoist << std::endl;
    //of << "Trolley and Hoist Vel: " << m_fVelTrolley << ',' << m_fVelHoist << std::endl;
    //of << "Hoist size and status: " << int(m_byHoistSize) << ',' << int(m_byHoistStatus) << std::endl;
    //of << "Gantry,trolley,hoist status: " << int(m_bMGRun) << ',' << int(m_bMTRun) << ',' << int(m_bMHRun) << std::endl;
    //of << std::endl;
}

std::string  ModelConf::ToJsonString()const
{
    nlohmann::json tempSend = { { "CMD",m_uCmd},{ "TYPE",m_nType },{ "SEQ",m_ulSeq},{ "STATUS",m_nLastStatus },{ "ZEOF","" } };
    if (!m_strModelName.empty())
        tempSend["MODELNAME"] = m_strModelName;
    return tempSend.dump();
}

bool TaskConf::AnalyzeData(nlohmann::json& fields)
{
    CCmdData::AnalyzeData(fields);

    if (!fields.contains("TASKID") || !fields.contains("MODELNAME") || !fields.contains("VIDEOLIST"))
        return false;

    m_strTaskID = GetValue<decltype(m_strTaskID)>(fields["TASKID"]);
    m_strModelName = GetValue<decltype(m_strModelName)>(fields["MODELNAME"]);

    if (fields.contains("TASKTYPE"))
    {
        m_nTaskType = GetValue<decltype(m_nTaskType)>(fields["TASKTYPE"]);
    }
    else
        m_nTaskType = 1;

    if (fields.contains("VIDEOTYPE"))
    {
        m_nVideoType = GetValue<decltype(m_nVideoType)>(fields["VIDEOTYPE"]);
    }
    else
        m_nVideoType = 1;

    //解析视频数据列表
    for (auto& element : fields["VIDEOLIST"])
    {
        //解析视频数据项
        if (1 == m_nVideoType)
            m_vecVideoSrc.emplace_back(element.template get<int>());
        else
            m_vecVideoSrc.emplace_back(GetValue<std::string>(element));
    }

    if (fields.contains("INFERCLASSES"))
    {
        std::vector<std::string> vecClasses;
        CConfigBase::Split(GetValue<std::string>(fields["INFERCLASSES"]), vecClasses);
         
        for (auto& element : vecClasses)
        {
            m_vecClasses.emplace_back(std::stoi(element));
        }
    }
    else
        m_vecClasses.clear();

    if (fields.contains("ORIGINVIDEOPATH"))
    {
        m_strVideoSavePath = GetValue<decltype(m_strVideoSavePath)>(fields["ORIGINVIDEOPATH"]);
    }
    else
        m_strVideoSavePath.clear();
    if (fields.contains("INFERVIDEOPATH"))
    {
        m_strInfrResultSavePath = GetValue<decltype(m_strInfrResultSavePath)>(fields["INFERVIDEOPATH"]);
    }
    else
        m_strInfrResultSavePath.clear();

    if (fields.contains("CONFTHRESHOLD"))
    {
        m_fConfThreshold = GetValue<decltype(m_fConfThreshold)>(fields["CONFTHRESHOLD"]);
    }
    else
        m_fConfThreshold=-0.1f;
    if (fields.contains("INFERTARGETNUM"))
    {
        m_nMaxTarget = GetValue<decltype(m_nMaxTarget)>(fields["INFERTARGETNUM"]);
    }
    else
        m_nMaxTarget=0;
    if (fields.contains("INFERFREQ"))
    {
        m_nInferFreq = GetValue<decltype(m_nInferFreq)>(fields["INFERFREQ"]);
    }
    else
        m_nInferFreq=0;

    return true;
}

void  TaskConf::Print(std::ostream& of)const
{
    //of << "Trolley and Hoist Pos: " << m_fTrolley << ',' << m_fHoist << std::endl;
    //of << "Trolley and Hoist Vel: " << m_fVelTrolley << ',' << m_fVelHoist << std::endl;
    //of << "Hoist size and status: " << int(m_byHoistSize) << ',' << int(m_byHoistStatus) << std::endl;
    //of << "Gantry,trolley,hoist status: " << int(m_bMGRun) << ',' << int(m_bMTRun) << ',' << int(m_bMHRun) << std::endl;
    //of << std::endl;
}

std::string  TaskConf::ToJsonString()const
{
    nlohmann::json tempSend = { { "CMD",m_uCmd},{ "TYPE",m_nType },{ "SEQ",m_ulSeq},{ "STATUS",m_nLastStatus },{ "ZEOF","" } };
    if (!m_strTaskID.empty())
        tempSend["TASKID"] = m_strTaskID;
    return tempSend.dump();
}

bool TaskRunning::AnalyzeData(nlohmann::json& fields)
{
    CCmdData::AnalyzeData(fields);

    if (50 == m_uCmd && !fields.contains("TASKID") || 51 == m_uCmd && !fields.contains("TASKNO"))
        return false;

    if (fields.contains("TASKID"))
        m_strTaskID = fields["TASKID"].template get<decltype(m_strTaskID)>();
    if (fields.contains("TASKNO"))
        m_nTaskNo = fields["TASKNO"].template get<decltype(m_nTaskNo)>();
    if (fields.contains("FLAG"))
        m_nTaskFlag = fields["FLAG"].template get<decltype(m_nTaskFlag)>();

    if (fields.contains("INFERCLASSES"))
    {
        std::vector<std::string> vecClasses;
        CConfigBase::Split(GetValue<std::string>(fields["INFERCLASSES"]), vecClasses);

        for (auto& element : vecClasses)
        {
            m_vecClasses.emplace_back(std::stoi(element));
        }
    }

    if (fields.contains("ORIGINVIDEOPATH"))
    {
        m_strVideoSavePath = GetValue<decltype(m_strVideoSavePath)>(fields["ORIGINVIDEOPATH"]);
    }

    if (fields.contains("INFERVIDEOPATH"))
    {
        m_strInfrResultSavePath = GetValue<decltype(m_strInfrResultSavePath)>(fields["INFERVIDEOPATH"]);
    }


    if (fields.contains("CONFTHRESHOLD"))
    {
        m_fConfThreshold = GetValue<decltype(m_fConfThreshold)>(fields["CONFTHRESHOLD"]);
    }

    if (fields.contains("INFERTARGETNUM"))
    {
        m_nMaxTarget = GetValue<decltype(m_nMaxTarget)>(fields["INFERTARGETNUM"]);
    }

    if (fields.contains("INFERFREQ"))
    {
        m_nInferFreq = GetValue<decltype(m_nInferFreq)>(fields["INFERFREQ"]);
    }

    if (fields.contains("VIDEOOVERLAYINFER"))
    {
        //视频叠加标志,1表示叠加，0表示不叠加,小于0表示无效
        m_bVideoOverlayFlag = 1 == fields["VIDEOOVERLAYINFER"].template get<decltype(m_nInferFreq)>();
    }

    if (fields.contains("TASKTIME"))
    {
        m_uTaskRunTime = fields["TASKTIME"].template get<decltype(m_uTaskRunTime)>();
    }
    return true;
}

std::string  TaskRunning::ToJsonString()const
{
    nlohmann::json tempSend = { { "CMD",m_uCmd},{ "TYPE",m_nType },{ "SEQ",m_ulSeq},{ "STATUS",m_nLastStatus },{ "ZEOF","" } };
    tempSend["TASKNO"] = m_nTaskNo;
    if (50 == m_uCmd)
        tempSend["TASKID"] = m_strTaskID;
    else if (51 == m_uCmd)
        tempSend["FLAG"] = m_nTaskFlag;
    return tempSend.dump();
}

void  TaskResult::Print(std::ostream& of)const
{
    //of << "Trolley and Hoist Pos: " << m_fTrolley << ',' << m_fHoist << std::endl;
    //of << "Trolley and Hoist Vel: " << m_fVelTrolley << ',' << m_fVelHoist << std::endl;
    //of << "Hoist size and status: " << int(m_byHoistSize) << ',' << int(m_byHoistStatus) << std::endl;
    //of << "Gantry,trolley,hoist status: " << int(m_bMGRun) << ',' << int(m_bMTRun) << ',' << int(m_bMHRun) << std::endl;
    //of << std::endl;
}

std::string  TaskResult::ToJsonString()const
{
    nlohmann::json tempSend = { { "CMD",60},{ "TYPE",3 },{ "SEQ",m_ulSeq},{ "STATUS",0 },{ "ZEOF","" } };
    tempSend["TASKNO"] = m_nTaskNo;
    tempSend["DATATYPE"] = m_nTaskType;
    tempSend["TASKID"] = m_strTaskName;
    auto arrayResult = nlohmann::json::array();

    for (auto& item : m_vecResults)
    {
        nlohmann::json itemData;
        itemData["SOURCE"] = item.strDataSource;
        if(!item.strImageResult.empty())
            itemData["IMAGENAME"] = item.strImageResult;
        itemData["TIMESTAP"] = static_cast<unsigned long long>(item.tCur);

        auto arrayItem = nlohmann::json::array();
        for (auto& obj : item.DetObjs)
        {
            std::ostringstream out;
            const Detection& det = *obj;
            out << det.class_id << ' ' << det.confidence << ' ' << det.box.x << ' ' << det.box.y << ' ' << det.box.width << ' ' << det.box.height;
            arrayItem.push_back(out.str());
        }
        itemData["RESULT"] = arrayItem;

       //写入结果
        arrayResult.push_back(itemData);
    }
    tempSend["DATA"] = arrayResult;
    return tempSend.dump();
}

