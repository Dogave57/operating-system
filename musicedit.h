#ifndef _MUSICEDIT
#define _MUSICEDIT
struct beep{
	unsigned int freq;
	unsigned int dur;
	struct beep* flink;
	struct beep* blink;
};
#endif
