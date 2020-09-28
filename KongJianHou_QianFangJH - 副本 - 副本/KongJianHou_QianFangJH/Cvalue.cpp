#include "stdafx.h"
#include "Cvalue.h"
#include <locale.h>
#include<cmath>

Cvalue::Cvalue()
{
}


Cvalue::~Cvalue()
{
	//释放内存
	if (knowPHOTO1)delete[]knowPHOTO1;
	if (knowPHOTO2)delete[]knowPHOTO2;
	if (unknowPoint_PHO1)delete[]unknowPoint_PHO1;
	if (unknowPoint_PHO2)delete[]unknowPoint_PHO2;
	if (knowGROUND)delete[]knowGROUND;
	if (unknowGROUND)delete[]unknowGROUND;
}

void Cvalue::getValue(CString &strinput)
{
	CFileDialog dlgFile(TRUE, _T("txt"), NULL); //创建打开文件对话框
	if (dlgFile.DoModal() == IDCANCEL) return;  //如果选择取消按钮，则退出
	CString strFileName = dlgFile.GetPathName();//获取选择的文件的名称
	//GROUNDnum = 0;
	setlocale(LC_ALL, "");                              //加上这一句,设置语言环境
	//打开文件
	CStdioFile rfile;                                   //定义文件
	if (!rfile.Open(strFileName, CFile::modeRead)) return; //打开文件

	int n;
	CString strLine,strtemp1;                          //变量
	CStringArray strtemp;                     //变量，存储每行分割的结果
	strtemp1.Format(_T("\r\n"));

	BOOL bEOF = rfile.ReadString(strLine);    //读取文件第一行
	strinput += strLine + strtemp1;
	f = _tstof(strLine)/1000.0;          //f

	bEOF = rfile.ReadString(strLine);    //读取文件第二行
	strinput += strLine + strtemp1;
	m = _tstof(strLine);                 //m

	bEOF = rfile.ReadString(strLine);
	strinput += strLine + strtemp1;
	bEOF = rfile.ReadString(strLine);
	strinput += strLine + strtemp1;
	knowPHOTOnum = _ttoi(strLine);
	knowPHOTO1 = new PointPHOTO[knowPHOTOnum];
	knowPHOTO2 = new PointPHOTO[knowPHOTOnum];
	knowGROUND = new Point[knowPHOTOnum];

	//左像点
	for (int i = 0; i < knowPHOTOnum; i++)
	{
		bEOF = rfile.ReadString(strLine);
		strinput += strLine + strtemp1;
		n = SpliteStringArray(strLine, ',', strtemp);
		knowPHOTO1[i].X = _tstof(strtemp[0]) / 1000.0;
		knowPHOTO1[i].Y = _tstof(strtemp[1]) / 1000.0;
	}
	//右像点
	bEOF = rfile.ReadString(strLine);
	strinput += strLine + strtemp1;
	for (int i = 0; i < knowPHOTOnum; i++)
	{
		bEOF = rfile.ReadString(strLine);
		strinput += strLine + strtemp1;
		n = SpliteStringArray(strLine, ',', strtemp);
		knowPHOTO2[i].X = _tstof(strtemp[0]) / 1000.0;
		knowPHOTO2[i].Y = _tstof(strtemp[1]) / 1000.0;
	}

	//控制点
	bEOF = rfile.ReadString(strLine);
	strinput += strLine + strtemp1;
	for (int i = 0; i < knowPHOTOnum; i++)
	{
		bEOF = rfile.ReadString(strLine);
		strinput += strLine + strtemp1;
		n = SpliteStringArray(strLine, ',', strtemp);
		knowGROUND[i].X = _tstof(strtemp[0]);
		knowGROUND[i].Y = _tstof(strtemp[1]);
		knowGROUND[i].Z = _tstof(strtemp[2]);
	}

	//未知点
	bEOF = rfile.ReadString(strLine);
	strinput += strLine + strtemp1;
	bEOF = rfile.ReadString(strLine);
	strinput += strLine + strtemp1;
	unknowPointnum = _ttoi(strLine);  
	unknowPoint_PHO1 = new PointPHOTO[unknowPointnum];
	unknowPoint_PHO2 = new PointPHOTO[unknowPointnum];
	unknowGROUND = new Point[unknowPointnum];

	//左像未知点
	for (int i = 0; i < unknowPointnum; i++)
	{
		bEOF = rfile.ReadString(strLine);
		strinput += strLine + strtemp1;
		n = SpliteStringArray(strLine, ',', strtemp);
		unknowPoint_PHO1[i].X = _tstof(strtemp[0]) / 1000.0;
		unknowPoint_PHO1[i].Y = _tstof(strtemp[1]) / 1000.0;
	}

	//右像未知点
	bEOF = rfile.ReadString(strLine);
	strinput += strLine + strtemp1;
	for (int i = 0; i < unknowPointnum; i++)
	{
		bEOF = rfile.ReadString(strLine);
		strinput += strLine + strtemp1;
		n = SpliteStringArray(strLine, ',', strtemp);
		unknowPoint_PHO2[i].X = _tstof(strtemp[0]) / 1000.0;
		unknowPoint_PHO2[i].Y = _tstof(strtemp[1]) / 1000.0;
	}

	
	rfile.Close();
}

