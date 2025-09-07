#include "kernel.h"
#include "video.h"
#include "stdlib.h"
#include "pci.h"
#include "dev.h"
struct dev_t* first_dev = (struct dev_t*)0x0;
struct dev_t* last_dev = (struct dev_t*)0x0;
struct dev_t* register_dev(struct dev_t dev){
	struct dev_t* pdev = (struct dev_t*)kmalloc(sizeof(struct dev_t));
	if (!pdev){	
		return pdev;
	}
	*pdev = dev;
	if (!last_dev){
		last_dev = pdev;
		first_dev = pdev;
		return pdev;
	}
	last_dev->flink = pdev;
	pdev->blink = last_dev;
	last_dev = pdev;
	return pdev;
}
int remove_dev(struct dev_t* pdev){
	if (!pdev)
		return -1;
	if (pdev->flink)
		pdev->flink->blink = pdev->blink;
	if (pdev->blink)
		pdev->blink->flink = pdev->flink;
	if (pdev==last_dev)
		last_dev = pdev->blink;
	if (pdev==first_dev)
		first_dev = pdev->flink;
	kfree((void*)pdev);
	return 0;
}
