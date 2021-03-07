#include "common.h"



#define round(d) floor(d+0.5f)

bool fRange(float i, float in, float ix){
	float n=min(in,ix);
	float x=max(in,ix);

	return  i>=(n-0.0001) && i<=(x+0.0001);

}

int MessageBox(const string& text,const string& caption){ System_Alert(caption,text); return 1;}


void printf(const string& s)
{
	printf("%s",s.c_str());
}


string itos(const int& i)
{
	char buf[20];
	_itoa(i,buf,10);
	return string(buf);
}
string itoss(const int& i)
{
	char buf[20];
	_itoa(i,buf,10);
	string s(buf);
	if(i>0) s="+"+s;
	return s;
}
string utos(const unsigned long int& i)
{
	char buf[20]={0};
	sprintf(buf,"%lX",i);
	//itoa(i,buf,10);
	return string(buf);
}

string dtos(double d){
	stringstream ss;
	ss<<d;
	return ss.str();
}
string ptos(void* p){ return utos((unsigned long int) p); }
unsigned long int stou(const string& s)
{
	return atoi(s.c_str());
}



//float fabs(float n){if(n<0) return -n; return n;}



float frand(){return (float)rand()/(float)RAND_MAX;}//0.0 - 1.0
int random(int min, int max){
    return rand()%((max-min)+1) + min;
}
float frandom(float min,float max){
    return frand()*(max-min) + min;
}
float sign(float n){//alternatively  abs(n)/n  but undefined for n=0
    if(n==0) return +1;//we don't really need a third condition, but It could be added
    if(n>0) return +1;
    return -1;//if(n<0)   - took off the condition because of compiler Warning
}
float limit(float i, float minimum, float maximum)
{
    if(i<minimum) return minimum;
    if(i>maximum) return maximum;
    return i;
}
float realAngle(float rad)//CONSIDER: modulus?
{
	while(rad>(+2*PI)) rad-=2*PI;
	while(rad<0) rad+=2*PI;
	/*
	float orad=rad;
	int i;
	for(i=0; (rad<0 || rad>=PI2) && i<100; i++){
		if(rad<0) rad=PI2+rad;
		if(rad>=PI2) rad=rad-PI2;
	}

	char buf[100]={0};
	sprintf(buf,"%0.6f",rad);
	if(i>=1000){
			System_Alert(buf,"RealAngle Failed");
	}
    //if(rad<0) rad=PI2+rad;
    //if(rad>=PI2) rad=rad-PI2;
    //if(rad<0 || rad>=PI2) rad=realAngle(rad);//recursion perhaps necessary.
	*/
    return rad;
}
void loadFromFile(const char* file,void* pMemory,int bytes)
{
    FILE * pFile = fopen (file , "rb");
    if (pFile == NULL) perror ("Error opening file");
    else {
        fread(pMemory,bytes,1,pFile);
        fclose (pFile);
    }
    delete[] file;
}
void saveToFile(const char* file,void* pMemory,int bytes)
{
    FILE * pFile = fopen (file , "wb");
    if (pFile == NULL) return;
    else {
        fwrite(pMemory,bytes,1,pFile);
        fclose (pFile);
    }
    delete[] file;
}