//外方位元素
void Cvalue::calculateFWYS(PointPHOTO * knowPHOTO, Point * knowGROUND, WFWYS &wfwys, JD &temJD, double &a0)
{
	wfwys.XS = 0;
	wfwys.YS = 0;
	wfwys.ZS = 0;
	for (int i = 0; i < knowPHOTOnum; i++)
	{
		wfwys.XS += knowGROUND[i].X;
		wfwys.YS += knowGROUND[i].Y;
		wfwys.ZS += knowGROUND[i].Z;
	}
	//初始化外方位元素
	wfwys.XS = wfwys.XS / knowPHOTOnum;
	wfwys.YS = wfwys.YS / knowPHOTOnum;
	wfwys.ZS = wfwys.ZS / knowPHOTOnum + m*f;
	wfwys.angle_t = 0.0;
	wfwys.angle_w = 0.0;
	wfwys.angle_k = 0.0;
	//A和L
	CMatrix A(2 * knowPHOTOnum, 6);
	CMatrix L(2 * knowPHOTOnum, 1);
	//CMatrix X(2 * knowPHOTOnum, 1);
	while (true)
	{
		//旋转矩阵系数
		double a1 = cos(wfwys.angle_t) * cos(wfwys.angle_k) - sin(wfwys.angle_t) * sin(wfwys.angle_w) * sin(wfwys.angle_k);
		double a2 = -cos(wfwys.angle_t) * sin(wfwys.angle_k) - sin(wfwys.angle_t) * sin(wfwys.angle_w) * cos(wfwys.angle_k);
		double a3 = -sin(wfwys.angle_t) * cos(wfwys.angle_w);
		double b1 = cos(wfwys.angle_w) * sin(wfwys.angle_k);
		double b2 = cos(wfwys.angle_w) * cos(wfwys.angle_k);
		double b3 = -sin(wfwys.angle_w);
		double c1 = sin(wfwys.angle_t) * cos(wfwys.angle_k) + cos(wfwys.angle_t) * sin(wfwys.angle_w) * sin(wfwys.angle_k);
		double c2 = -sin(wfwys.angle_t) * sin(wfwys.angle_k) + cos(wfwys.angle_t) * sin(wfwys.angle_w) * cos(wfwys.angle_k);
		double c3 = cos(wfwys.angle_t) * cos(wfwys.angle_w);

		//给A和L赋值
		for (int i = 0; i < knowPHOTOnum; i++)
		{
			////
			//double ap1 = -f * (a1 * (knowGROUND[i].X - wfwys.XS) + b1 * (knowGROUND[i].Y - wfwys.YS) + c1 * (knowGROUND[i].Z - wfwys.ZS)) /
			//	(a3 * (knowGROUND[i].X - wfwys.XS) + b3 * (knowGROUND[i].Y - wfwys.YS) + c3 * (knowGROUND[i].Z - wfwys.ZS));
			//double ap2 = -f * (a2 * (knowGROUND[i].X - wfwys.XS) + b2 * (knowGROUND[i].Y - wfwys.YS) + c2 * (knowGROUND[i].Z - wfwys.ZS)) /
			//	(a3 * (knowGROUND[i].X - wfwys.XS) + b3 * (knowGROUND[i].Y - wfwys.YS) + c3 * (knowGROUND[i].Z - wfwys.ZS));


			//分母***临时变量
			double Zbar = a3 * (knowGROUND[i].X - wfwys.XS) + b3 * (knowGROUND[i].Y - wfwys.YS) + c3 * (knowGROUND[i].Z - wfwys.ZS);
			//系数矩阵
			A(i * 2, 0) = (a1 * f + a3 * knowPHOTO[i].X) / Zbar;
			A(i * 2, 1) = (b1 * f + b3 * knowPHOTO[i].X) / Zbar;
			A(i * 2, 2) = (c1 * f + c3 * knowPHOTO[i].X) / Zbar;
			A(i * 2, 3) = knowPHOTO[i].Y * sin(wfwys.angle_w) - (knowPHOTO[i].X * (knowPHOTO[i].X * cos(wfwys.angle_k) - knowPHOTO[i].Y * sin(wfwys.angle_k)) / f + f * cos(wfwys.angle_k)) * cos(wfwys.angle_w);
			A(i * 2, 4) = -f * sin(wfwys.angle_k) - knowPHOTO[i].X * (knowPHOTO[i].X * sin(wfwys.angle_k) + knowPHOTO[i].Y * cos(wfwys.angle_k)) / f;
			A(i * 2, 5) = knowPHOTO[i].Y;
			A(i * 2 + 1, 0) = (a2 * f + a3 * knowPHOTO[i].Y) / Zbar;
			A(i * 2 + 1, 1) = (b2 * f + b3 * knowPHOTO[i].Y) / Zbar;
			A(i * 2 + 1, 2) = (c2 * f + c3 * knowPHOTO[i].Y) / Zbar;
			A(i * 2 + 1, 3) = knowPHOTO[i].X * sin(wfwys.angle_w) - (knowPHOTO[i].Y * (knowPHOTO[i].X * cos(wfwys.angle_k) - knowPHOTO[i].Y * sin(wfwys.angle_k)) / f - f * sin(wfwys.angle_k)) * cos(wfwys.angle_w);
			A(i * 2 + 1, 4) = -f * cos(wfwys.angle_k) - knowPHOTO[i].Y * (knowPHOTO[i].X * sin(wfwys.angle_k) + knowPHOTO[i].Y * cos(wfwys.angle_k)) / f;
			A(i * 2 + 1, 5) = -knowPHOTO[i].X;


			//
			double ap1 = -f * (a1 * (knowGROUND[i].X - wfwys.XS) + b1 * (knowGROUND[i].Y - wfwys.YS) + c1 * (knowGROUND[i].Z - wfwys.ZS)) /
				(a3 * (knowGROUND[i].X - wfwys.XS) + b3 * (knowGROUND[i].Y - wfwys.YS) + c3 * (knowGROUND[i].Z - wfwys.ZS));
			double ap2 = -f * (a2 * (knowGROUND[i].X - wfwys.XS) + b2 * (knowGROUND[i].Y - wfwys.YS) + c2 * (knowGROUND[i].Z - wfwys.ZS)) /
				(a3 * (knowGROUND[i].X - wfwys.XS) + b3 * (knowGROUND[i].Y - wfwys.YS) + c3 * (knowGROUND[i].Z - wfwys.ZS));
			L(i * 2, 0) = knowPHOTO[i].X - ap1;
			L(i * 2 + 1, 0) = knowPHOTO[i].Y - ap2;
		}
		CMatrix Naa = ~A*A;    //Naa
		CMatrix fx = ~A*L;     //fx

		CMatrix X = Naa.Inv() * fx;

		wfwys.XS = X(0, 0) + wfwys.XS;
		wfwys.YS = X(1, 0) + wfwys.YS;
		wfwys.ZS = X(2, 0) + wfwys.ZS;
		wfwys.angle_t = X(3, 0) + wfwys.angle_t;
		wfwys.angle_w = X(4, 0) + wfwys.angle_w;
		wfwys.angle_k = X(5, 0) + wfwys.angle_k;

		CMatrix V = A * X - L;
		CMatrix CMtem = ~V ;
		CMatrix a02 = (CMtem * V )*(1.0/(2.0 * knowPHOTOnum - 6));
		double aa = a02(0, 0);
		CMatrix Qxx = Naa.Inv();
		a0 = sqrt(a02(0, 0));      //单位权中误差

		//精度
		temJD.jdXS = a0 * sqrt(Qxx(0, 0));
		temJD.jdYS = a0 * sqrt(Qxx(1, 1));
		temJD.jdZS = a0 * sqrt(Qxx(2, 2));
		temJD.jdangle_t = a0 * sqrt(Qxx(3, 3));
		temJD.jdangle_w = a0 * sqrt(Qxx(4, 4));
		temJD.jdangle_k = a0 *  sqrt(Qxx(5, 5));
		//判断是否满足条件
		if (abs(X(3, 0)) < 0.001 && abs(X(4, 0)) < 0.001 && abs(X(5, 0)) < 0.001)
		{
			break;
		}
		
	}

}

