#include <stdio.h>
#include <complex.h>
#include <math.h>
#include <windows.h>

#define data "data.txt"	//���������ļ�����
#define result "result.txt"	//��������ļ�����
#define pai 3.141592653	//����pai 

/*-----------------------------------------------------------*/
void color(short x)	//�Զ��庯���ݲ����ı���ɫ https://blog.csdn.net/qq_31975227/article/details/51758461
{
    if(x>=0 && x<=15)//������0-15�ķ�Χ��ɫ
    	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), x);	//ֻ��һ���������ı�������ɫ 
    else//Ĭ�ϵ���ɫ��ɫ
    	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
}
/*
��ɫ����SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),ǰ��ɫ | ����ɫ | ǰ����ǿ | ������ǿ);
	ǰ��ɫ������0-15 �� FOREGROUND_XXX ��ʾ	������XXX����BLUE��RED��GREEN��ʾ�� 
	ǰ����ǿ������8 �� FOREGROUND_INTENSITY ��ʾ
	����ɫ������16 32 64 �� BACKGROUND_XXX ������ɫ��ʾ 
	������ǿ�� ����128 �� BACKGROUND_INTENSITY ��ʾ
��ҪӦ�ã��ı�ָ�����������뱳������ɫ
ǰ����ɫ��Ӧֵ�� 
����0=��ɫ                8=��ɫ����
  ��1=��ɫ                9=����ɫ        ʮ������        ����                        
����2=��ɫ                10=����ɫ       0xa        ����
����3=����ɫ              11=��ǳ��ɫ     0xb��
����4=��ɫ                12=����ɫ       0xc����
����5=��ɫ                13=����ɫ       0xd        ����
����6=��ɫ                14=����ɫ       0xe        ����
����7=��ɫ                15=����ɫ       0xf 
����Ҳ���԰���Щֵ���óɳ�����
https://blog.csdn.net/qq_31975227/article/details/51758461
*/
/*-----------------------------------------------------------*/
complex float waveletTrans(float, float, float, complex float);  //����С���任���ʽ 
complex float morlet(float, float, float, float, float);	//����morlet���� 

int
main()

{
	complex float w=0.0 + _Complex_I * 0.0; //С���任(Morlet)ϵ�������任����ֵ��
	float a,b; //С���任�������ӣ�a����λ��(b) 
	float da;	//a��b��ȡ���� 
	float db; 
	float amin;
	float amax;
	float bmin;
	float bmax;
	
	//����ϣ�������r�ķ�Χ 
	float rmax;
	float rmin;
	
 	float k; //EXAFS��k��������
 	float x; //EXAFS����ֵ��x(k)*k^n) 
 	float ki[1000]={0.0};	//����������ݵ�K 
 	float xi[1000]={0.0};	//����������ݵ�x(k)k^n 
	float r; //EXAFS����Ҷ�任���R�����룩
	float k1,k2,dk; //EXAFS��k֮������ݼ��(dk=k2-k1)
	float kmax; 
	float kminInput;
	float kmaxInput;
	
	int i=-1;
	int n=0; //�������ݵĸ��� 
	int m=100; //С���任��r��k��ȡֵ���� 
	
	//Morlet������ϵ����Ĭ��ֵ��; 
	float eta=5.0;
	float sigma=1.0; 
	 	
	FILE *f_data, *f_result; //���嵼�뵼���ļ�ָ�� 

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

	color(14);	//��������ɫ 
	printf("!!!NOTICE!!!");
	color(16);	//�ص�ԭ����ɫ 
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
	//�û�����eta
	color(14);
	printf("ETA");
	color(16);
	printf(" (if not sure, input 5)=");
	scanf("%f", &eta);
	fflush(stdin);	//ˢ�¼��̻����� 
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
		fflush(stdin);	//ˢ�¼��̻�����
		printf("\n");
	} 
	//�û�����sigma 
	color(14);
	printf("SIGMA");
	color(16);
	printf(" (if not sure, input 1)=");
	scanf("%f", &sigma);
	fflush(stdin);	//ˢ�¼��̻�����
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
		fflush(stdin);	//ˢ�¼��̻�����
		printf("\n");		
	}
		
	//��data�����ļ���ͬʱ����ļ����������ȷ�����ݼ��dk 
	if(!(f_data = fopen(data, "r")))
	{
		printf("Cannot open file, please put the data.txt in the same file directory with the program.");
		getch();
		exit(0);
	}
			
	//����data�е�k�����ݼ������Сֵ�����ֵ 
	if (!feof(f_data))	//�ļ�δ��������feof()����0�������١�)���ļ����������ط�0ֵ�������桱�� 
	{
	fscanf(f_data, "%f", &k1);	//��һ�е�һ�У�������k��
	fscanf(f_data, "%f", &x);	//�ڶ��У�������x�У�����һ�¶��ѣ� 
	fscanf(f_data, "%f", &k2);	//��һ�еڶ��У�������x��
	fscanf(f_data, "%f", &x);	//�ڶ��У�������x�У�����һ�¶��ѣ� 
	dk = (k2 - k1);
		//һ��Ҫ���û�׼��˳������k��data.txt�ļ���
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
	kmax = k2; //���ֻ���������ݣ�k2����kmax�� 
	while (!feof(f_data))
	{
					fscanf(f_data, "%f", &kmax);	//������ȡk 
					fscanf(f_data, "%f", &x);	//����һ��		
	}			
	fclose(f_data);	//�ر��ļ� 
	
	//�û�����k����Сֵ
Q2:		color(14);
		printf("kmin");
		color(16);
		printf(" (if not sure, input 3)="); 
		scanf("%f", &kminInput);
		fflush(stdin);	//ˢ�¼��̻�����
		printf("\n");
	//�ж��û������kֵ��data.txt��Χ��
	if (kminInput < k1 || kminInput > kmax)
	{
		printf("*******************************************************\n");
		printf("kmin should be in the range of the original input data.\n");
		printf("*******************************************************\n\n");
		goto Q2;
	}
	
	//�û�����k�����ֵ
Q3:		color(14);
		printf("kmax");
		color(16);
		printf(" (if not sure, input 12)="); 
		scanf("%f", &kmaxInput);
		fflush(stdin);	//ˢ�¼��̻�����
		printf("\n");
	//�ж��û������kֵ��data.txt��Χ��
	if (kmaxInput > kmax || kmaxInput < k1)
	{
		printf("*******************************************************\n");
		printf("kmax should be in the range of the original input data.\n");
		printf("*******************************************************\n\n");
		goto Q3;
	}
	//��֤�û������kmax�������kmin
	if (kminInput >= kmaxInput)
	{
		printf("********************************\n");
		printf("kmax should be larger than kmin.\n");
		printf("********************************\n\n");
		goto Q3;
	}
	
	//�û�����Rmin 
	color(14);
	printf("Rmin");
	color(16);
	printf(" (if not sure, input 1)=");
	scanf("%f", &rmin);
	fflush(stdin);	//ˢ�¼��̻�����
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
		fflush(stdin);	//ˢ�¼��̻�����
		printf("\n");
	}
	//�û�����Rmax
