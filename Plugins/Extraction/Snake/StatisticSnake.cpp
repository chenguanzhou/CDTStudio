#include "StatisticSnake.h"


CStatisticSnake::CStatisticSnake()
{
	m_pBMPData=NULL;
	m_pShapeImage=NULL;

	m_statistic_n=NULL;
	m_staticstic_intensity=NULL;
	m_staticstic_intensity_sqr=NULL;
}

CStatisticSnake::~CStatisticSnake()
{
	if(m_pShapeImage!=NULL)delete m_pShapeImage;
}

bool CStatisticSnake::GetStatisticSnake()
{
	if(m_pBMPData==NULL)return false;
	if(m_pShapeImage!=NULL)delete m_pShapeImage;
	m_pShapeImage=FillSeedArrayRgn(m_seedArray,m_statistic_start,m_statistic_w,m_statistic_h);
	if(m_pShapeImage==NULL)return false;

    int i,j;

	m_statistic_n=new double[m_statistic_w*m_statistic_h];
	m_staticstic_intensity=new double[m_statistic_w*m_statistic_h];
	m_staticstic_intensity_sqr=new double[m_statistic_w*m_statistic_h];

    uchar* statistic_band=new uchar[m_statistic_w*m_statistic_h];

	for(i=0;i<m_statistic_h;i++)
	{
		for(j=0;j<m_statistic_w;j++)
		{
//			int vh=m_BMPHeight-1-i-m_statistic_start.y;
			int vh = i+m_statistic_start.y;
// 			double r=((double)((m_pBMPData[(int)(vh*m_BMPWidthBytes+(j+m_statistic_start.x)*3)]+
// 				m_pBMPData[(int)(vh*m_BMPWidthBytes+(j+m_statistic_start.x)*3+1)]+
// 				m_pBMPData[(int)(vh*m_BMPWidthBytes+(j+m_statistic_start.x)*3+2)])))/3.0;
 			double r = m_pBMPData[(int)(vh*m_BMPWidthBytes+j+m_statistic_start.x)];
            statistic_band[i*m_statistic_w+j]=(uchar)r;
		}
	}

	for(i=0;i<m_statistic_h;i++)
	{
		for(j=0;j<m_statistic_w;j++)
		{
			m_statistic_n[i*m_statistic_w+j] = Statistic_GetIncrement_Polygon(0,i,j,statistic_band,m_statistic_w);			
			m_staticstic_intensity[i*m_statistic_w+j] = Statistic_GetIncrement_Polygon(1,i,j,statistic_band,m_statistic_w);			
			m_staticstic_intensity_sqr[i*m_statistic_w+j] = Statistic_GetIncrement_Polygon(2,i,j,statistic_band,m_statistic_w);			
		}
	}

	double total_i=0.0;
	double total_is=0.0;
	double total_n=0.0;
	for(i=0;i<m_statistic_h;i++)
	{
		for(j=0;j<m_statistic_w;j++)
		{
			if(!m_pShapeImage[i*m_statistic_w+j])continue;
			double h=statistic_band[i*m_statistic_w+j];
			total_n=total_n+1.0;
			total_i=total_i+h;
			total_is=total_is+h*h;
		}
	}



	///////////////////////////////////////////////////////////////////////
	Statistic_Iteration(m_seedArray,m_freeCodePolyLine,m_sgnArray,total_n,total_i,total_is,20,300);
	////////////////////////////////////////////////	
	Statistic_DeleteNeedle(m_seedArray,0.52);
	Statistic_InsertPoint(m_seedArray,30.0);
	//////////////////////////////////////////////	
	Statistic_Iteration(m_seedArray,m_freeCodePolyLine,m_sgnArray,total_n,total_i,total_is,10,400);
	Statistic_DeleteNeedle(m_seedArray,0.52);
	Statistic_InsertPoint(m_seedArray,10.0);
	Statistic_Iteration(m_seedArray,m_freeCodePolyLine,m_sgnArray,total_n,total_i,total_is,4,500);
	Statistic_DeletePointByMDL(m_seedArray,total_n,total_i,total_is);
	Statistic_DeleteNeedle(m_seedArray,0.174);

	delete statistic_band;
	delete m_statistic_n;
	delete m_staticstic_intensity;
	delete m_staticstic_intensity_sqr;
	return true;
}

