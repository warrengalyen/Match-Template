#include "stdafx.h"
#include "Utility.h"

void *IP_AllocMemory(unsigned int Size, bool ZeroMemory)
{
	void *Ptr = _mm_malloc(Size, 32);
	if (Ptr != NULL)
		if (ZeroMemory == true)
			memset(Ptr, 0, Size);
	return Ptr;
}

void IP_FreeMemory(void *Ptr)
{
	if (Ptr != NULL) _mm_free(Ptr);
}

/// <summary>
/// Obtain the number of bytes actually occupied by an element based on the type of the matrix element.
/// </summary>
/// <param name="Depth">According to the type of the matrix element.</param>
/// <returns>Only used internally.</returns>
int IP_ELEMENT_SIZE(int Depth)
{
	int Size;
	switch (Depth)
	{
	case IP_DEPTH_8U:
		Size = sizeof(unsigned char);
		break;
	case IP_DEPTH_8S:
		Size = sizeof(char); 
		break;
	case IP_DEPTH_16S:
		Size = sizeof(short);
		break;
	case IP_DEPTH_32S:
		Size = sizeof(int);
		break;
	case IP_DEPTH_32F:
		Size = sizeof(float);
		break;
	case IP_DEPTH_64F:
		Size = sizeof(double);
		break;
	default:
		Size = 0;
		break;
	}
	return Size;
}

/// <summary>
/// Create new matrix data.
/// </summary>
/// <param name="Width">The width of the matrix.</param>
/// <param name="Height">The height of a matrix.</param>
/// <param name="Depth">The color depth of a matrix.</param>
/// <param name="Channel">The number of channels of a matrix.</param>
/// <param name="Matrix">The returned Matrix object.</param>
/// <returns>Returns 0 if success, otherwise fail</returns>
IP_RET IP_CreateMatrix(int Width, int Height, int Depth, int Channel, TMatrix **Matrix)
{
	if (Width < 1 || Height < 1) return IP_RET_ERR_ARGUMENTOUTOFRANGE;
	if (Depth != IP_DEPTH_8U && Depth != IP_DEPTH_8S && Depth != IP_DEPTH_16S && Depth != IP_DEPTH_32S && Depth != IP_DEPTH_32F && Depth != IP_DEPTH_64F) return IP_RET_ERR_ARGUMENTOUTOFRANGE;
	if (Channel != 1 && Channel != 2 && Channel != 3 && Channel != 4) return IP_RET_ERR_ARGUMENTOUTOFRANGE;
	*Matrix = (TMatrix *)IP_AllocMemory(sizeof(TMatrix));
	(*Matrix)->Width = Width;
	(*Matrix)->Height = Height;
	(*Matrix)->Depth = Depth;
	(*Matrix)->Channel = Channel;
	(*Matrix)->WidthStep = WIDTHBYTES(Width * Channel * IP_ELEMENT_SIZE(Depth));
	(*Matrix)->Data = (unsigned char *)IP_AllocMemory((*Matrix)->Height * (*Matrix)->WidthStep, true);
	if ((*Matrix)->Data == NULL)
	{
		IP_FreeMemory(*Matrix);
		return IP_RET_ERR_OUTOFMEMORY;
	}
	(*Matrix)->Reserved = 0;
	return IP_RET_OK;
}

/// <summary>
/// Release a created matrix.
/// </summary>
/// <param name="Matrix">A matrix object that needs to be released.</param>
/// <returns>Returns 0 if success, otherwise fail</returns>
IP_RET IP_FreeMatrix(TMatrix **Matrix)
{
	if ((*Matrix) == NULL) return IP_RET_ERR_NULLREFERENCE;
	if ((*Matrix)->Data == NULL)
	{
		IP_FreeMemory((*Matrix));
		return IP_RET_ERR_OUTOFMEMORY;
	}
	else
	{
		IP_FreeMemory((*Matrix)->Data);		// Release in proper order
		IP_FreeMemory((*Matrix));
		return IP_RET_OK;
	}
}

/// <summary>
/// Clone existing matrix.
/// </summary>
/// <param name="Src">Source matrix object</param>
/// <param name="Dest">Destination matrix object</param>
/// <returns>Returns 0 if success, otherwise fail</returns>
IP_RET IP_CloneMatrix(TMatrix *Src, TMatrix **Dest)
{
	if (Src == NULL) return IP_RET_ERR_NULLREFERENCE;
	if (Src->Data == NULL) return IP_RET_ERR_NULLREFERENCE;
	IP_RET Ret = IP_CreateMatrix(Src->Width, Src->Height, Src->Depth, Src->Channel, Dest);
	if (Ret == IP_RET_OK) memcpy((*Dest)->Data, Src->Data, (*Dest)->Height * (*Dest)->WidthStep);
	return Ret;
}