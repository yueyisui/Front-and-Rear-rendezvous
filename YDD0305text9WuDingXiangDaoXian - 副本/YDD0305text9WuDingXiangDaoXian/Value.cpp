#include "stdafx.h"
#include "Value.h"
#include <locale.h>
#include<cmath>
const double EPSILON = 1.0E-12;  //极小值
const double PI = 4.0*atan(1.0);  //PI圆周率


Value::Value()
{
}

//释放内存
Value::~Value()
{
	if (knowpoint)delete[]knowpoint;
	if (unknowpoint)delete[]unknowpoint;
	if (angleObs)delete[]angleObs;
	if (distantObs)delete[]distantObs;

	if (FW_angle)delete[]FW_angle;
	if (falseUnknowpoint)delete[]falseUnknowpoint;
}

//打开文件，获取文件中的信息
void Value::getvalue(CString filename)
{
	setlocale(LC_ALL, "");                              //加上这一句,设置语言环境
	//打开文件
	CStdioFile rfile;                                   //定义文件
	if (!rfile.Open(filename, CFile::modeRead)) return; //打开文件

	CString strLine;                          //变量
	BOOL bEOF = rfile.ReadString(strLine);    //读取文件第一行
	 bEOF = rfile.ReadString(strLine);        //读取文件第二行
	knowpointnum = _ttoi(strLine);            //获取已知点个数
	knowpoint = new Point[knowpointnum];      //为已知点开辟空间
	CStringArray strtemp;                     //变量，存储每行分割的结果
	//获取已知点
	for (int i = 0; i < knowpointnum; i++)
	{
		bEOF = rfile.ReadString(strLine);               //读取文件
		int n = SpliteStringArray(strLine,',',strtemp); //分割读取的这一行
		//坐标
		double dx = _tstof(strtemp[1]);          
		double dy = _tstof(strtemp[2]); 
		knowpoint[i].setpoint(strtemp[0], dx, dy);      //获取点名，坐标
	}

	//获取未知点
	bEOF = rfile.ReadString(strLine);
	bEOF = rfile.ReadString(strLine);        //读取未知点个数
	unknowpointnum = _ttoi(strLine);         //未知点个数
	unknowpoint = new Point[unknowpointnum]; //为未知点开辟空间
	bEOF = rfile.ReadString(strLine);        //读取未知点名字所在行
	int n = SpliteStringArray(strLine, ',', strtemp); //分割
	//未知点的名字
	for (int i = 0; i < unknowpointnum; i++)
	{
		//获取未知点
		unknowpoint[i].setpoint(strtemp[i], 0, 0);
	}

	//获取观测角度
	bEOF = rfile.ReadString(strLine);
	bEOF = rfile.ReadString(strLine);   //读取角度观测值类别（1为左观测，2为右观测）
	type = _ttoi(strLine);              //获取观测角度类型
	bEOF = rfile.ReadString(strLine);   //读取角度观测值所在行
	angleObsnum = _ttoi(strLine);       //获取观测角度个数
	angleObs = new Angle[angleObsnum];  //为角度观测值开辟空间
	//逐行获取角度观测值
	for (int i = 0; i < angleObsnum; i++)
	{
		bEOF = rfile.ReadString(strLine);
		int n = SpliteStringArray(strLine, ',', strtemp);//分割
		Point *Pstart = SearchPointUsingID(strtemp[0]);  //上一点名
		Point *Pmid = SearchPointUsingID(strtemp[1]);    //角度观测值对应点名
		Point *Pend = SearchPointUsingID(strtemp[2]);    //下一点名
		double dangle = _tstof(strtemp[3]);              //观测值
		angleObs[i].setangle(Pstart, Pmid, Pend, dangle);//获取观测角度信息
	}

	//观测距离
	bEOF = rfile.ReadString(strLine);
	bEOF = rfile.ReadString(strLine);        //读取观测距离个数
	Obsdistantnum = _ttoi(strLine);          //获取观测距离个数
	distantObs = new Distant[Obsdistantnum]; //为观测距离开辟空间
	//逐行获取距离观测值
	for (int i = 0; i < Obsdistantnum; i++)
	{
		bEOF = rfile.ReadString(strLine);
		int n = SpliteStringArray(strLine, ',', strtemp);  //分割
		Point *Pstart = SearchPointUsingID(strtemp[0]);   //起点
		Point *Pend = SearchPointUsingID(strtemp[1]);     //终点
		double dlength = _tstof(strtemp[2]);              //观测值
		distantObs[i].setdistant(Pstart, Pend, dlength);  //获取观测距离信息
	}

	rfile.Close();
}