bool* CStatisticSnake::FillSeedArrayRgn(std::vector<VERTEX2D> &seedArray, VERTEX2D &border_start, int &w, int &h)
{
	int n=seedArray.size();
	if(n<3)return NULL;
	int border_w;
	int border_h;
	VERTEX2D maxv,minv;
	maxv=seedArray[0];
	minv=maxv;
	int i,j;
	for(i=0;i<n;i++)
	{
		if(maxv.x<seedArray[i].x)maxv.x=seedArray[i].x;
		if(maxv.y<seedArray[i].y)maxv.y=seedArray[i].y;
		if(minv.x>seedArray[i].x)minv.x=seedArray[i].x;
		if(minv.y>seedArray[i].y)minv.y=seedArray[i].y;
	}
	border_start=minv;
	border_w=maxv.x-minv.x+1;
	border_h=maxv.y-minv.y+1;
	///////////////////////////////////////////
	int* borderImage_temp=new int[border_w*border_h];
	for(i=0;i<border_w*border_h;i++)borderImage_temp[i]=255;

	FREECODEPOLYLINE freeCodePolyLine;
	FREECODEARRAY sgnArray;
	VERTEX2D* seed=new VERTEX2D[n];
	for(i=0;i<n;i++)seed[i]=seedArray[i];
	if(!Statistic_SeedLine2FreeCode(freeCodePolyLine, seed,n))return NULL;
	delete seed;
	Statistic_GetSgnArray(freeCodePolyLine,sgnArray);		
	/////////////////////////////////////////////////////////////
	VERTEX2D p=seedArray[0];
	n=sgnArray.size();
	for(i=0;i<n;i++)
	{
		int c=freeCodePolyLine.fca[i];
		int cc=sgnArray[i];
		if(borderImage_temp[(int)((p.y-border_start.y)*border_w+p.x-border_start.x)]==255)
			borderImage_temp[(int)((p.y-border_start.y)*border_w+p.x-border_start.x)]=cc;
		else
			borderImage_temp[(int)((p.y-border_start.y)*border_w+p.x-border_start.x)]+=cc;
		////////////////////////////////////////////////////////////////////////////////////////////////
		switch(c)
		{
		case 0:p.x++;break;
		case 1:p.x++;p.y--;break;
		case 2:p.y--;break;
		case 3:p.x--;p.y--;break;
		case 4:p.x--;break;
		case 5:p.x--;p.y++;break;
		case 6:p.y++;break;
		case 7:p.x++;p.y++;break;
		default:break;
		}
	}	

	bool* borderImage=new bool[border_w*border_h];
	bool change=false;
	for(i=0;i<border_h;i++)
	{
		change=false;
		for(j=0;j<border_w;j++)
		{
			borderImage[i*border_w+j]=false;
			if(borderImage_temp[i*border_w+j]==1||borderImage_temp[i*border_w+j]==-1)change=!change;
			if(change)
			{
				borderImage[i*border_w+j]=true;
			}
		}
	}
	//////////////////////////////////////////////////////////////
	delete borderImage_temp;
	w=border_w;
	h=border_h;
	return borderImage;
}

double CStatisticSnake::Statistic_GetIncrement_Polygon(int s, int i, int j, uchar *blockimage, int statistic_w)
{
	if(m_pShapeImage==NULL)return 0.0;
	double rtn=0.0;
	int k;
	switch(s)
	{
	case 0:
		rtn=j+1;
		break;
	case 1:
		for(k=0;k<=j;k++)
		{
			rtn=rtn+blockimage[i*statistic_w+k];
		}
		break;
	case 2:
		for(k=0;k<=j;k++)
		{
			rtn=rtn+blockimage[i*statistic_w+k]*blockimage[i*statistic_w+k];
		}
		break;
	case 3:
		for(k=0;k<=j;k++)
		{
			double x=blockimage[i*statistic_w+k];
			rtn=rtn+x*pow(2.71828,-x*x/2.0);
		}
		break;
	case 4:
		for(k=0;k<=j;k++)
		{
			double x=blockimage[i*statistic_w+k];
			rtn=rtn+pow(2.71828,-x*x/2.0);
		}
		break;
	case 5:
		for(k=0;k<=j;k++)
		{
			double x=blockimage[i*statistic_w+k];
			rtn=rtn+fabs(x);
		}
		break;
	case 6:
		for(k=0;k<=j;k++)
		{
			double x=blockimage[i*statistic_w+k];
			rtn=rtn+log(cosh(x));
		}
		break;
	default:break;
	}
	return rtn;
}