//未知点地面坐标计算
void Cvalue::GROUNDunpointCal(WFWYS wfwys1, WFWYS wfwys2, PointPHOTO * unknowPoint_PHO1, PointPHOTO * unknowPoint_PHO2)
{
	BX = wfwys2.XS - wfwys1.XS;
	BY = wfwys2.YS - wfwys1.YS;
	BZ = wfwys2.ZS - wfwys1.ZS;

	CMatrix R1;  //左片的旋转矩阵
	R1(0, 0) = cos(wfwys1.angle_t)*cos(wfwys1.angle_k);
	R1(0, 1) = -cos(wfwys1.angle_t)*sin(wfwys1.angle_k);
	R1(0, 2) = -sin(wfwys1.angle_t);
	R1(1, 0) = cos(wfwys1.angle_w)*sin(wfwys1.angle_k) - sin(wfwys1.angle_w)*sin(wfwys1.angle_t)*cos(wfwys1.angle_k);
	R1(1, 1) = cos(wfwys1.angle_w)*cos(wfwys1.angle_k) + sin(wfwys1.angle_w)*sin(wfwys1.angle_t)*sin(wfwys1.angle_k);
	R1(1, 2) = -sin(wfwys1.angle_w)*cos(wfwys1.angle_t);
	R1(2, 0) = sin(wfwys1.angle_w)*sin(wfwys1.angle_k) + cos(wfwys1.angle_w)*sin(wfwys1.angle_t)*cos(wfwys1.angle_k);
	R1(2, 1) = sin(wfwys1.angle_w)*cos(wfwys1.angle_k) - cos(wfwys1.angle_w)*sin(wfwys1.angle_t)*sin(wfwys1.angle_k);
	R1(2, 2) = cos(wfwys1.angle_t)*cos(wfwys1.angle_w);

	
	CMatrix R2;   //右片的旋转矩阵
	R2(0, 0) = cos(wfwys2.angle_t)*cos(wfwys2.angle_k);
	R2(0, 1) = -cos(wfwys2.angle_t)*sin(wfwys2.angle_k);
	R2(0, 2) = -sin(wfwys2.angle_t);
	R2(1, 0) = cos(wfwys2.angle_w)*sin(wfwys2.angle_k) - sin(wfwys2.angle_w)*sin(wfwys2.angle_t)*cos(wfwys2.angle_k);
	R2(1, 1) = cos(wfwys2.angle_w)*cos(wfwys2.angle_k) + sin(wfwys2.angle_w)*sin(wfwys2.angle_t)*sin(wfwys2.angle_k);
	R2(1, 2) = -sin(wfwys2.angle_w)*cos(wfwys2.angle_t);
	R2(2, 0) = sin(wfwys2.angle_w)*sin(wfwys2.angle_k) + cos(wfwys2.angle_w)*sin(wfwys2.angle_t)*cos(wfwys2.angle_k);
	R2(2, 1) = sin(wfwys2.angle_w)*cos(wfwys2.angle_k) - cos(wfwys2.angle_w)*sin(wfwys2.angle_t)*sin(wfwys2.angle_k);
	R2(2, 2) = cos(wfwys2.angle_t)*cos(wfwys2.angle_w);

	for (int i = 0; i < unknowPointnum; i++)
	{
		CMatrix TEMP1(3,1);
		TEMP1(0, 0) = unknowPoint_PHO1[i].X;
		TEMP1(1, 0) = unknowPoint_PHO1[i].Y;
		TEMP1(2, 0) = -f;

		CMatrix TEMP2(3, 1);
		TEMP2(0, 0) = unknowPoint_PHO2[i].X;
		TEMP2(1, 0) = unknowPoint_PHO2[i].Y;
		TEMP2(2, 0) = -f;
		//像空间辅助坐标
		CMatrix PHOTO_KJ1 = R1 * TEMP1;
		CMatrix PHOTO_KJ2 = R2 * TEMP2;

		double temp = PHOTO_KJ1(0, 0)*PHOTO_KJ2(2, 0) - PHOTO_KJ1(2, 0)*PHOTO_KJ2(0, 0);//分母
		//投影系数
		double N1 = (BX* PHOTO_KJ2(2, 0) - BZ * PHOTO_KJ2(0, 0)) / temp;
		double N2 = (BX* PHOTO_KJ1(2, 0) - BZ * PHOTO_KJ1(0, 0)) / temp;
		//坐标增量
		double dx = N1 * PHOTO_KJ1(0, 0);
		double dy = 0.5*(N1*PHOTO_KJ1(1, 0) + N2 * PHOTO_KJ2(1, 0) + BY);
		double dz = N1 * PHOTO_KJ1(2, 0);

		//地面点坐标
		unknowGROUND[i].X = wfwys1.XS + dx;
		unknowGROUND[i].Y = wfwys1.YS + dy;
		unknowGROUND[i].Z = wfwys1.ZS + dz;
	}
}

