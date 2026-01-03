#include "reader.h"

bool ACXReader_file_check2(ACXReader* restrict areader) // 检查文件
{
	const ACX_Item* lastItem = areader->itemTable[areader->itemCount - 1];
	uint32_t fileLen = ntohl(lastItem->relativeOffset) + ntohl(lastItem->fileLen), fileLen_real = get_file_len(areader->acx);

	if (abs(fileLen - fileLen_real) < sizeof(uint32_t)) {
		return true;
	} else {
		return false;
	}
}

void ACXReader_init(ACXReader* restrict areader, const Args* args)
{
	areader->args = args;

	areader->acx = fopen(areader->args->filePath, "rb");
	if (not areader->acx) {
		error(ENOENT, ENOENT, "%s：无法打开areader->acx(%s)文件指针(%p)", __func__, args->filePath, areader->acx);
	}

	// 读头
	areader->header = (ACX_Header*)malloc(sizeof(ACX_Header));
	if (not areader->header) {
		error(ENOMEM, ENOMEM, "%s：为areader->header(%p)malloc失败", __func__, areader->header);
	}

	fread(areader->header, sizeof(ACX_Header), 1, areader->acx);

	// 检查文件1：检查头
	if (not ACXReader_file_check1(areader)) {
		error(EPERM, EPERM, "%s：传入的文件%s(%p)疑似不是ACX文件，至少不符合文件头规定", __func__, areader->args->filePath, areader->acx);
	}

	areader->itemCount = ntohl(areader->header->fileCount); // 大端换小端
	areader->itemCount_real = areader->itemCount;		// 本系统的设计，itemCount >= itemCount_real。itemCount是头中所述数据，申请itemTable空间时使用这个数据；_real是实际文件条目数，申请fileArray空间时使用这个数据。

	// 读文件目录
	areader->itemTable = (ACX_Item**)malloc(sizeof(ACX_Item*) * areader->itemCount);
	if (not areader->itemTable) {
		error(ENOMEM, ENOMEM, "%s：为areader->tiemTable(%p)malloc失败", __func__, areader->itemTable);
	}
	for (uint32_t i = 0; i < areader->itemCount; i += 1) {
		areader->itemTable[i] = (ACX_Item*)malloc(sizeof(ACX_Item));
		if (not areader->itemTable[i]) {
			error(ENOMEM, ENOMEM, "%s：为areader->tiemTable[%u](%p)malloc失败", __func__, i, areader->itemTable[i]);
		}
		fread(areader->itemTable[i], sizeof(ACX_Item), 1, areader->acx);
	}

	// 检查文件2：检查文件目录
	if (not ACXReader_file_check2(areader)) {
		error(EPERM, EPERM, "%s：传入的文件%s(%p)疑似不是ACX文件，至少不符合文件目录规定", __func__, areader->args->filePath, areader->acx);
	}

	// 初始化文件数组
	areader->fileArray = (ACX_File**)malloc(sizeof(ACX_File*) * areader->itemCount_real);
	if (not areader->fileArray) {
		error(ENOMEM, ENOMEM, "%s：为areader->fileArray(%p)malloc失败", __func__, areader->fileArray);
	}
	for (uint32_t i = 0; i < areader->itemCount_real; i += 1) {
		areader->fileArray[i] = (ACX_File*)malloc(sizeof(ACX_File));
		if (not areader->fileArray[i]) {
			error(ENOMEM, ENOMEM, "%s：为areader->fileArray[%u](%p)malloc失败", __func__, i, areader->fileArray[i]);
		}

		areader->fileArray[i]->content = NULL;
		areader->fileArray[i]->relativeOffset = 0;
		areader->fileArray[i]->fileLen = 0;
		areader->fileArray[i]->bufferSize = 0;
	}

	ACXReader_read_metadata(areader);
}

void ACXReader_read_metadata(ACXReader* restrict areader) // 读取ACX_File中的元数据
{
	uint32_t i, fileLen, fileLen_real;

	for (i = 0; i < areader->itemCount_real - 1; i += 1) {
		areader->fileArray[i]->relativeOffset = ntohl(areader->itemTable[i]->relativeOffset);
		areader->fileArray[i]->fileLen = ntohl(areader->itemTable[i]->fileLen);

		areader->fileArray[i]->bufferSize = areader->fileArray[i]->fileLen;
	}

	i = areader->itemCount - 1,

	areader->fileArray[i]->relativeOffset = ntohl(areader->itemTable[i]->relativeOffset);

	fileLen = ntohl(areader->itemTable[i]->fileLen),
	fileLen_real = get_file_len(areader->acx) - areader->fileArray[i]->relativeOffset;

	areader->fileArray[i]->fileLen = min(fileLen, fileLen_real);
	areader->fileArray[i]->bufferSize = fileLen;
	/*
	if (fileLen > fileLen_real) { // 文件实际结束得比偏移表中早
		areader->fileArray[i]->bufferSize = fileLen;
	} else { // 文件实际结束得比偏移表晚
		areader->fileArray[i]->bufferSize = fileLen;
	}
	*/
}