double CStatisticSnake::Statistic_Iteration(std::vector<VERTEX2D>& seedArray,
	FREECODEPOLYLINE& freeCodePolyLine,
	FREECODEARRAY& sgnArray,
	double total_n,double total_i,double total_is,
	double dist,int nIteration)
{
	double increment_n=0.0;
	double increment_i=0.0;
	double increment_is=0.0;
	double increment_n_save=0.0;
	double increment_i_save=0.0;
	double increment_is_save=0.0;
	double increment_n_temp=0.0;
	double increment_i_temp=0.0;
	double increment_is_temp=0.0;
	int i;

	int n=seedArray.size();
	VERTEX2D* seed=new VERTEX2D[n];
	for(i=0;i<n;i++)
	{
		seed[i]=seedArray[i];
	}
	Statistic_SeedLine2FreeCode(freeCodePolyLine,seed,n);
	delete seed;

	Statistic_GetSgnArray(freeCodePolyLine,sgnArray);		
	Statistic_GetStatisticValue(sgnArray,freeCodePolyLine,increment_n,increment_i,increment_is);
	if(increment_n<0)
	{
		increment_n_temp=-increment_n;
		increment_i_temp=-increment_i;
		increment_is_temp=-increment_is;
	}
	else
	{
		increment_n_temp=increment_n;
		increment_i_temp=increment_i;
		increment_is_temp=increment_is;
	}

	///////////////////////////////////////////////////////////////////	
	double aa1=increment_is_temp/increment_n_temp-(increment_i_temp*increment_i_temp)/(increment_n_temp*increment_n_temp);
	double aa2_n=total_n-increment_n_temp;
	double aa2_is=total_is-increment_is_temp;
	double aa2_i=total_i-increment_i_temp;
	double aa2=aa2_is/aa2_n-(aa2_i*aa2_i)/(aa2_n*aa2_n);

	double Statistic_J;

	if(total_n/aa2_n>100)
		Statistic_J=increment_n_temp/2.0*log(aa1);
	else if(total_n/increment_n_temp>100)
		Statistic_J=aa2_n/2.0*log(aa2);
	else 
		Statistic_J=increment_n_temp/2.0*log(aa1)+aa2_n/2.0*log(aa2)+increment_n_temp*dist/total_n;

	double Prev_J,Next_J;
	Prev_J=Statistic_J;
	int debugn=0;
	int count=seedArray.size();
	/////////////////////////////////
	for(;;)
	{
		debugn++;
		if(debugn>nIteration)break;

		int num=Statistic_Rnd(0,seedArray.size());


		VERTEX2D v[3];

		v[0]=seedArray[(num-1<0?count-1:num-1)];
		v[1]=seedArray[num];
		v[2]=seedArray[(num+1>count-1?0:num+1)];
		FREECODEPOLYLINE freeCodePolyLine1;
		FREECODEARRAY sgnArray1;
		Statistic_SeedLine2FreeCode(freeCodePolyLine1,v,3);
		freeCodePolyLine1.fca[0]; 
		Statistic_GetSgnArray(freeCodePolyLine1,sgnArray1);
		freeCodePolyLine1.fca[0]; 

		increment_n_save=increment_n;
		increment_i_save=increment_i;
		increment_is_save=increment_is;

		increment_n_temp=0.0;
		increment_i_temp=0.0;
		increment_is_temp=0.0;
		Statistic_GetStatisticValue(sgnArray1,freeCodePolyLine1,increment_n_temp,increment_i_temp,increment_is_temp);
		increment_n=increment_n-increment_n_temp;
		increment_i=increment_i-increment_i_temp;
		increment_is=increment_is-increment_is_temp;

		v[1]=seedArray[num];
		v[1].x=v[1].x+Statistic_Rnd(-dist,dist);
		v[1].y=v[1].y+Statistic_Rnd(-dist,dist);
		if(v[1].x<m_statistic_start.x||v[1].x>=m_statistic_start.x+m_statistic_w)
		{
			increment_n=increment_n_save;
			increment_i=increment_i_save;
			increment_is=increment_is_save;
			continue;
		}
		if(v[1].y<m_statistic_start.y||v[1].y>=m_statistic_start.y+m_statistic_h)
		{
			increment_n=increment_n_save;
			increment_i=increment_i_save;
			increment_is=increment_is_save;
			continue;
		}
		if(!m_pShapeImage[(int)((v[1].y-m_statistic_start.y)*m_statistic_w+v[1].x-m_statistic_start.x)])
		{
			increment_n=increment_n_save;
			increment_i=increment_i_save;
			increment_is=increment_is_save;
			continue;
		}
		if(Statistic_CrossTest(seedArray,v[1],num))
		{
			increment_n=increment_n_save;
			increment_i=increment_i_save;
			increment_is=increment_is_save;
			continue;
		}
		v[0]=seedArray[(num-1<0?count-1:num-1)];
		v[2]=seedArray[(num+1>count-1?0:num+1)];
		FREECODEPOLYLINE freeCodePolyLine2;
		FREECODEARRAY sgnArray2;
		Statistic_SeedLine2FreeCode(freeCodePolyLine2,v,3);
		Statistic_GetSgnArray(freeCodePolyLine2,sgnArray2);
		increment_n_temp=0.0;
		increment_i_temp=0.0;
		increment_is_temp=0.0;
		Statistic_GetStatisticValue(sgnArray2,freeCodePolyLine2,increment_n_temp,increment_i_temp,increment_is_temp);
//		if(increment_n_temp<=0)
//			int test=0;
		increment_n=increment_n+increment_n_temp;
		increment_i=increment_i+increment_i_temp;
		increment_is=increment_is+increment_is_temp;

		if(increment_n<0)
		{
			increment_n_temp=-increment_n;
			increment_i_temp=-increment_i;
			increment_is_temp=-increment_is;
		}
		else
		{
			increment_n_temp=increment_n;
			increment_i_temp=increment_i;
			increment_is_temp=increment_is;
		}
		aa1=increment_is_temp/increment_n_temp-(increment_i_temp*increment_i_temp)/(increment_n_temp*increment_n_temp);
		aa2_n=total_n-increment_n_temp;
		aa2_is=total_is-increment_is_temp;
		aa2_i=total_i-increment_i_temp;
		aa2=aa2_is/aa2_n-(aa2_i*aa2_i)/(aa2_n*aa2_n);

		if(total_n/aa2_n>100)
			Next_J=increment_n_temp/2.0*log(aa1+0.1);
		else if(total_n/increment_n_temp>100)
			Next_J=aa2_n/2.0*log(aa2+0.1);
		else 

			Next_J=increment_n_temp/2.0*log(aa1+0.1)+aa2_n/2.0*log(aa2+0.1);

		if(Next_J<Prev_J)
		{
			Prev_J=Next_J;
			seedArray[num] = v[1];
		}
		else
		{
			increment_n=increment_n_save;
			increment_i=increment_i_save;
			increment_is=increment_is_save;
		}
	}
	return Prev_J;
}

