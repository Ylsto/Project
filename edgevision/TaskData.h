#pragma once
#include <string>
#include<vector>
#include<any>
//#include<set>
#include"common.h"
#include "../../../base/infer/infer.h"
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


//视频设备信息
struct VideoDev : public CCmdData
{
private:
    //视频数据源类型
    //1、表示USB相机
    //2、CSI相机
    //3、RTSP视频流
    //4、UDP视频流
    //5、HTTP视频流
    int             m_nVideoSource;     
    //视频数据编码
    //1、表示H264编码
    //2、表示H265编码
    int             m_nVideoCodec;
    //视频流数据源或者相机设备的名字
    std::string     m_strVideoName;       
    //视频流设备编号,数据唯一，内部管理编号，缺省为0，表示不存在该视频设备
    int             m_nVideoNo=0;  
    std::vector<std::pair<std::string, int>> m_vecVideos;
public:
    VideoDev() {}
    const std::string& GetVideoName()const { return m_strVideoName; }
    int GetVideoType()const { return m_nVideoSource; }
    int GetVideoCodec()const { return m_nVideoCodec; }
    int GetVideoNum()const { return m_nVideoNo; }
    void SetVideoNum(int val){ m_nVideoNo = val; }
    virtual void  Print(std::ostream&)const override;
    virtual std::string  ToJsonString()const override;
    bool AnalyzeData(nlohmann::json& data) override;
    void ClearVideoDevsCache() { m_vecVideos.clear(); }
    void SetVideoDevsCache(std::vector<std::pair<std::string, int>>& devs) { m_vecVideos.swap(devs); }
protected:
};

//模型信息
struct ModelConf : public CCmdData
{
    //模型类型
    //1、onnx模型
    //2、TensorRT模型
    int             m_nModelType;
    std::string     m_strModelName;     //模型名字
    std::string     m_strModelFile;     //模型文件(含路径)
    std::vector<std::string>    m_vecClasses;   //模型类别描述
public:
    ModelConf() {}
    const std::string& GetModelName()const { return m_strModelName; }
    const std::string& GetModelFile()const { return m_strModelFile; }
    int GetModelType()const { return m_nModelType; }
    std::vector<std::string>& GetMoleClasses() {return m_vecClasses;}
    virtual void  Print(std::ostream&)const override;
    virtual std::string  ToJsonString()const override;
    bool AnalyzeData(nlohmann::json& data) override;
protected:
};

//任务信息配置
struct TaskConf : public CCmdData
{
    //任务类型
    //1、	目标识别
    //2、	目标分割
    //3、   目标定位
    enum TASK_TYPE{OBJ_RECOG=1, OBJ_SEG, OBJ_POS};
    int             m_nTaskType=-1;
    std::string     m_strModelName;         //模型名字
    std::string     m_strTaskID;            //任务ID，唯一

    //视频类型
    //1、	表示相机或者网络视频流
    //2、	表示本地文件视频文件
    //3、	表示本地单张图片文件
    //4、	表示本地图片文件夹
    int             m_nVideoType=-1;
    float           m_fConfThreshold=-0.1f;      //置信度
    int             m_nMaxTarget = -1;            //最大推理解算目标数目
    int             m_nInferFreq = -1;           //推理频率
    
    std::string     m_strVideoSavePath;         //视频保存路径
    std::string     m_strInfrResultSavePath;    //推理结果图片保存路径
    std::vector<std::any>    m_vecVideoSrc;     //数据源列表
    std::vector<int>    m_vecClasses;           //推理类别表
public:
    TaskConf() {}
    const std::string& GetModelName()const { return m_strModelName; }
    const std::string& GetTaskName()const { return m_strTaskID; }
    int GetVideoType()const { return m_nVideoType; }
    int GetTaskType()const { return m_nTaskType; }
    float GetConfThreshold()const { return m_fConfThreshold; }
    int GetMaxTarget()const { return m_nMaxTarget; }
    int GetInferFreq()const { return m_nInferFreq; }
    const std::string& GetVideoSavePath() { return m_strVideoSavePath; }
    const std::string& GetInfrResultSavePath() { return m_strInfrResultSavePath; }
    const std::vector<std::any>& GetVideosSource()const { return m_vecVideoSrc; }

    const std::vector<int>& GetMoleClasses()const { return m_vecClasses; }
    virtual void  Print(std::ostream&)const override;
    virtual std::string  ToJsonString()const override;
    bool AnalyzeData(nlohmann::json& data) override;
protected:
};