//
void Value::falseUnKnowPoint()
{
	//假设路线的第一个未知点
	double RadFW_AnglePointC = PI / 2.0;   //假设起始方位角为90度
	unknowpoint[0].X = knowpoint[0].X + distantObs[0].distant*cos(RadFW_AnglePointC);//假设路线的第一个未知点坐标
	unknowpoint[0].Y = knowpoint[0].Y + distantObs[0].distant*sin(RadFW_AnglePointC);

	FW_angle = new Angle[angleObsnum];  //为假设方位角开辟空间
	
	//第二个未知点
	
	//假设路线的第一个未知点对应的方位角
	FW_angle[0].startID = SearchPointUsingID(unknowpoint[0].pointname);     //假设和对应点号一样，因为没有正北方向点，无影响
	FW_angle[0].midID = SearchPointUsingID(unknowpoint[0].pointname);       //对应点号
	FW_angle[0].endID = SearchPointUsingID(unknowpoint[1].pointname);       //下一点号
	FW_angle[0].angle = FangWeiAngle(angleObs[0].angle, RadFW_AnglePointC); //值
	//假设路线的第二个未知点坐标
	unknowpoint[1].X = unknowpoint[0].X + distantObs[1].distant*cos(FW_angle[0].angle);
	unknowpoint[1].Y = unknowpoint[0].Y + distantObs[1].distant*sin(FW_angle[0].angle);

	//假设路线的第三个点到最后一个未知点
	for (int i = 1; i < angleObsnum - 1; i++)
	{
		//假设路线的第（i+1）个未知点对应的方位角
		FW_angle[i].startID = SearchPointUsingID(unknowpoint[i].pointname);//假设和对应点号一样，因为没有正北方向点，无影响
		FW_angle[i].midID = SearchPointUsingID(unknowpoint[i].pointname);
		FW_angle[i].endID = SearchPointUsingID(unknowpoint[i+1].pointname);
		FW_angle[i].angle = FangWeiAngle(angleObs[i].angle, FW_angle[i-1].angle);
		//假设路线的第(i+2)个未知点对应的方位角
		unknowpoint[i+1].X = unknowpoint[i].X + distantObs[i+1].distant*cos(FW_angle[i].angle);
		unknowpoint[i+1].Y = unknowpoint[i].Y + distantObs[i+1].distant*sin(FW_angle[i].angle);
	}

	//假设路线 最后一个角度观测值对应的方位角
	FW_angle[angleObsnum - 1].startID = SearchPointUsingID(unknowpoint[angleObsnum - 1].pointname);//假设和对应点号一样，因为没有正北方向点，无影响
	FW_angle[angleObsnum - 1].midID = SearchPointUsingID(unknowpoint[angleObsnum - 1].pointname);
	FW_angle[angleObsnum - 1].endID = SearchPointUsingID(knowpoint[1].pointname);
	FW_angle[angleObsnum - 1].angle = FangWeiAngle(angleObs[angleObsnum - 1].angle, FW_angle[angleObsnum - 2].angle);
	//假设的C点坐标
	falseknowpoint.X = unknowpoint[angleObsnum - 1].X +
		distantObs[Obsdistantnum - 1].distant*cos(FW_angle[angleObsnum - 1].angle);
	falseknowpoint.Y = unknowpoint[angleObsnum - 1].Y +
		distantObs[Obsdistantnum - 1].distant*sin(FW_angle[angleObsnum - 1].angle);

}

//计算方位角差
void Value::Fw_angleError()
{
	double temp1 = atan2((knowpoint[1].Y - knowpoint[0].Y) ,(knowpoint[1].X - knowpoint[0].X));    //BC方位角
	double temp2 = atan2((falseknowpoint.Y - knowpoint[0].Y), (falseknowpoint.X - knowpoint[0].X)); //BC'方位角
	if (temp1 < 0)temp1 += 2 * PI;
	angleError = temp1 - temp2;//方位角差
}

