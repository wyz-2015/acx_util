#include "writer.h"

uint32_t find_c1o(const uint32_t c1)
{
	uint32_t c1o = c1;
	while (c1o % 4 != 0) {
		c1o += 1;
	}

	return c1o;
}

uint32_t find_n_block_size(const uint32_t size)
{
	uint32_t size2 = size;
	while (size2 % BLOCK_SIZE != 0) {
		size2 += 1;
	}

	return size2;
}

#define to_odd(n) (((n) % 2 == 0) ? ((n) + 1) : (n))

void ACXWriter_init(ACXWriter* restrict awriter, const Args* restrict args)
{
	awriter->args = args;

	awriter->acx = fopen(awriter->args->filePath, "wb");
	if (not awriter->acx) {
		error(ENOENT, ENOENT, "%s：打开文件%s的指针(%p)失败", __func__, awriter->args->filePath, awriter->acx);
	}

	// 初始化头
	awriter->header = (ACX_Header*)malloc(sizeof(ACX_Header));
	if (not awriter->header) {
		error(ENOMEM, ENOMEM, "%s：为awriter->header(%p)malloc失败", __func__, awriter->header);
	}

	memset(awriter->header, 0, sizeof(ACX_Header));	      // 先全部填0，后面类似
	awriter->itemCount_real = args->itemDeque->__len__;   // 文件数即传入的文件deque的项数
	awriter->itemCount = to_odd(awriter->itemCount_real); // 头，2个uint32_t。一条item也2个uint32_t，故想要让头+目录对齐16 Byte，让文件目录条目数占 奇数 个 2 uint32_t，即可。

	// 初始化文件偏移表。偏移表后面也要搞16 Byte对齐吗？
	awriter->itemTable = (ACX_Item**)malloc(awriter->itemCount * sizeof(ACX_Item*));
	if (not awriter->itemTable) {
		error(ENOMEM, ENOMEM, "%s：为awriter->fileOffsetTable(%p)malloc失败", __func__, awriter->itemTable);
	}
	for (uint32_t i = 0; i < awriter->itemCount; i += 1) {
		awriter->itemTable[i] = (ACX_Item*)malloc(sizeof(ACX_Item));
		if (not awriter->itemTable[i]) {
			error(ENOMEM, ENOMEM, "%s：为awriter->fileOffsetTable[%u](%p)malloc失败", __func__, i, awriter->itemTable[i]);
		}
		memset(awriter->itemTable[i], 0, sizeof(ACX_Item));
	}

	// 初始化文件数组，一切留空
	awriter->fileArray = (ACX_File**)malloc(awriter->itemCount_real * sizeof(ACX_File*));
	if (not awriter->fileArray) {
		error(ENOMEM, ENOMEM, "%s：为awriter->fileArray(%p)malloc失败", __func__, awriter->fileArray);
	}

	for (uint32_t i = 0; i < awriter->itemCount_real; i += 1) {
		awriter->fileArray[i] = (ACX_File*)malloc(sizeof(ACX_File));
		if (not awriter->fileArray[i]) {
			error(ENOMEM, ENOMEM, "%s：为awriter->fileArray[%u](%p)malloc失败", __func__, i, awriter->fileArray[i]);
		}

		memset(awriter->fileArray[i], 0, sizeof(ACX_File));
	}
}

void ACXWriter_read(ACXWriter* restrict awriter) // 读取待写入文件的内容
{
	Deque_Node* item = awriter->args->itemDeque->head;
	ACX_File** pf = awriter->fileArray;
	FILE* inFile;
	while (item) {
		if (awriter->args->verbose) {
			puts(item->value);
		}

		inFile = fopen(item->value, "rb");
		if (not inFile) {
			error(ENOENT, ENOENT, "%s：无法打开文件%s的指针%p", __func__, item->value, inFile);
		}

		(*pf)->fileLen = get_file_len(inFile);
		(*pf)->bufferSize = find_n_block_size((*pf)->fileLen);

		(*pf)->content = malloc((*pf)->bufferSize);
		if (not(*pf)->content) {
			error(ENOMEM, ENOMEM, "%s：为(*pf)->content(%p)malloc失败", __func__, (*pf)->content);
		}
		memset((*pf)->content, 0, (*pf)->bufferSize); // 补0

		fseek(inFile, 0, 0);
		fread((*pf)->content, (*pf)->fileLen, 1, inFile);

		fclose(inFile);
		inFile = NULL;

		item = item->next;
		pf += 1;
	}
	// 计算文件偏移表
	awriter->fileArray[0]->relativeOffset = sizeof(ACX_Header) + sizeof(ACX_Item) * awriter->itemCount; // 但是第0个文件的相对偏移要先算好，作为后续计算的起点

	for (uint32_t i = 1; i < awriter->itemCount_real; i += 1) {
		awriter->fileArray[i]->relativeOffset = awriter->fileArray[i - 1]->relativeOffset + awriter->fileArray[i - 1]->bufferSize;
	}
}

void ACXWriter_build_acxFile(ACXWriter* restrict awriter) // 构建ACX文件
{
	fseek(awriter->acx, 0, 0);

	awriter->header->fileCount = htonl(awriter->itemCount_real);
	fwrite(awriter->header, sizeof(ACX_Header), 1, awriter->acx);

	for (uint32_t i = 0; i < awriter->itemCount; i += 1) {
		if (i < awriter->itemCount_real) {
			awriter->itemTable[i]->fileLen = htonl(awriter->fileArray[i]->fileLen);
			awriter->itemTable[i]->relativeOffset = htonl(awriter->fileArray[i]->relativeOffset);
		}

		fwrite(awriter->itemTable[i], sizeof(ACX_Item), 1, awriter->acx);
	}

	for (uint32_t i = 0; i < awriter->itemCount_real; i += 1) {
		fwrite(awriter->fileArray[i]->content, awriter->fileArray[i]->bufferSize, 1, awriter->acx);
	}
}

// clear函数倒是完全一致的，所以直接作为一个别名
// #define ACXWriter_clear(awriter) ACXReader_clear((awriter))

void archive(const Args* restrict args)
{
	ACXWriter awriter;
	ACXWriter_init(&awriter, args);

	ACXWriter_read(&awriter);

	ACXWriter_build_acxFile(&awriter);

	ACXWriter_clear(&awriter);
}