//任务启动信息配置
struct TaskRunning : public TaskConf
{
    unsigned        m_nTaskNo;              //任务编号
    unsigned        m_nTaskFlag;            //任务运行标志,0表示停止任务，1表示暂停，2表示恢复暂停运行，小于0表示未定义
    unsigned        m_uTaskRunTime = 0;     //任务运行时间
    bool            m_bVideoOverlayFlag = false;   //视频叠加标志,0表示叠加，1表示不叠加,小于0表示无效
public:
    TaskRunning() {}
    unsigned GetTaskNo()const { return m_nTaskNo; }
    int GetContrlFlag()const { return m_nTaskFlag; }
    virtual std::string  ToJsonString()const override;
    bool AnalyzeData(nlohmann::json& data) override;
protected:
};

//任务信息配置
struct TaskResult : public CCmdData
{
    struct ResultInfer
    {
        std::string     strDataSource;
        std::string     strImageResult;
        time_t          tCur;
        std::vector<std::shared_ptr<Detection>> DetObjs;
    };
    std::string     m_strTaskName;      //任务名字
    //任务类型
    //1、	目标识别
    //2、	目标分割
    int             m_nTaskType;
    unsigned        m_nTaskNo;              //任务编号
    std::vector<ResultInfer>    m_vecResults;  //任务结果
public:
    TaskResult() = default;
    TaskResult(unsigned nTaskNo, const std::string& strTaskName, int nTaskType = 1)
        : m_strTaskName(strTaskName), m_nTaskType(nTaskType), m_nTaskNo(nTaskNo)
    {
    }
    void SetTask(unsigned task, int type)
    {
        m_nTaskNo = task; m_nTaskType = type;
    }
    void SetTask(unsigned task, const std::string& name, int type)
    {
        m_nTaskNo = task; m_nTaskType = type; m_strTaskName = name;
    }
    unsigned GetTaskNo()const { return m_nTaskNo; }
    void AddResultData(std::string&& source, std::vector<std::shared_ptr<Detection>>& Dets, time_t tStamp,const std::string& image="")
    {
        ResultInfer& tmp = m_vecResults.emplace_back();
        tmp.strDataSource = source;
        tmp.strImageResult = image;
        tmp.DetObjs.swap(Dets);
        tmp.tCur = tStamp;
    }
    int GetResultNum()const { return m_vecResults.size(); }
    virtual void  Print(std::ostream&)const override;
    virtual std::string  ToJsonString()const override;
};

#if 0
//桥吊和轮胎吊公共数据
struct Crane_data : public CMqttData
{
    nlohmann::json  m_business;
    std::string     m_strDevNum;                //设备编号,也就是对应的桥吊/轮胎吊编号
    CPoint          m_posCenter;                //中心点坐标
    CPoint          m_posCorner[4];             //四个角落坐标
    time_t          m_Timestamp;                //接收数据时的时间戳

    float           m_gpsDop = 0;                 //GPS的DOP值
    unsigned char   m_byGpsStatus = 0;            //GPS定位状态
    unsigned char   m_bySatellite = 0;            //吊捕获的卫星数
    virtual void  Print(std::ostream&)const override;
protected:
    bool AnalyzeData(nlohmann::json& data) override;
};

//桥吊数据
struct ShoreCrane : public Crane_data
{
    CPoint          m_ptLane[6];                //6个车道作业点坐标，从1号车道到6号车道，如果该车道坐标没有，则为0
public:
    ShoreCrane() { m_pid = PidType::PID_QC; }
    //检查主题是否正确
    //bool  CheckTopic(const std::string& topic)const override;
    virtual void  Print(std::ostream&)const override;
protected:
    bool AnalyzeData(nlohmann::json& data) override;
};

//桥吊数据
struct RtgCrane : public Crane_data
{
    std::string     m_strCranePos;          //RTG贝位位置
    unsigned char   m_byWorkMode;           //RTG作业模式
    unsigned char   m_byWorkStatus;         //RTG作业状态
public:
    RtgCrane() { m_pid = PidType::PID_RTG; }
    virtual void  Print(std::ostream&)const override;
protected:
    bool AnalyzeData(nlohmann::json& data) override;
};

//m_strFilter过滤器为设备编号
//pid=910
struct Vehicledata : public CMqttData
{
    struct VehicleInfo : public VehicleBase {
        //std::string     m_strVehicleNo;     //车辆编号
        std::string     m_strTargetNo;      //目标设备号
        //CPoint          m_coordCur;         //当前位置坐标
        CPoint          m_coordTarget;      //目标位置坐标
        CargoInfo       m_Cargo1;           //第一个集装箱
        CargoInfo       m_Cargo2;           //第二个集装箱
        //unsigned char   m_byVehicleType;    //车辆类型，ITK-1，AIV-2，OTK-3，0保留不用
        //unsigned char   m_byWeightFlag;     //空重车标志，对应"wightFlag"
    };
    std::string     m_strTargetCrane;        //目标桥吊或者轮吊编号
    time_t  m_Timestamp;           //时间戳
    std::vector<VehicleInfo>    m_lstVechicle;  //所有车辆信息数据
public:
    Vehicledata() { m_pid = PidType::PID_VEHICLE; }
    virtual void  Print(std::ostream&)const override;
    const std::vector<VehicleInfo>& GetTruckList()const { return m_lstVechicle; }
protected:
    bool AnalyzeData(nlohmann::json& data) override;
};

