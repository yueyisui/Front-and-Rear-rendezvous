#pragma once
#include "Matrix.h"

struct PointPHOTO
{
	double X;           //二维坐标
	double Y;
};

struct Point
{
	double X;           //三维坐标
	double Y;
	double Z;
};

//外方位元素
struct WFWYS
{
	double XS, YS, ZS, angle_t, angle_w, angle_k;
};

//精度
struct JD
{
	double jdXS, jdYS, jdZS, jdangle_t, jdangle_w, jdangle_k;
};


class Cvalue
{
	int knowPHOTOnum, unknowPointnum; //已知控制点数量，未知地面点数量
	PointPHOTO *knowPHOTO1;
	PointPHOTO *knowPHOTO2;
	PointPHOTO *unknowPoint_PHO1;
	PointPHOTO *unknowPoint_PHO2;
	Point *knowGROUND;
	Point *unknowGROUND;
	double f, m;
	double BX, BY, BZ;  //摄影基线分量
	//精度
	WFWYS left; 
	WFWYS right;
	JD leftJD;
	JD rightJD;
	double a01, a02;

public:
	Cvalue();
	~Cvalue();

public:
	void getValue(CString &strinput);  //获取数据
	void calculateFWYS(PointPHOTO *knowPHOTO, Point *knowGROUND, WFWYS &wfwys, JD &temJD, double &a0);//计算外方位元素
	void GROUNDunpointCal(WFWYS wfwys1, WFWYS wfwys2, PointPHOTO *unknowPoint_PHO1, PointPHOTO *unknowPoint_PHO2); //计算地面点坐标
	void outputValue(CString &strout);  //输出结果
public:
	int SpliteStringArray(CString str, char split, CStringArray & astr);//分割函数
};

