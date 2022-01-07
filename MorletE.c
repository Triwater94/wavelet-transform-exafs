#include <stdio.h>
#include <complex.h>
#include <math.h>
#include <windows.h>

#define data "data.txt"	//定义输入文件名称
#define result "result.txt"	//定义输出文件名称
#define pai 3.141592653	//定义pai 

/*-----------------------------------------------------------*/
void color(short x)	//自定义函根据参数改变颜色 https://blog.csdn.net/qq_31975227/article/details/51758461
{
    if(x>=0 && x<=15)//参数在0-15的范围颜色
    	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), x);	//只有一个参数，改变字体颜色 
    else//默认的颜色白色
    	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
}
/*
颜色函数SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),前景色 | 背景色 | 前景加强 | 背景加强);
	前景色：数字0-15 或 FOREGROUND_XXX 表示	（其中XXX可用BLUE、RED、GREEN表示） 
	前景加强：数字8 或 FOREGROUND_INTENSITY 表示
	背景色：数字16 32 64 或 BACKGROUND_XXX 三种颜色表示 
	背景加强： 数字128 或 BACKGROUND_INTENSITY 表示
主要应用：改变指定区域字体与背景的颜色
前景颜色对应值： 
　　0=黑色                8=灰色　　
  　1=蓝色                9=淡蓝色        十六进制        　　                        
　　2=绿色                10=淡绿色       0xa        　　
　　3=湖蓝色              11=淡浅绿色     0xb　
　　4=红色                12=淡红色       0xc　　
　　5=紫色                13=淡紫色       0xd        　　
　　6=黄色                14=淡黄色       0xe        　　
　　7=白色                15=亮白色       0xf 
　　也可以把这些值设置成常量。
https://blog.csdn.net/qq_31975227/article/details/51758461
*/
/*-----------------------------------------------------------*/
complex float waveletTrans(float, float, float, complex float);  //定义小波变换单项公式 
complex float morlet(float, float, float, float, float);	//定义morlet函数 

int
main()