//m_strFilter过滤器为设备编号
struct EquipmentData : public CMqttData
{
    struct EquipmentInfo {
        std::string     m_strFeelNum;       //感知编号，每个物理设备唯一一个，对舱盖和锁钮箱如何唯一需要考虑，舱盖可以跟船的号码组合，锁钮箱呢？
        CPoint2D        m_coordCur;         //当前设备坐标
        CPoint2D        m_coordCorner[4];   //四个角的坐标
        std::string     m_strComment;       //附加说明
        std::string     m_strvehicleNo;     //集卡编号
        float           m_fHealthy;         //坐标健康度
        unsigned char   m_byEquipmentType;  //设备类型，1-集卡,2-人,3-舱盖板,4-集装箱,5-锁钮箱,6-锁钮框,7-小铲车,99-其他
        unsigned char   m_byVehicleType;    //集卡类型，ITK-1，AIV-2，OTK-3，0保留不用
        unsigned char   m_byVehicleLane;    //集卡所在车道号，1-6，对于RTG，始终为1
        unsigned char   m_byVehicleOrder;   //该集卡在该车道的排队顺序
        unsigned char   m_byHasIdentify;    //该集卡是否被识别到，0表示没有被识别到，这样集卡排序字段就不存在，1表示感知到但是没有识别到车号，2表示感知到并且识别到车号
        unsigned char   m_byCurVehicleFlag; //当前车辆位置标记，0表示为当前作业车辆，1表示为跟在当前车辆后的车辆，2表示其他车辆
        unsigned char   m_byCounterpoint;   //当前车辆对位标记，1表示停在前小箱作业位置，2表示停在后小箱作业位置，3表示停在大箱作业位置
        unsigned char   m_byCargoStatus;    //当前车辆装载集装箱状态，0表示空箱，1大箱，2双小箱，3前小箱，4后小箱
        unsigned short  m_wVehicleAngle;    //车辆倾斜角度，0~360，如果大于360则表示无效，在上报时，如果角度小于正负10度，则需要增加inclineFlag标志
    };
    std::string     m_strDevNum;            //设备编号,发布数据的融合定位设备号
    //unsigned long long  m_Timestamp;           //时间戳
    std::vector<EquipmentInfo>    m_lstEquipment;  //所有车辆信息数据
public:
    EquipmentData() { m_pid = PID_EQUIP; }
    std::string  ToJsonString() override;
};

struct EquipmentDataRes : public CMqttData
{
    std::string     m_strMachNo;            //设备编号,发布数据的融合定位设备号
    time_t          m_tTimeStamp;           //时间戳
    time_t          m_tReportTimeStamp;     //上报报文时间戳
    std::vector<std::string>    m_lstRecvEquipment;  //所有上报的设备信息感知编号
public:
    EquipmentDataRes() { m_pid = PID_EQUIP_RES; }
    bool AnalyzeData(nlohmann::json& data) override;
};

//设备自检数据
struct EquipmentCheck : public CMqttData
{
    std::string     m_strDevNum;            //设备编号,发布数据的融合定位设备号
    std::string     m_strComment;           //附加说明
    unsigned char   m_byStatus;             //状态数据
    //unsigned long long  m_Timestamp;         //时间戳
public:
    EquipmentCheck() { m_pid = PID_EQUIP_CHECK; }
    std::string  ToJsonString() override;
};

//设备自检数据应答
struct EquipmentCheckRes : public CMqttData
{
    std::string     m_strDevNum;            //设备编号,发布数据的融合定位设备号
    //unsigned long long  m_Timestamp;         //时间戳
public:
    EquipmentCheckRes() { m_pid = PID_EQUIP_CHECK_RES; }

protected:
    bool AnalyzeData(nlohmann::json& data) override;
};

//通过HTTP获取设备号信息
struct RequireEquipments : public CMqttData
{
    std::string     m_strDevNum;            //设备编号
    std::string     m_strOwer;              //设备编号,所属设备,该设备属于哪个设备
    unsigned char   m_byType;               //设备类型
    //unsigned long long  m_Timestamp;         //时间戳
public:
    RequireEquipments() { m_pid = PID_EQUIP_RES; }

protected:
    bool AnalyzeData(nlohmann::json& data) override;
};