//计算R,T
void Value::getR_T_value()
{
	//原路线BC间距离
	double distantBC = sqrt((knowpoint[1].X - knowpoint[0].X)*(knowpoint[1].X - knowpoint[0].X) 
		+ (knowpoint[1].Y - knowpoint[0].Y)*(knowpoint[1].Y - knowpoint[0].Y));
	//假设路线BC间距离
	double False_distantBC = sqrt((falseknowpoint.X - knowpoint[0].X)*(falseknowpoint.X - knowpoint[0].X) 
		+ (falseknowpoint.Y - knowpoint[0].Y)*(falseknowpoint.Y - knowpoint[0].Y));
	R = distantBC / False_distantBC;  //计算R，闭合边长度比
	T = abs((distantBC - False_distantBC) / distantBC);  //计算T，全长相对闭合差
}

//计算每个点的原路线近似坐标
void Value::truePoint()
{
	falseUnknowpoint = new Point[unknowpointnum];
	//计算原路线第一个未知点的近似坐标
	falseUnknowpoint[0].X = knowpoint[0].X + distantObs[0].distant*cos(PI / 2.0 + angleError);
	falseUnknowpoint[0].Y = knowpoint[0].Y + distantObs[0].distant*sin(PI / 2.0 + angleError);
	//依次计算剩下未知点的近似坐标
	for (int i = 0; i < angleObsnum - 1; i++)
	{
		FW_angle[i].angle += angleError;//原路线每个方位角等于假设路线方位角加方位角差
		falseUnknowpoint[i + 1].X = falseUnknowpoint[i].X + distantObs[i + 1].distant*cos(FW_angle[i].angle);
		falseUnknowpoint[i + 1].Y = falseUnknowpoint[i].Y + distantObs[i + 1].distant*sin(FW_angle[i].angle);
	}
	//计算原图C点的近似坐标，用于求fx,fy
	FW_angle[angleObsnum - 1].angle += angleError;//原路线每个方位角等于假设路线方位角加方位角差
	falseknowpoint2.X = falseUnknowpoint[angleObsnum - 1].X +
		distantObs[Obsdistantnum - 1].distant*cos(FW_angle[angleObsnum - 1].angle);
	falseknowpoint2.Y = falseUnknowpoint[angleObsnum - 1].Y +
		distantObs[Obsdistantnum - 1].distant*sin(FW_angle[angleObsnum - 1].angle);


	//计算原路线第一个未知点的坐标
	unknowpoint[0].X = knowpoint[0].X + distantObs[0].distant*R*cos(PI / 2.0 + angleError);
	unknowpoint[0].Y = knowpoint[0].Y + distantObs[0].distant*R*sin(PI / 2.0 + angleError);
	//依次计算剩下未知点的坐标
	for (int i = 0; i < angleObsnum - 1; i++)
	{
		//FW_angle[i].angle += angleError;//原路线每个方位角等于假设路线方位角加方位角差
		unknowpoint[i + 1].X = unknowpoint[i].X + distantObs[i + 1].distant*R*cos(FW_angle[i].angle);
		unknowpoint[i + 1].Y = unknowpoint[i].Y + distantObs[i + 1].distant*R*sin(FW_angle[i].angle);
	}

}

//输出
void Value::resultput()
{
	CFileDialog dlgFile(FALSE, _T("txt"), NULL, OFN_EXPLORER,
		_T("(文本文件)|*.txt"));
	if (dlgFile.DoModal() == IDCANCEL) return;    //如果成功则返回
	CString strFileName2 = dlgFile.GetPathName();
	setlocale(LC_ALL, "");                         //设置环境语言

	//打开文件，写入
	CStdioFile wfile;
	if (!wfile.Open(strFileName2, CFile::modeCreate | CFile::modeWrite)) return;

	double fx, fy;  //坐标闭合差
	int accuracy;    //边长观测精度
	fx = falseknowpoint2.X - knowpoint[1].X;
	fy = falseknowpoint2.Y - knowpoint[1].Y;
	accuracy = int(abs(1 / T));
	//输出结果
	CString strout;
	strout.Format(_T("#####无定向导线近似平差结果如下#####\n已知点个数："));
	CString out1;  //变量
	CString out2;  //变量
	//已知点
	out1.Format(_T("%d\n"), knowpointnum);
	strout += out1;
	for (int i = 0; i < knowpointnum; i++)
	{
		out1 = knowpoint[i].pointname;
		out2.Format(_T(",%.3f,%.3f\n"), knowpoint[i].X, knowpoint[i].Y);
		strout += out1 + out2;
	}
	//未知点
	out2.Format(_T("未知点个数：%d\n"), unknowpointnum);
	strout += out2;
	for (int i = 0; i < unknowpointnum; i++)
	{
		out1 = unknowpoint[i].pointname;
		out2.Format(_T(",%.3f,%.3f\n"), unknowpoint[i].X, unknowpoint[i].Y);
		strout += out1 + out2;
	}
	//闭合差，精度
	out2.Format(_T("坐标闭合差：fx=%.3f  fy=%.3f\n边长观测精度：1/%d"), fx, fy, accuracy);
	strout += out2;

	wfile.WriteString(strout); //数据读入到wfile
	wfile.Close();              //关闭文件
}