void ACXReader_read_content(ACXReader* restrict areader, const uint32_t fileIndex) // 读取ACX_File中的内容数据
{
	uint32_t i = fileIndex;
	areader->fileArray[i]->content = malloc(areader->fileArray[i]->bufferSize);
	if (not areader->fileArray[i]->content) {
		error(ENOMEM, ENOMEM, "%s：为areader->fileArray[%u]->content(%p)malloc失败", __func__, i, areader->fileArray[i]->content);
	}
	memset(areader->fileArray[i]->content, 0, areader->fileArray[i]->bufferSize);

	fseek(areader->acx, areader->fileArray[i]->relativeOffset, 0);
	fread(areader->fileArray[i]->content, areader->fileArray[i]->bufferSize, 1, areader->acx);
}

void ACXReader_copy_content(ACXReader* restrict areader, const uint32_t fileIndex)
{
	char outFilePath[FILEPATH_LEN_MAX];
	memset(outFilePath, '\0', FILEPATH_LEN_MAX * sizeof(char));

	sprintf(outFilePath, "%s/%03u.adx", areader->args->dir, fileIndex);

	if (areader->args->verbose) {
		printf("0x%08x\n", areader->fileArray[fileIndex]->relativeOffset);
	}

	FILE* outFile = fopen(outFilePath, "wb");
	if (not outFile) {
		error(ENOENT, ENOENT, "%s：打开文件%s的指针%p失败", __func__, outFilePath, outFile);
	}

	fwrite(areader->fileArray[fileIndex]->content, areader->fileArray[fileIndex]->bufferSize, 1, outFile);

	fclose(outFile);
	outFile = NULL;
}

void ACXReader_clear(ACXReader* restrict areader)
{
	if (areader->header) {
		free(areader->header);
		areader->header = NULL;
	}

	if (areader->itemTable) {
		for (uint32_t i = 0; i < areader->itemCount; i += 1) {
			if (areader->itemTable[i]) {
				free(areader->itemTable[i]);
				areader->itemTable[i] = NULL;
			}
		}
		free(areader->itemTable);
		areader->itemTable = NULL;
	}

	if (areader->fileArray) {
		for (uint32_t i = 0; i < areader->itemCount_real; i += 1) {
			if (areader->fileArray[i]) {
				if (areader->fileArray[i]->content) {
					free(areader->fileArray[i]->content);
					areader->fileArray[i]->content = NULL;
				}
				free(areader->fileArray[i]);
				areader->fileArray[i] = NULL;
			}
		}
		areader->fileArray = NULL;
	}
}

/* ********************************** */

void extract(const Args* restrict args)
{
	ACXReader areader;
	ACXReader_init(&areader, args);

	if (args->extractAll) {
		for (uint32_t i = 0; i < areader.itemCount; i += 1) {
			ACXReader_read_content(&areader, i);
			ACXReader_copy_content(&areader, i);
		}
	} else {
		Deque_Node* item = args->itemDeque->head;
		uint32_t itemNo;
		while (item) {
			if (sscanf(item->value, "%u", &itemNo) != 1) {
				error(EINVAL, EINVAL, "%s：“%s”无法读取为一个uint32_t数，无法作为文件序号", __func__, item->value);
			}
			ACXReader_read_content(&areader, itemNo);
			ACXReader_copy_content(&areader, itemNo);

			item = item->next;
		}
	}

	ACXReader_clear(&areader);
}

void list(const Args* restrict args)
{
	ACXReader areader;
	ACXReader_init(&areader, args);

	ACXReader_read_metadata(&areader);

	printf("总文件数：%u\n", areader.itemCount);

	char s1[15], s2[15];
	printf("%-5s%-15s%-20s%-15s\n", "No.", "Offset(hex)", "Length(hex, Byte)", "Length(KiB)");
	puts("========================================================================");
	for (uint32_t i = 0; i < areader.itemCount; i += 1) {
		sprintf(s1, "0x%08x", areader.fileArray[i]->relativeOffset);
		sprintf(s2, "0x%08x", areader.fileArray[i]->fileLen);
		printf("%-5u%-15s%-20s%-15lf\n",
		    i,
		    s1,
		    s2,
		    B2KB(areader.fileArray[i]->fileLen));
	}

	ACXReader_clear(&areader);
}