//电子围栏订阅
//m_pid=915
//主题：videoCoordinate/ElectronicFence/+
struct ElectronicFence : public CMqttData
{
    std::vector<FenceInfo>  m_lstFences;    //所有电子围栏信息
    unsigned long long  m_Timestamp;        //时间戳
public:
    ElectronicFence() { m_pid = PID_GET_FENCE; }
    std::vector<FenceInfo>& GetAllFences() { return m_lstFences; }
protected:
    bool AnalyzeData(nlohmann::json& data) override;
    void  Print(std::ostream&)const override;
};

//电子围栏订阅应答
//m_pid=916
//主题：videoCoordinate/ElectronicFence/receipt/*
struct ElectronicFenceRes : public CMqttData
{
    //unsigned long long  m_Timestamp;      //时间戳
    std::string         m_strDevID;         //设备ID
public:
    ElectronicFenceRes() { m_pid = PID_RES_FENCE; }
    ElectronicFenceRes(const std::string& strDevID)
        : m_strDevID(strDevID)
    {
        m_pid = PID_RES_FENCE;
    }
    std::string  ToJsonString() override;
};

//电子围栏消失推送
//m_pid=917
//主题：videoCoordinate/ElectronicFence/receipt/*
struct FenceDisappear : public CMqttData
{
    //unsigned long long  m_Timestamp;      //时间戳
    std::string         m_strDevID;         //设备ID
    std::vector<std::string>    m_lstFences;//电子围栏ID
public:
    FenceDisappear() { m_pid = PID_FENCE_INVALID; }
    FenceDisappear(const std::string& strDevID)
        : m_strDevID(strDevID)
    {
        m_pid = PID_FENCE_INVALID;
    }
    void AddFenceID(const std::string& id) { m_lstFences.emplace_back(id); }
    void Clear() { m_lstFences.clear(); }
    int  GetFencesCount()const { return m_lstFences.size(); }
    bool IsEmpty()const { return m_lstFences.empty(); }
    std::string  ToJsonString() override;
};

//电子围栏消失推送应答
//m_pid=918
//主题：videoCoordinate/ElectronicFence/+
struct FenceDisappearRes : public CMqttData
{
    std::string         m_strDevID;         //设备ID
    std::string         m_strFenceID;       //电子围栏ID
    unsigned long long  m_Timestamp;        //时间戳
public:
    FenceDisappearRes() { m_pid = PID_FENCE_INVALID_RES; }

protected:
    bool AnalyzeData(nlohmann::json& data) override;
    void  Print(std::ostream&)const override;
};

//集卡GPS坐标单独推送，1秒一次
//m_pid=920
//主题：vehicleTask/coordinate/+
struct VehicleCoord : public CMqttData
{
    std::string         m_strDevNum;        //设备ID
    unsigned long long  m_Timestamp;        //时间戳
    std::vector<VehicleBase>    m_lstVechicle;  //所有车辆信息数据
public:
    VehicleCoord() { m_pid = PID_VEHICLE_COOR; }

protected:
    bool AnalyzeData(nlohmann::json& data) override;
    void  Print(std::ostream&)const override;
};

//可驶离信号
//m_pid=921
//主题：vehicleTask/leave/request/+
struct PermitLeave : public CMqttData
{
    std::string         m_strDevNum;        //设备ID
    unsigned long long  m_Timestamp;        //时间戳
    std::string     m_strVehicleNo;         //车辆编号
public:
    PermitLeave() { m_pid = PID_LEAVE_PERMIT; }

protected:
    bool AnalyzeData(nlohmann::json& data) override;
    void  Print(std::ostream&)const override;
};

//可驶离信号回执，实质是主动推送报文，只是名字定义为回执
//m_pid=922
//主题：videoCoordinate/leave/receipt/+
struct PermitLeaveRes : public CMqttData
{
    std::string         m_strDevNum;        //设备ID
    unsigned long long  m_Timestamp;        //时间戳
    unsigned long long  m_RecvTimestamp;    //接收到可驶离信号的包中的时间戳
    std::string         m_strVehicleNo;     //车辆编号
    CPoint2D            m_coordVehicle;     //车辆此时的坐标
    unsigned char       m_byReceiptType;    //回执类型，0表示已接收
    unsigned char       m_byLeaveFlag;      //离开标志，0表示未驶离，1表示已经离开
public:
    PermitLeaveRes() { m_pid = PID_LEAVE_RES; }

protected:
    bool AnalyzeData(nlohmann::json& data) override;
    void  Print(std::ostream&)const override;
};

#endif // 0
