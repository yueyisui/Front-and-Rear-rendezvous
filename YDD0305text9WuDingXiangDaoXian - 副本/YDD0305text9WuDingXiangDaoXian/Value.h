#pragma once
//点
class Point
{
public:
	CString pointname;  //点名
	double X;           //坐标
	double Y;
public:
	Point() {};
	~Point() {};
	void setpoint(CString str, double dx, double dy);
};
//角度
class Angle
{
public:
	Point *startID, *midID, *endID;//上一点，所在点，下一点：名
	double angle;   //值
public:
	Angle() {};
	~Angle() {};
	void setangle(Point *Pstart, Point *Pmid, Point *Pend,double dangle);
};
//距离
class Distant
{
public:
	Point *startID, *endID;    //起始点，终点
	double distant;  //值
public:
	~Distant() {};
	Distant() {};
	void setdistant(Point *Pstart, Point *Pend, double dlength);
};


class Value
{
private:  //文件输入进来的数据
	int knowpointnum;   //已知点个数
	int unknowpointnum; //未知点个数
	int angleObsnum;    //角度观测值个数
	int Obsdistantnum;  //观测距离个数
	int type;           //导线观测交类型
	Point *knowpoint;   //已知点
	Point *unknowpoint; //未知点
	Angle *angleObs;    // 角度观测值
	Distant *distantObs;//观测距离

public:  //构造和析构函数
	Value();
	~Value();

public:  //对数据进行操作的函数
	void getvalue(CString filename); //打开文件，获取文件中的信息
	void falseUnKnowPoint();         //计算假设起始方位角为90度的坐标
	void Fw_angleError();            //计算角度差
	void getR_T_value();             //计算R,T 
	void truePoint();                //计算真正观测点的近似坐标

	void resultput();                //输出结果

private:  //新产生的变量，辅助计算
	Angle *FW_angle;         //方位角
	Point *falseUnknowpoint; //近似未知点
	Point falseknowpoint;    //假设点
	Point falseknowpoint2;   //计算C点
	
	double angleError;  //角度差
	double R, T;        //R闭合边长度比,T全长相对闭合差

public:  //辅助计算函数
	int SpliteStringArray(CString str, char split, CStringArray& astr); //分割字符串

	Point * SearchKnownPointUsingID(CString strID);  //查找已知点名

	Point * SearchUnKnownPointUsingID(CString strID); //查找未知点名

	Point * SearchPointUsingID(CString strID);  //查找所有点名

	double Dms2Rad(double DMS);  //度分秒转弧度制

	double FangWeiAngle(double DMS, double RadfrontFW_Angle); //计算方位角

};



