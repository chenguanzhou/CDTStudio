#pragma once
#include <QtCore>

typedef std::vector<int> FREECODEARRAY;

typedef struct tagVERTEX2D
{
    double x;
    double y;
}VERTEX2D;

typedef struct tagFREECODEPOLYLINE
{
    VERTEX2D startpoint;
    FREECODEARRAY fca;
}FREECODEPOLYLINE;

class CStatisticSnake  
{
public:
	CStatisticSnake();
	~CStatisticSnake();

    std::vector<VERTEX2D> m_seedArray;//Seed points list for input, result for output

public:
    void Initialize(uchar *pBMPData, int w, int h, int wb,const std::vector<VERTEX2D> &pointVec);
	bool GetStatisticSnake();

private:
    uchar* m_pBMPData;// Image
	bool* m_pShapeImage;
    int m_BMPWidth;
    int m_BMPHeight;
    int m_BMPWidthBytes;

	VERTEX2D m_statistic_start;
	int m_statistic_w;
	int m_statistic_h;
	double* m_statistic_n;
	double* m_staticstic_intensity;
	double* m_staticstic_intensity_sqr;
	FREECODEARRAY m_sgnArray;
	FREECODEPOLYLINE m_freeCodePolyLine;

private:
	bool*	FillSeedArrayRgn(std::vector<VERTEX2D>& seedArray,VERTEX2D& border_start,int& w,int& h);
    double	Statistic_GetIncrement_Polygon(int s,int i,int j,uchar* blockimage,int statistic_w);
	void	Statistic_DeleteNeedle(std::vector<VERTEX2D>& seedArray,double rate);
	void	Statistic_InsertPoint(std::vector<VERTEX2D>& seedArray,double threshold);
	bool	Statistic_SeedLine2FreeCode(FREECODEPOLYLINE& freeCodePolyLine, VERTEX2D* seedpoints,int n);
	void	Statistic_GetSgnArray(FREECODEPOLYLINE& freeCodePolyLine,FREECODEARRAY& sgnArray);
	bool	Statistic_GetStatisticValue(FREECODEARRAY& sgnArray,FREECODEPOLYLINE& freeCodePolyLine,double& increment_n,double& increment_i,double& increment_is);
	int		Statistic_Rnd(int min, int max);
	double	Statistic_Iteration(std::vector<VERTEX2D>& seedArray,FREECODEPOLYLINE& freeCodePolyLine,FREECODEARRAY& sgnArray,double total_n,double total_i,double total_is,double dist,int nIteration);
	void	Statistic_DeletePointByMDL(std::vector<VERTEX2D>& seedArray,double total_n,double total_i,double total_is);
	bool	Statistic_CrossTest(std::vector<VERTEX2D>& seedArray,VERTEX2D v,int num);
	bool	GetFreeCodeLine( FREECODEARRAY& fca,VERTEX2D v1,VERTEX2D v2);
	int		GetFreeCode(int x,int y,int px,int py);
	double	Distance_PointToLine(VERTEX2D p, VERTEX2D lp1, VERTEX2D lp2);
	bool	Statistic_CrossTest(std::vector<VERTEX2D>& seedArray);

};