void CStatisticSnake::Statistic_DeleteNeedle(std::vector<VERTEX2D>& seedArray,double rate)
{
	int i;
	int n=seedArray.size();
//	double area;
	double da,db;
	for(i=0;i<n;i++)
	{
		if(n<4)break;
		VERTEX2D v1=seedArray.at(i-1<0?n-1:i-1);
		VERTEX2D v2=seedArray[i];
		VERTEX2D v3=seedArray.at(i+1==n?0:i+1);
		da=atan2(v1.y-v2.y,v1.x-v2.x);
		db=atan2(v3.y-v2.y,v3.x-v2.x);
		if(fabs(db-da)<rate)
		{
			seedArray.erase(seedArray.begin()+i);
			n--;
			i--;
		}
	}
}

void CStatisticSnake::Statistic_InsertPoint(std::vector<VERTEX2D>& seedArray,double threshold)
{
	int i,n;
	n=seedArray.size();
	bool ok=true;
	VERTEX2D v,v1,v2;
	for(;;)
	{
		ok=true;
		n=seedArray.size();
		for(i=0;i<n;i++)
		{
			v1=seedArray[i];
			v2=seedArray.at(i+1>n-1?0:i+1);
			double dist=sqrt((v1.x-v2.x)*(v1.x-v2.x)+(v1.y-v2.y)*(v1.y-v2.y));
			if(dist>threshold)
			{
				ok=false;
				v.x=int((v1.x+v2.x)/2.0);
				v.y=int((v1.y+v2.y)/2.0);
//				seedArray.InsertAt(i+1>n-1?0:i+1,v);
				seedArray.insert(seedArray.begin()+((i+1)>(n-1)?0:i+1),v);
				break;
			}
		}
		if(ok)break;
	}
}

