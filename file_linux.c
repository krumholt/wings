#include <malloc.h>
#include <assert.h>
#include <stdio.h>

#include "file.h"


char *
ReadFileAsText(char *Filename)
{
	FILE *InputFile;
	InputFile = fopen(Filename, "rb");
	assert(InputFile != 0);
	fseek(InputFile, 0, SEEK_END);
	uint64 Size = (uint64)ftell(InputFile);
	fseek(InputFile, 0, SEEK_SET);

	char *Data = (char *)calloc(Size + 1, sizeof(char));
	fread(Data, 1, Size, InputFile);
	fclose(InputFile);
	return(Data);
}

inline void
ReadFile(wchar_t *Location, uint8 **Data, uint64 *Size)
{
	assert("Not implemented" == 0);
}

inline void
ReadFile(wchar_t *Location, uint8 **Data, uint32 *Size)
{
	ReadFile(Location, Data, (uint64 *)Size);
}

inline void
ReadFile(char *Location, uint8 **Data, uint32 *Size)
{
	FILE *InputFile = 0;
	InputFile = fopen(Location, "rb");
	assert(InputFile != 0);
	fseek(InputFile, 0, SEEK_END);
	*Size = (uint64)ftell(InputFile);
	fseek(InputFile, 0, SEEK_SET);

	*Data = (uint8 *)calloc(*Size, sizeof(uint8));
	fread(*Data, 1, *Size, InputFile);
	fclose(InputFile);
}

inline void
ReadFile(char *Location, uint8 **Data, uint64 *Size)
{
	ReadFile(Location, Data, (uint32 *)Size);
}

inline void
WriteFile(char *Location, uint8 *Data, uint64 Size)
{
	assert(0);
	//"not implemented";
}