//输出结果
void Cvalue::outputValue(CString &strout)
{
	CFileDialog dlgFile(FALSE, _T("txt"), NULL, OFN_EXPLORER,
		_T("(文本文件)|*.txt"));
	if (dlgFile.DoModal() == IDCANCEL) return;    //如果成功则返回
	CString strFileName2 = dlgFile.GetPathName();
	setlocale(LC_ALL, "");                         //设置环境语言

	//打开文件，写入
	CStdioFile wfile;
	if (!wfile.Open(strFileName2, CFile::modeCreate | CFile::modeWrite)) return;

	//计算外方位元素和未知点坐标
	calculateFWYS(knowPHOTO1, knowGROUND, left, leftJD,a01);  //左
	calculateFWYS(knowPHOTO2, knowGROUND, right, rightJD, a02);  //右
	GROUNDunpointCal(left, right, unknowPoint_PHO1, unknowPoint_PHO2);

	strout.Format(_T("######结果#####\r\n左片外方位元素：\r\n中误差：%.6f\r\nXS=%.3f   精度：%.6f\r\nYS=%.3f   精度：%.6f\r\nZS=%.3f   精度：%.6f\r\nt=%.6f      精度：%.6f\r\nw=%.6f    精度：%.6f\r\nk=%.6f     精度：%.6f"),
		a01, left.XS, leftJD.jdXS, left.YS, leftJD.jdYS, left.ZS, leftJD.jdZS, left.angle_t, leftJD.jdangle_t,left.angle_w,leftJD.jdangle_w,left.angle_k, leftJD.jdangle_k);
	CString out1;
	
	out1.Format(_T("\r\n右片外方位元素：\r\n中误差：%.6f\r\nXS=%.3f   精度：%.6f\r\nYS=%.3f   精度：%.6f\r\nZS=%.3f   精度：%.6f\r\nt=%.6f      精度：%.6f\r\nw=%.6f    精度：%.6f\r\nk=%.6f     精度：%.6f"),
		a02, right.XS, rightJD.jdXS, right.YS, rightJD.jdYS, right.ZS, rightJD.jdZS, right.angle_t, rightJD.jdangle_t, right.angle_w,rightJD.jdangle_w, right.angle_k, rightJD.jdangle_k);
	strout += out1;

	out1.Format(_T("\r\n坐标："));
	strout += out1;
	out1.Format(_T("\r\n\t左片\t\t\t右片\t\t\tX\tY\tZ"));
	strout += out1;
	//for(int i = 0;i<6)
	for (int i = 0; i < knowPHOTOnum; i++)
	{
		CString out;
		out.Format(_T("\r\n%f,%f\t%f,%f\t%f,%f,%f"), knowPHOTO1[i].X*1000, knowPHOTO1[i].Y * 1000, knowPHOTO2[i].X * 1000, knowPHOTO2[i].Y * 1000, knowGROUND[i].X, knowGROUND[i].Y, knowGROUND[i].Z);
		strout += out;
	}
	for (int i = 0; i < unknowPointnum; i++)
	{
		CString out;
		out.Format(_T("\r\n%f,%f\t%f,%f\t%f,%f,%f"), unknowPoint_PHO1[i].X * 1000, unknowPoint_PHO1[i].Y * 1000, unknowPoint_PHO2[i].X * 1000, unknowPoint_PHO2[i].Y * 1000, unknowGROUND[i].X, unknowGROUND[i].Y, unknowGROUND[i].Z);
		strout += out;
	}
	CString str = strout;
	str.Remove('\r');
	wfile.WriteString(str); //数据读入到wfile
	wfile.Close();              //关闭文件
}


//分割函数
int Cvalue::SpliteStringArray(CString str, char split, CStringArray & astr)
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