bool CStatisticSnake::Statistic_SeedLine2FreeCode(FREECODEPOLYLINE& freeCodePolyLine, VERTEX2D* seedpoints,int n)
{
	bool rtn=true;
	int i;
	if(freeCodePolyLine.fca.size()>0)freeCodePolyLine.fca.clear(); 
	freeCodePolyLine.startpoint = seedpoints[0];
	FREECODEARRAY fra;
	for(i=0;i<n-1;i++)
	{
		if(!GetFreeCodeLine(fra,seedpoints[i],seedpoints[i+1]))
		{
			rtn=false;
			break;
		}
		freeCodePolyLine.fca.insert(freeCodePolyLine.fca.end(),fra.begin(),fra.end());
		fra.clear();
	}
	fra.clear();
	if(!GetFreeCodeLine(fra,seedpoints[i],seedpoints[0]))
	{
		rtn=false;
	}
	else
	{
		freeCodePolyLine.fca.insert(freeCodePolyLine.fca.end(),fra.begin(),fra.end());
	}
	return rtn;
}

void CStatisticSnake::Statistic_GetSgnArray(FREECODEPOLYLINE& freeCodePolyLine,FREECODEARRAY& sgnArray)
{
	int n=freeCodePolyLine.fca.size();
	int i;
	int s=1;
	if(sgnArray.size()>0)sgnArray.clear();
	for(i=0;i<n;i++)
	{
		int t1=(i-1<0?n-1:i-1);
		int t2=i;
		int c1=freeCodePolyLine.fca[t1];
		int c2=freeCodePolyLine.fca[t2];
		if(c1==0&&c2==5)sgnArray.push_back(-1*s);
		else if(c1==0&&c2==6)sgnArray.push_back(-1*s);
		else if(c1==0&&c2==7)sgnArray.push_back(-1*s);
		else if(c1==1&&c2==0)sgnArray.push_back(1*s);
		else if(c1==1&&c2==1)sgnArray.push_back(1*s);
		else if(c1==1&&c2==2)sgnArray.push_back(1*s);
		else if(c1==1&&c2==3)sgnArray.push_back(1*s);
		else if(c1==1&&c2==4)sgnArray.push_back(1*s);
		else if(c1==2&&c2==0)sgnArray.push_back(1*s);
		else if(c1==2&&c2==1)sgnArray.push_back(1*s);
		else if(c1==2&&c2==2)sgnArray.push_back(1*s);
		else if(c1==2&&c2==3)sgnArray.push_back(1*s);
		else if(c1==2&&c2==4)sgnArray.push_back(1*s);
		else if(c1==3&&c2==0)sgnArray.push_back(1*s);
		else if(c1==3&&c2==1)sgnArray.push_back(1*s);
		else if(c1==3&&c2==2)sgnArray.push_back(1*s);
		else if(c1==3&&c2==3)sgnArray.push_back(1*s);
		else if(c1==3&&c2==4)sgnArray.push_back(1*s);
		else if(c1==4&&c2==5)sgnArray.push_back(-1*s);
		else if(c1==4&&c2==6)sgnArray.push_back(-1*s);
		else if(c1==4&&c2==7)sgnArray.push_back(-1*s);
		else if(c1==5&&c2==5)sgnArray.push_back(-1*s);
		else if(c1==5&&c2==6)sgnArray.push_back(-1*s);
		else if(c1==5&&c2==7)sgnArray.push_back(-1*s);
		else if(c1==6&&c2==5)sgnArray.push_back(-1*s);
		else if(c1==6&&c2==6)sgnArray.push_back(-1*s);
		else if(c1==6&&c2==7)sgnArray.push_back(-1*s);
		else if(c1==7&&c2==5)sgnArray.push_back(-1*s);
		else if(c1==7&&c2==6)sgnArray.push_back(-1*s);
		else if(c1==7&&c2==7)sgnArray.push_back(-1*s);
		else sgnArray.push_back(0);
	}
}

bool CStatisticSnake::Statistic_GetStatisticValue(FREECODEARRAY& sgnArray,
	FREECODEPOLYLINE& freeCodePolyLine,
	double& increment_n,
	double& increment_i,
	double& increment_is)
{
	int i,n,c;
	n=sgnArray.size();
	VERTEX2D sv=freeCodePolyLine.startpoint;
	for(i=0;i<n;i++)
	{
		double sc=(double)sgnArray[i];
		if(sc==1||sc==-1)
		{
			increment_n=increment_n+sc*m_statistic_n[(int)((sv.y-m_statistic_start.y)*m_statistic_w+sv.x-m_statistic_start.x)];
			increment_i=increment_i+sc*m_staticstic_intensity[(int)((sv.y-m_statistic_start.y)*m_statistic_w+sv.x-m_statistic_start.x)];
			increment_is=increment_is+sc*m_staticstic_intensity_sqr[(int)((sv.y-m_statistic_start.y)*m_statistic_w+sv.x-m_statistic_start.x)];

		}
		c=freeCodePolyLine.fca[i];
		switch(c)
		{
		case 0:sv.x++;break;
		case 1:sv.x++;sv.y--;break;
		case 2:sv.y--;break;
		case 3:sv.x--;sv.y--;break;
		case 4:sv.x--;break;
		case 5:sv.x--;sv.y++;break;
		case 6:sv.y++;break;
		case 7:sv.x++;sv.y++;break;
		default:break;
		}
	}
	return true;
}