{
	complex float w=0.0 + _Complex_I * 0.0; //小波变换(Morlet)系数（即变换的数值）
	float a,b; //小波变换伸缩因子（a）和位移(b) 
	float da;	//a和b的取点间隔 
	float db; 
	float amin;
	float amax;
	float bmin;
	float bmax;
	
	//设置希望输出的r的范围 
	float rmax;
	float rmin;
	
 	float k; //EXAFS的k（波数）
 	float x; //EXAFS的数值（x(k)*k^n) 
 	float ki[1000]={0.0};	//存放输入数据的K 
 	float xi[1000]={0.0};	//存放输入数据的x(k)k^n 
	float r; //EXAFS傅里叶变换后的R（距离）
	float k1,k2,dk; //EXAFS的k之间的数据间隔(dk=k2-k1)
	float kmax; 
	float kminInput;
	float kmaxInput;
	
	int i=-1;
	int n=0; //定义数据的个数 
	int m=100; //小波变换后，r和k的取值点数 
	
	//Morlet的两个系数（默认值）; 
	float eta=5.0;
	float sigma=1.0; 
	 	
	FILE *f_data, *f_result; //定义导入导出文件指针 

	color(11);
	printf("MorletE");
	color(16);
	printf(", a Wavelet transform (Morlet wavelet) program for EXAFS (data from ");
	color(14);
	printf("Athena");
	color(16);
	printf(").\n");
	printf("Designed by ");
	color(11);
	printf("WANG Miao");
	color(16);
	printf(", Kyoto, Japan, 2019.11.05\n");
	printf("Reference: Physical Review B, 2005, 71(9), 094110.\n");
	printf("--------------------------------------------------\n");
	printf("If you find any bug, please send an email (titled by ");
	color(11);
	printf("Bug for MorletE");
	color(16);
	printf(") to: ");
	color(11);
	printf("wangmiaow@stu.xjtu.edu.cn\n");
	color(16);
	printf("Many thanks~\n");
	printf("--------------------------------------------------\n");

	color(14);	//文字亮黄色 
	printf("!!!NOTICE!!!");
	color(16);	//回到原来颜色 
	printf("\nBefore starting, use a pure data file (named by ");
	color(14);
	printf("data.txt");
	color(16);
	printf(") for your EXAFS result.\nThe 1st column is for k (equal interval, in order), the 2nd column is for x(k)k^n.\n");
	printf("such as:\n\n");
	printf("3.30	0.56776\n");
	printf("3.35	0.89253\n");
	printf("3.40	1.24951\n");
	printf("...	...\n\n");
	printf("Then, put the ");
	color(14);
	printf("data.txt");
	color(16);
	printf(" in the same directory as this program. The program will read the data automatically.\n");
	printf("--------------------------------------------------\n");
	//用户输入eta
	color(14);
	printf("ETA");
	color(16);
	printf(" (if not sure, input 5)=");
	scanf("%f", &eta);
	fflush(stdin);	//刷新键盘缓冲区 
	printf("\n");
	while (eta <= 0)
	{
		printf("**************************\n");
		printf("ETA must be larger than 0.\n");
		printf("**************************\n");
		printf("\n");
		color(14);
		printf("ETA");
		color(16);
		printf(" (if not sure, input 5)=");
		scanf("%f", &eta);
		fflush(stdin);	//刷新键盘缓冲区
		printf("\n");
	} 
	//用户输入sigma 
	color(14);
	printf("SIGMA");
	color(16);
	printf(" (if not sure, input 1)=");
	scanf("%f", &sigma);
	fflush(stdin);	//刷新键盘缓冲区
	printf("\n");
	while (sigma <= 0)
	{
		printf("****************************\n");
		printf("SIGMA must be larger than 0.\n");
		printf("****************************\n");
		printf("\n");
		color(14);
		printf("SIGMA");
		color(16);
		printf(" (if not sure, input 1)=");
		scanf("%f", &sigma);
		fflush(stdin);	//刷新键盘缓冲区
		printf("\n");		
	}
		
	//打开data数据文件，同时检测文件存在与否，先确定数据间隔dk 
	if(!(f_data = fopen(data, "r")))
	{
		printf("Cannot open file, please put the data.txt in the same file directory with the program.");
		getch();
		exit(0);
	}
			
	//储存data中的k的数据间隔、最小值、最大值 
	if (!feof(f_data))	//文件未结束，则feof()返回0（即“假”)；文件结束，返回非0值（即“真”） 
	{
	fscanf(f_data, "%f", &k1);	//第一列第一行，储存在k中
	fscanf(f_data, "%f", &x);	//第二列，储存在x中（过渡一下而已） 
	fscanf(f_data, "%f", &k2);	//第一列第二行，储存在x中
	fscanf(f_data, "%f", &x);	//第二列，储存在x中（过渡一下而已） 
	dk = (k2 - k1);
		//一定要让用户准备顺序排列k的data.txt文件！
		if (dk < 0)
		{
			printf("The values of ");
			color(14);
			printf("k");
			color(16);
			printf(" in the data should be sorted ");
			color(14);
			printf("in order");
			color(16);
			printf(" (not reversed order).\n\n");
			printf("Please prepare the ");
			color(14);
			printf("data.txt");
			color(16);
			printf(" once again.");
			getchar();
			exit(0);
		}
	}
	kmax = k2; //如果只有两个数据，k2就是kmax了 
	while (!feof(f_data))
	{
					fscanf(f_data, "%f", &kmax);	//继续读取k 
					fscanf(f_data, "%f", &x);	//过渡一下		
	}			
	fclose(f_data);	//关闭文件 
	
	//用户输入k的最小值
Q2:		color(14);
		printf("kmin");
		color(16);
		printf(" (if not sure, input 3)="); 
		scanf("%f", &kminInput);
		fflush(stdin);	//刷新键盘缓冲区
		printf("\n");
	//判断用户输入的k值在data.txt范围内
	if (kminInput < k1 || kminInput > kmax)
	{
		printf("*******************************************************\n");
		printf("kmin should be in the range of the original input data.\n");
		printf("*******************************************************\n\n");
		goto Q2;
	}
	
	//用户输入k的最大值
Q3:		color(14);
		printf("kmax");
		color(16);
		printf(" (if not sure, input 12)="); 
		scanf("%f", &kmaxInput);
		fflush(stdin);	//刷新键盘缓冲区
		printf("\n");
	//判断用户输入的k值在data.txt范围内
	if (kmaxInput > kmax || kmaxInput < k1)
	{
		printf("*******************************************************\n");
		printf("kmax should be in the range of the original input data.\n");
		printf("*******************************************************\n\n");
		goto Q3;
	}
	//保证用户输入的kmax必须大于kmin
	if (kminInput >= kmaxInput)
	{
		printf("********************************\n");
		printf("kmax should be larger than kmin.\n");
		printf("********************************\n\n");
		goto Q3;
	}
	
	//用户输入Rmin 
	color(14);
	printf("Rmin");
	color(16);
	printf(" (if not sure, input 1)=");
	scanf("%f", &rmin);
	fflush(stdin);	//刷新键盘缓冲区
	printf("\n");
	while (rmin <= 0)
	{
		printf("***************************\n");
		printf("Rmin must be larger than 0.\n");
		printf("***************************\n");
		printf("\n");
		color(14);
		printf("Rmin");
		color(16);
		printf(" (if not sure, input 1)=");
		scanf("%f", &rmin);
		fflush(stdin);	//刷新键盘缓冲区
		printf("\n");
	}
	//用户输入Rmax
Q1:	color(14);
	printf("Rmax");
	color(16);
	printf(" (if not sure, input 3)=");
	scanf("%f", &rmax);
	fflush(stdin);	//刷新键盘缓冲区
	printf("\n");
	while (rmax <= 0)
	{
		printf("***************************\n");
		printf("Rmax must be larger than 0.\n");
		printf("***************************\n");
		printf("\n");
		color(14);
		printf("Rmax");
		color(16);
		printf(" (if not sure, input 3)=");
		scanf("%f", &rmax);
		fflush(stdin);	//刷新键盘缓冲区
		printf("\n");
	}
	
	if (rmin >= rmax)
	{
		printf("******************************\n");
		printf("Rmax must be larger than Rmin.\n");
		printf("******************************\n\n");
		goto Q1;
	}
	
	//再次打开data数据文件，同时检测文件存在与否，正式读入数据 
	if(!(f_data = fopen(data, "r")))
	{
		printf("Cannot open file, please put the data.txt in the same file directory with the program.");
		getch();
		exit(0);
	}
	
	//data.txt的数据储存在一对数组中 
	while (!feof(f_data))	//文件未结束，则feof()返回0（即“假”)；文件结束，返回非0值（即“真”） 
	{
		fscanf(f_data, "%f", &k);	//第一列，储存在k中
		fscanf(f_data, "%f", &x);	//第二列，储存在x中 
		
		if (k>kmaxInput)
		{
			break;
		}
		
		if (k >= kminInput)
		{
			i++;	//i的初始值为-1 
			n++;	//n的初始值为0
			ki[i]=k;
			xi[i]=x;
		}
		
	}
	
//	printf("The data length used is ：%d\n\n",n);
	printf("calculating ...");
	
	amin = eta/(2.0*rmax); //根据a=eta/(2*r) 
	amax =  eta/(2.0*rmin);
	a = amax;
	da = (amax-amin)/(m-1);
	
	bmin = ki[0];
	bmax = ki[n-1];
	b = bmin;
	db = (bmax-bmin)/(m-1);
	
	//创建result数据文件
	if(!(f_result = fopen(result, "w")))
	{
		printf("file creation error!");
		getch();
		exit(0);
	}
	
	//result.txt文件写入初始信息 
	fprintf(f_result, "ETA	SIGMA	kmin	kmax	rmin	rmax\n");
	fprintf(f_result,"%-.2f	%-.2f	%-.2f	%-.2f	%-.2f	%-.2f\n-------------------------\n", eta,sigma,kminInput,kmaxInput,rmin,rmax);
	fprintf(f_result, "k	r	Mag	Real	Imag\n");
	
	while (a > amin)
	{
		while (b < bmax)
		{
			for (i=0;i<n;i++)
				{
					//接着小波积分运算 
					w = w + waveletTrans(a, xi[i], dk, morlet(a,b,eta,sigma,ki[i]));
				}
			r = eta/(2.0*a);
			//输出计算结果 
			fprintf(f_result, "%-f	%-f	%-f	%-f	%-f\n", b, r, cabs(w), crealf(w), cimagf(w));
					
			w = 0.0 + _Complex_I * 0.0;			
			b = b + db;
		}
		b = bmin;
		a = a - da;
	}
	
	fclose (f_result);
	fclose(f_data);
	
	color(10);
	printf("\n\nFinished!");
	color(16);
	printf("\n\nPlease refer to the generated ");
	color(14);
	printf("result.txt");
	color(16);
	printf(" file\n");
	printf("\n~Enjoy the EXAFS analysis~\n");

	getchar();
	fflush(stdin);	//刷新键盘缓冲区 
	return 0;
}

//小波变换单项公式 
complex float
waveletTrans(float _a, float _x, float _dk, complex float _morlet)
{
	complex float wi;
	wi =  1.0/sqrtf(_a) * _x * _morlet * _dk;
	return wi;	
}

//Morlet公式 
complex float
morlet(float _a, float _b, float _eta, float _sigma, float _k )
{
	complex float morleti;
	morleti = 1/(sqrtf(2.0*pai)*_sigma) * ( cexpf(_Complex_I*_eta*(_k - _b)/_a) - expf(-(_eta*_eta*_sigma*_sigma/2.0) )) 
	* expf(  -((_k-_b)/_a)*((_k-_b)/_a)/(2.0*_sigma*_sigma)  );

	return morleti;
}