//分割函数
int Value::SpliteStringArray(CString str, char split, CStringArray & astr)
{
	int startID = 0;
	int ID = str.Find(split, startID);
	astr.RemoveAll();   //先清空
	while (-1 != ID)
	{
		CString sTemp = str.Mid(startID, ID - startID);
		astr.Add(sTemp);
		startID = ID + 1;
		ID = str.Find(split, startID);
	}
	CString sTemp = str.Right(str.GetLength() - startID);
	if (!sTemp.IsEmpty())
		astr.Add(sTemp);
	return astr.GetSize();
}

//根据点号从已知点数组中找到控制点，并返回该点的指针
Point* Value::SearchKnownPointUsingID(CString strID)
{
	for (int i = 0; i < knowpointnum; i++)
	{
		if (strID == knowpoint[i].pointname)
		{
			return &knowpoint[i];
		}
	}
	return NULL;
}

//根据点号从未知点数组中找到控制点，并返回该点的指针
Point *Value::SearchUnKnownPointUsingID(CString strID)
{
	for (int i = 0; i < unknowpointnum; i++)
	{
		if (strID == unknowpoint[i].pointname)
		{
			return &unknowpoint[i];
		}
	}
	return NULL;
}

//根据点号从未知点和已知点数组中找到控制点，并返回该点的指针
Point *Value::SearchPointUsingID(CString strID)
{
	Point  *pCP = NULL;
	pCP = SearchKnownPointUsingID(strID);
	if (pCP == NULL)
	{
		pCP = SearchUnKnownPointUsingID(strID);
	}
	return pCP;
}

//度分秒转弧度
double  Value::Dms2Rad(double DMS)
{
	int iDeg, iMin;  //整度，分
	double dSec;     //秒
	iDeg = int(DMS + EPSILON);
	iMin = int((DMS - iDeg) * 100 + EPSILON);
	dSec = ((DMS - iDeg) * 100 - iMin) * 100;
	return ((iDeg + double(iMin) / 60 + dSec / 3600)*PI / 180);  //弧度
}


//获取点信息
void Point::setpoint(CString str, double dx, double dy)
{
	pointname = str;
	X = dx;
	Y = dy;
}

//获取角度信息
void Angle::setangle(Point *Pstart, Point *Pmid, Point *Pend,double dangle)
{
	startID = Pstart;
	midID = Pmid;
	endID = Pend;
	angle = dangle;
}

//获取距离信息
void Distant::setdistant(Point *Pstart, Point *Pend, double dlength)
{
	startID = Pstart;
	endID = Pend;
	distant = dlength;
}

//方位角
double  Value::FangWeiAngle(double DMS, double RadfrontFW_Angle)  //DMS观测角度，RadfrontFW_Angle上一个点的方位角
{
	double radAngle = Dms2Rad(DMS);  //转为弧度
	double FW_Angle; //临时变量，该点方位角
	//观测角度为左边
	if (type == 1)
	{
		FW_Angle = RadfrontFW_Angle + radAngle - PI;
	}
	//观测角度为右边
	else
	{
		FW_Angle = RadfrontFW_Angle - radAngle + PI;
	}
	//将值确定在0-2*pi
	if (FW_Angle > 2.0*PI)
	{
		FW_Angle -= 2.0*PI;
	}
	else if (FW_Angle < 0)
	{
		FW_Angle += 2.0*PI;
	}
	return FW_Angle;  //返回方位角
}
