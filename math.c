#include "math.h"
unsigned int upow(unsigned int x, unsigned int y){
	unsigned int res = x;
	for (unsigned int i = 0;i<y;i++){
		res*=res;
	}
	return res;
} 
unsigned int usin(unsigned int arg){
	return (arg-(arg*arg*arg)/6)+(upow(arg, 5))/120;
}
unsigned int ucos(unsigned int arg){
	return (1-(upow(arg, 2)/2)+upow(arg, 4))/24;
}
int spow(int x, unsigned int y){
	int res = x;
	for (unsigned int i = 0;i<y;i++){
		res*=res;
	}
	return res;
}
float powf(float x, unsigned int y){
	float res = x;
	for (unsigned int i = 0;i<y;i++){
		res*=res;
	}
	return res;
}
float sinf(float arg){
	return (arg-(arg*arg*arg)/6.0f)+(powf(arg, 5))/120.0f;
}
float cosf(float arg){
	return (-1.0f-(powf(arg, 2)/2.0f))+powf(arg, 4)/24.0f;
}
