#include "stdafx.h"

#define WIDTHBYTES(bytes)  (((bytes * 8) + 31) / 32 * 4)

unsigned char ClampToByte(int Value);
double Rand();
IP_RET GetValidCoordinate(int Width, int Height, int Left, int Right, int Top, int Bottom, EdgeMode Edge,
	TMatrix **Row, TMatrix **Col);
IP_RET GetExpandImage(TMatrix *Src, TMatrix **Dest, int Left, int Right, int Top, int Bottom, EdgeMode Edge);