int CStatisticSnake::Statistic_Rnd(int min, int max)
{
	int i;
	double s=rand()%1000/1000.0;
	i=(int)((double)(s*((double)max-(double)min))+min);
	return i;
}

void CStatisticSnake::Statistic_DeletePointByMDL(std::vector<VERTEX2D>& seedArray,
	double total_n,double total_i,double total_is)
{
//	int ntest=999;
	int i;
	double JMDL,J,JMDL_temp,JMDL_MIN;
	double N=m_statistic_w*m_statistic_h;
	int k;
	bool ok=true;
	double increment_n=0.0;
	double increment_i=0.0;
	double increment_is=0.0;
	double increment_n_temp=0.0;
	double increment_i_temp=0.0;
	double increment_is_temp=0.0;
	double increment_n_save=0.0;
	double increment_i_save=0.0;
	double increment_is_save=0.0;
	double increment_n_save_min=0.0;
	double increment_i_save_min=0.0;
	double increment_is_save_min=0.0;
	VERTEX2D v[3];
	int n=seedArray.size();
	VERTEX2D* seed=new VERTEX2D[n];
	for(i=0;i<n;i++)
	{
		seed[i]=seedArray[i];
	}
	FREECODEPOLYLINE freeCodePolyLine_temp;
	FREECODEARRAY sgnArray_temp;
	Statistic_SeedLine2FreeCode(freeCodePolyLine_temp,seed,n);
	delete seed;

	Statistic_GetSgnArray(freeCodePolyLine_temp,sgnArray_temp);		
	Statistic_GetStatisticValue(sgnArray_temp,freeCodePolyLine_temp,increment_n,increment_i,increment_is);
	if(increment_n<0)
	{
		increment_n_temp=-increment_n;
		increment_i_temp=-increment_i;
		increment_is_temp=-increment_is;
	}
	///////////////////////////////////////////////////////////////////	
	double aa1=increment_is_temp/increment_n_temp-(increment_i_temp*increment_i_temp)/(increment_n_temp*increment_n_temp);
	double aa2_n=total_n-increment_n_temp;
	double aa2_is=total_is-increment_is_temp;
	double aa2_i=total_i-increment_i_temp;
	double aa2=aa2_is/aa2_n-(aa2_i*aa2_i)/(aa2_n*aa2_n);

	J=increment_n_temp/2.0*log(aa1)+aa2_n/2.0*log(aa2);
	JMDL=J+n*log(N);
	JMDL_temp=JMDL;
	JMDL_MIN=JMDL_temp;
//	int nn=n;
//	int step=0;

	for(;;)
	{
		if(JMDL_temp>JMDL_MIN)
		{
			if(!Statistic_CrossTest(seedArray))
				break;
		}
		else
		{
			JMDL_MIN=JMDL_temp;
		}
		if(n<5)
			break;
		///////////////////////////////////////////////////////
		ok=true;
		increment_n_save=increment_n;
		increment_i_save=increment_i;
		increment_is_save=increment_is;

		for(i=0;i<n;i++)
		{

			increment_n=increment_n_save;
			increment_i=increment_i_save;
			increment_is=increment_is_save;

			int num=i;
			v[0]=seedArray.at((num-1<0?n-1:num-1));
			v[1]=seedArray[num];
			v[2]=seedArray.at((num+1>n-1?0:num+1));

			FREECODEPOLYLINE freeCodePolyLine;
			FREECODEARRAY sgnArray;
			Statistic_SeedLine2FreeCode(freeCodePolyLine,v,3);
			Statistic_GetSgnArray(freeCodePolyLine,sgnArray);

			increment_n_temp=0.0;
			increment_i_temp=0.0;
			increment_is_temp=0.0;
			Statistic_GetStatisticValue(sgnArray,freeCodePolyLine,increment_n_temp,increment_i_temp,increment_is_temp);
			increment_n=increment_n-increment_n_temp;
			increment_i=increment_i-increment_i_temp;
			increment_is=increment_is-increment_is_temp;

			if(increment_n<0)
			{
				increment_n_temp=-increment_n;
				increment_i_temp=-increment_i;
				increment_is_temp=-increment_is;
			}
			else
			{
				increment_n_temp=increment_n;
				increment_i_temp=increment_i;
				increment_is_temp=increment_is;
			}
			aa1=increment_is_temp/increment_n_temp-(increment_i_temp*increment_i_temp)/(increment_n_temp*increment_n_temp);
			aa2_n=total_n-increment_n_temp;
			aa2_is=total_is-increment_is_temp;
			aa2_i=total_i-increment_i_temp;
			aa2=aa2_is/aa2_n-(aa2_i*aa2_i)/(aa2_n*aa2_n);

			J=increment_n_temp/2.0*log(aa1)+aa2_n/2.0*log(aa2);
			JMDL=J+(n-1)*log(N);

			if(JMDL<=JMDL_temp)
			{
				JMDL_temp=JMDL;
				k=i;
				ok=false;
//				ntest=increment_n_temp;
				increment_n_save_min=increment_n;
				increment_i_save_min=increment_i;
				increment_is_save_min=increment_is;
			}
		}
		if(ok)
			break;
		seedArray.erase(seedArray.begin()+k);
		n--;
		i--;
		increment_n=increment_n_save_min;
		increment_i=increment_i_save_min;
		increment_is=increment_is_save_min;
	}
}

