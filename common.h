/**
@file common.h
@brief Commonly used functions and debugging

Provides commonly-used includes; functions for manipulating strings, memory and logic, and functions for 
printing useful debugging textwith minimal effort.
@author crashenator (crashenator -at- gmail.com)
*/




#pragma once
#define _CRT_SECURE_NO_DEPRECATE
#define _CRT_SECURE_NO_WARNINGS
//#define WIN32_LEAN_AND_MEAN
#define NOCRYPT
#define NOSERVICE
#define NOMCX
#define NOIME
#include <cstdio>
#include <cstdarg>
#include <string>
#include <vector>
#include <iostream>
#include <sstream>
#include <math.h>
//#include <windows.h>
#include "system.h"
using namespace std;

#define Q(x) #x
#define QI(x) Q(x)

#define PI (3.14159265f)
#define PI2 (6.2831853f)
#define PID2 (1.570796325f)
#define DEGRAD (0.0174532925f)
#define RADDEG 180/PI

#define RANGE(i,n,x) (i>=n && i<=x)
#define RANGE2(i,n,x) ((i>=n && i<=x) || (i>=x && i<=n))
#define OUTRANGE(i,n,x) (i<n || i>x)

#define NIMP(a,b) (a&(~b))//nonimplication - A but not B
#define UNSET(a,b) (a&(~b))
#define FLAG_SET(a,b) a|=b//a bits OR b bits.
#define FLAG_UNSET(a,b) a&=~b//a AND (all bits but B)
#define FLAG_TOGGLE(a,b) a^=b//a XOR b,
#define FLAG_CHECK(a,b) ((a&b)==b)


bool fRange(float i, float in, float ix);
/*
template <class T>
vector<T> INLINE_VECTOR(T element, ...) {
	
	MessageBox({1,2});
  int i;
  double val;
  printf ("Printing floats:");
  va_list vl;
  va_start(vl,n);
  for (i=0;i<n;i++)
  {
    val=va_arg(vl,double);
    printf (" [%.2f]",val);
  }
  va_end(vl);
  printf ("\n");


  T result;
  result = (a>b)? a : b;
  return (result);
}
*/



//#define KEY_DOWN(vk_code) ((GetAsyncKeyState(vk_code)&0x8000)?1:0)
//#define KEY_UP(vk_code) ((GetAsyncKeyState(vk_code)&0x8000)?1:0)

int MessageBox(const string& text,const string& caption);

unsigned long int stou(const string& s);
string itos(const int& i);///< int to string
string itoss(const int& i);///< int to string with mandatory signs
string utos(const unsigned long int& i);///< unsigned int to string
string ptos(void* p);///< pointer to string
string dtos(double d);
void printf(const string& s);


//float fabs(float n){if(n<0) return -n; return n;}



float frand();///< floating point random value
int random(int min, int max);///< random integer value inclusive to a range
float frandom(float min,float max);///floating point random inclusive to range
float sign(float n);///< sign of a float (-1.0f or +1.0f. zero yields positive)
float limit(float i, float minimum, float maximum);///< restricts a float to a given range. overflows default to the value passed.
float realAngle(float rad);///< attempts to resolve parallel angles for ones that expand past the normal range of 0 to 2PI.
void loadFromFile(const char* file,void* pMemory,int bytes);///< loads binary data from a file into the given address.
void saveToFile(const char* file,void* pMemory,int bytes);///< saves binary data to a file from the given address.
class Range/// class implementing minimum-maximum rang comparison logic and functions
{
    public:
        bool ai,ao;
        float n, x;
        void set(float min,float max,bool allInside=false,bool allOutside=false){
            n=min;x=max;
            ai=allInside;ao=allOutside;
        }
        Range(){set(0,0,false,false);}
        bool isInside(float i){
            if(ai) return true;
            if(ao) return false;
            return (i>=n && i<=x);
        }
        bool isOutside(float i){
            if(ao) return true;
            if(ai) return false;
            return (i<n && i>x); 
        }
};