Q1:	color(14);
	printf("Rmax");
	color(16);
	printf(" (if not sure, input 3)=");
	scanf("%f", &rmax);
	fflush(stdin);	//ˢ�¼��̻�����
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
		fflush(stdin);	//ˢ�¼��̻�����
		printf("\n");
	}
	
	if (rmin >= rmax)
	{
		printf("******************************\n");
		printf("Rmax must be larger than Rmin.\n");
		printf("******************************\n\n");
		goto Q1;
	}
	
	//�ٴδ�data�����ļ���ͬʱ����ļ����������ʽ�������� 
	if(!(f_data = fopen(data, "r")))
	{
		printf("Cannot open file, please put the data.txt in the same file directory with the program.");
		getch();
		exit(0);
	}
	
	//data.txt�����ݴ�����һ�������� 
	while (!feof(f_data))	//�ļ�δ��������feof()����0�������١�)���ļ����������ط�0ֵ�������桱�� 
	{
		fscanf(f_data, "%f", &k);	//��һ�У�������k��
		fscanf(f_data, "%f", &x);	//�ڶ��У�������x�� 
		
		if (k>kmaxInput)
		{
			break;
		}
		
		if (k >= kminInput)
		{
			i++;	//i�ĳ�ʼֵΪ-1 
			n++;	//n�ĳ�ʼֵΪ0
			ki[i]=k;
			xi[i]=x;
		}
		
	}
	
//	printf("The data length used is ��%d\n\n",n);
	printf("calculating ...");
	
	amin = eta/(2.0*rmax); //����a=eta/(2*r) 
	amax =  eta/(2.0*rmin);
	a = amax;
	da = (amax-amin)/(m-1);
	
	bmin = ki[0];
	bmax = ki[n-1];
	b = bmin;
	db = (bmax-bmin)/(m-1);
	
	//����result�����ļ�
	if(!(f_result = fopen(result, "w")))
	{
		printf("file creation error!");
		getch();
		exit(0);
	}
	
	//result.txt�ļ�д���ʼ��Ϣ 
	fprintf(f_result, "ETA	SIGMA	kmin	kmax	rmin	rmax\n");
	fprintf(f_result,"%-.2f	%-.2f	%-.2f	%-.2f	%-.2f	%-.2f\n-------------------------\n", eta,sigma,kminInput,kmaxInput,rmin,rmax);
	fprintf(f_result, "k	r	Mag	Real	Imag\n");
	
	while (a > amin)
	{
		while (b < bmax)
		{
			for (i=0;i<n;i++)
				{
					//����С���������� 
					w = w + waveletTrans(a, xi[i], dk, morlet(a,b,eta,sigma,ki[i]));
				}
			r = eta/(2.0*a);
			//��������� 
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
	fflush(stdin);	//ˢ�¼��̻����� 
	return 0;
}

//С���任���ʽ 
complex float
waveletTrans(float _a, float _x, float _dk, complex float _morlet)
{
	complex float wi;
	wi =  1.0/sqrtf(_a) * _x * _morlet * _dk;
	return wi;	
}

//Morlet��ʽ 
complex float
morlet(float _a, float _b, float _eta, float _sigma, float _k )
{
	complex float morleti;
	morleti = 1/(sqrtf(2.0*pai)*_sigma) * ( cexpf(_Complex_I*_eta*(_k - _b)/_a) - expf(-(_eta*_eta*_sigma*_sigma/2.0) )) 
	* expf(  -((_k-_b)/_a)*((_k-_b)/_a)/(2.0*_sigma*_sigma)  );

	return morleti;
}