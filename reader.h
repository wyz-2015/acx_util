#ifndef _READER_H
#define _READER_H

#include "acx.h"
#include "args.h"
#include "common.h"

typedef struct ACXReader {
	const Args* args;

	ACX_Header* header;

	ACX_Item** itemTable;
	uint32_t itemCount;
	uint32_t itemCount_real;

	ACX_File** fileArray;

	FILE* acx;
} ACXReader;

extern void ACXReader_init(ACXReader* __restrict areader, const Args* args);		     // 初始化类
extern void ACXReader_read_metadata(ACXReader* __restrict areader);			     // 读取ACX_File中的元数据
extern void ACXReader_read_content(ACXReader* __restrict areader, const uint32_t fileIndex); // 读取ACX_File中的内容数据
extern void ACXReader_copy_content(ACXReader* __restrict areader, const uint32_t fileIndex); // 将PAK中指定序号的文件拷出
extern void ACXReader_clear(ACXReader* __restrict areader);				     // 释放类的内存，类本身仍需要额外释放
extern bool ACXReader_file_check2(ACXReader* __restrict areader);			     // 检查文件
// 检查头。zero部分确实都是0吧？
#define ACXReader_file_check1(areader) ((areader)->header->zero == 0)

extern void extract(const Args* __restrict args); // 前端函数，解压
extern void list(const Args* __restrict args);	  // 前端函数，罗列文件表

#endif
