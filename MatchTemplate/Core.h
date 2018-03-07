#include "stdafx.h"

const float Inv255 = 1.0 / 255;
const double Eps = 2.220446049250313E-16;
const int MaxValueI = +2147483647;
const int MinValueI = -2147483648;

enum EdgeMode				// Boundary processing methods for some domain algorithms
{
	Tile = 0,				// Repeating edge pixels
	Smear = 1				// Mirrored edge pixels
};

enum IP_RET
{
	IP_RET_OK,									//	Normal
	IP_RET_ERR_OUTOFMEMORY,						//	Out of memory
	IP_RET_ERR_STACKOVERFLOW,					//	Stack overflow
	IP_RET_ERR_NULLREFERENCE,					//	Empty reference
	IP_RET_ERR_ARGUMENTOUTOFRANGE,				//	The parameters are not within the normal range
	IP_RET_ERR_PARAMISMATCH,					//	Parameter mismatch
	IP_RET_ERR_DIVIDEBYZERO,
	IP_RET_ERR_INDEXOUTOFRANGE,
	IP_RET_ERR_NOTSUPPORTED,
	IP_RET_ERR_OVERFLOW,
	IP_RET_ERR_FILENOTFOUND,
	IP_RET_ERR_UNKNOWN
};

enum IP_DEPTH
{
	IP_DEPTH_8U = 0,			//	unsigned char
	IP_DEPTH_8S = 1,			//	char
	IP_DEPTH_16S = 2,			//	short
	IP_DEPTH_32S = 3,			//  int
	IP_DEPTH_32F = 4,			//	float
	IP_DEPTH_64F = 5,			//	double
};

struct TMatrix
{
	int Width;					// The width of a matrix
	int Height;					// The height of a matrix
	int WidthStep;				// The number of bytes occupied by a line element of a matrix
	int Channel;				// Number of matrix channels
	int Depth;					// The type of matrix element
	unsigned char *Data;		// Data of a matrix
	int Reserved;				// Reserved use
};

int IP_ELEMENT_SIZE(int Depth);																// Get the size of the element
void *IP_AllocMemory(unsigned int size, bool ZeroMemory = false);							// Allocating memory, 32 byte alignment
void IP_FreeMemory(void *Ptr);																// Release memory
IP_RET IP_CreateMatrix(int Width, int Height, int Depth, int Channel, TMatrix **Matrix);	// Creating a data matrix
IP_RET IP_FreeMatrix(TMatrix **Matrix);														// Release data matrix
IP_RET IP_CloneMatrix(TMatrix *Src, TMatrix **Dest);										// Cloned data matrix