#ifndef _DEV
#define _DEV
struct dev_t;
enum devType{
	DEV_INVALID,
	DEV_PCI,
	DEV_USB2,
	DEV_USB3,
};
struct dev_t{
	enum devType type;
	void* pdata;
	struct dev_t* flink;
	struct dev_t* blink;
};
extern struct dev_t* first_dev;
extern struct dev_t* last_dev;
#endif