bool CStatisticSnake::Statistic_CrossTest(std::vector<VERTEX2D>& seedArray,VERTEX2D v,int num)
{
	int n=seedArray.size();
	int num1=num-1<0?n-1:num-1;
	VERTEX2D v1=seedArray.at(num1);
	VERTEX2D vmax,vmin;
	int i;
	if(v.x>v1.x)
	{
		vmax.x=v.x;
		vmin.x=v1.x;
	}
	else
	{
		vmax.x=v1.x;
		vmin.x=v.x;
	}
	if(v.y>v1.y)
	{
		vmax.y=v.y;
		vmin.y=v1.y;
	}
	else
	{
		vmax.y=v1.y;
		vmin.y=v.y;
	}
	for(i=0;i<n;i++)
	{
		if(i==(num1-1<0?n-1:num1-1))continue;//
		if(i==num1)continue;
		if(i==num)continue;//
		VERTEX2D sv1=seedArray[i];
		VERTEX2D sv2=seedArray.at(i+1>n-1?0:i+1);
		if(sv1.x>=vmax.x&&sv2.x>=vmax.x)continue;
		if(sv1.x<=vmin.x&&sv2.x<=vmin.x)continue;
		if(sv1.y>=vmax.y&&sv2.y>=vmax.y)continue;
		if(sv1.y<=vmin.y&&sv2.y<=vmin.y)continue;
		double dist1=Distance_PointToLine(v, sv1, sv2);
		double dist2=Distance_PointToLine(v1, sv1, sv2);
		double rtn1=dist1*dist2;

		dist1=Distance_PointToLine(sv1, v, v1);
		dist2=Distance_PointToLine(sv2, v, v1);
		double rtn2=dist1*dist2;
		if(rtn1<0&&rtn2<0)return true;
		if(rtn1==0||rtn2==0)return true;

	}
	int num2=num+1>n-1?0:num+1;
	v1=seedArray.at(num2);
	if(v.x>v1.x)
	{
		vmax.x=v.x;
		vmin.x=v1.x;
	}
	else
	{
		vmax.x=v1.x;
		vmin.x=v.x;
	}
	if(v.y>v1.y)
	{
		vmax.y=v.y;
		vmin.y=v1.y;
	}
	else
	{
		vmax.y=v1.y;
		vmin.y=v.y;
	}
	for(i=0;i<n;i++)
	{
		if(i==num1)continue;
		if(i==num)continue;
		if(i==num2)continue;
		VERTEX2D sv1=seedArray[i];
		VERTEX2D sv2=seedArray.at(i+1>n-1?0:i+1);
		if(sv1.x>=vmax.x&&sv2.x>=vmax.x)continue;
		if(sv1.x<=vmin.x&&sv2.x<=vmin.x)continue;
		if(sv1.y>=vmax.y&&sv2.y>=vmax.y)continue;
		if(sv1.y<=vmin.y&&sv2.y<=vmin.y)continue;
		double dist1=Distance_PointToLine(v, sv1, sv2);
		double dist2=Distance_PointToLine(v1, sv1, sv2);
		double rtn1=dist1*dist2;
		dist1=Distance_PointToLine(sv1, v, v1);
		dist2=Distance_PointToLine(sv2, v, v1);
		double rtn2=dist1*dist2;
		if(rtn1<0&&rtn2<0)return true;
		if(rtn1<0&&rtn2<0)return true;
		if(rtn1==0||rtn2==0)return true;
	}
	return false;
}

bool CStatisticSnake::GetFreeCodeLine( FREECODEARRAY& fca,VERTEX2D v1,VERTEX2D v2)
{
	bool rtn=true;
	float dx,dy,x,y;
	float xIncerement,yIncerement;
	float px,py;
	int step,k;
	dx=v2.x-v1.x;
	dy=v2.y-v1.y;
	x=v1.x;
	y=v1.y;

	int c=0;
	if(abs(dx)>abs(dy)) 
		step=abs(dx); 
	else
		step=abs(dy); 
	xIncerement=dx/(float)step;
	yIncerement=dy/(float)step; 
	px=v1.x;
	py=v1.y;
	for (k=0;k<step;k++)
	{ 
		x+=xIncerement; 
		y+=yIncerement; 
//		if(k==step-1)
//			int test=0;
		c=GetFreeCode((x+0.5),(y+0.5),(px+0.5),(py+0.5));
		if(c<0)
		{
			rtn=false;
			break;
		}
		fca.push_back(c);
		px=x;
		py=y;	
	}
	if(!rtn)fca.clear();
	return rtn;
}

int CStatisticSnake::GetFreeCode(int x,int y,int px,int py)
{
	int a,b;
	a=x-px;
	b=y-py;

	if(a==1&&b==0)return 0;
	if(a==1&&b==-1)return 1;
	if(a==0&&b==-1)return 2;
	if(a==-1&&b==-1)return 3;
	if(a==-1&&b==0)return 4;
	if(a==-1&&b==1)return 5;
	if(a==0&&b==1)return 6;
	if(a==1&&b==1)return 7;

	if(a==2&&b==0)return 0;
	if(a==2&&b==-2)return 1;
	if(a==0&&b==-2)return 2;
	if(a==-2&&b==-2)return 3;
	if(a==-2&&b==0)return 4;
	if(a==-2&&b==2)return 5;
	if(a==0&&b==2)return 6;
	if(a==2&&b==2)return 7;

	if(a==2&&b==1)return 0;
	if(a==2&&b==-2)return 1;
	if(a==1&&b==-2)return 2;
	if(a==-2&&b==-2)return 3;
	if(a==-2&&b==1)return 4;
	if(a==-2&&b==2)return 5;
	if(a==1&&b==2)return 6;
	if(a==2&&b==2)return 7;

	if(a==2&&b==-1)return 0;
	if(a==2&&b==-2)return 1;
	if(a==-1&&b==-2)return 2;
	if(a==-2&&b==-2)return 3;
	if(a==-2&&b==-1)return 4;
	if(a==-2&&b==2)return 5;
	if(a==-1&&b==2)return 6;
	if(a==2&&b==2)return 7;

	//////////////////////////for debug
	//	CString str;
	//	str.Format("%d %d %d %d",x,y,px,py);
	//	AfxMessageBox(str);
	return -1;
}

double CStatisticSnake::Distance_PointToLine(VERTEX2D p, VERTEX2D lp1, VERTEX2D lp2)
{
	double A=lp2.y-lp1.y;
	double B=-(lp2.x-lp1.x);
	double C=-B*lp1.y-A*lp1.x;
	double distance=(A*p.x+B*p.y+C)/sqrt(A*A+B*B);
	return distance;
}

bool CStatisticSnake::Statistic_CrossTest(std::vector<VERTEX2D>& seedArray)
{
    for(size_t i=0;i<seedArray.size();i++)
	{
		VERTEX2D v=seedArray[i];
		bool test=Statistic_CrossTest(seedArray,v,i);
		if(test)
			return true;
	}
	return false;
}

void CStatisticSnake::Initialize(uchar *pBMPData, int w, int h, int wb,const std::vector<VERTEX2D> &pointVec)
{
	m_pBMPData=	pBMPData;
	m_BMPWidth=w;
	m_BMPHeight=h;
	m_BMPWidthBytes=wb;
	m_seedArray.clear();
	VERTEX2D v;

    for(size_t i=0;i<pointVec.size();i++)
	{
		v=pointVec[i];
		v.x=(int)(v.x+0.5);
		v.y=(int)(v.y+0.5);
		m_seedArray.push_back(v);
	}
}
