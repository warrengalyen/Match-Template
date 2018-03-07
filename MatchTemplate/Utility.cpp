#include "Utility.h"

unsigned char ClampToByte(int Value)
{
	return ((Value | ((signed int)(255 - Value) >> 31)) & ~((signed int)Value >> 31));
}

double Rand()
{
	return (double)rand() / (RAND_MAX + 1.0);
}

/// <summary>
/// Calculates the rational value of each coordinate after expansion according to the specified edge mode
/// </summary>
/// <param name="Width">The width of the matrix.</param>
/// <param name="Height">The height of the matrix.</param>
/// <param name="Left">The left edge of the coordinates.</param>
/// <param name="Right">The right edge of the coordinates.</param>
/// <param name="Top">The top edge of the coordinates.</param>
/// <param name="Bottom">The bottom edge of the coordinates.</param>
/// <param name="Edge">The method to deal with the edge.</param>
/// <param name="Row">The coordinates of the row.</param>
/// <param name="Col">The coordinates of the column.</param>
/// <returns>Returns whether the function is successful.</returns>
IP_RET GetValidCoordinate(int Width, int Height, int Left, int Right, int Top, int Bottom, EdgeMode Edge, TMatrix **Row, TMatrix **Col)
{
	if ((Left < 0) || (Right < 0) || (Top < 0) || (Bottom < 0)) return IP_RET_ERR_ARGUMENTOUTOFRANGE;
	IP_RET Ret = IP_CreateMatrix(Width + Left + Right, 1, IP_DEPTH_32S, 1, Row);
	if (Ret != IP_RET_OK) return Ret;
	Ret = IP_CreateMatrix(1, Height + Top + Bottom, IP_DEPTH_32S, 1, Col);
	if (Ret != IP_RET_OK) return Ret;

	int XX, YY, *RowPos = (int *)(*Row)->Data, *ColPos = (int *)(*Col)->Data;

	for (int X = -Left; X < Width + Right; X++)
	{
		if (X < 0)
		{
			if (Edge == EdgeMode::Tile)						// Repeating edge pixels
				RowPos[X + Left] = 0;
			else
			{
				XX = -X;
				while (XX >= Width) XX -= Width;			// mirror data
				RowPos[X + Left] = XX;
			}
		}
		else if (X >= Width)
		{
			if (Edge == EdgeMode::Tile)
				RowPos[X + Left] = Width - 1;
			else
			{
				XX = Width - (X - Width + 2);
				while (XX < 0) XX += Width;
				RowPos[X + Left] = XX;
			}
		}
		else
		{
			RowPos[X + Left] = X;
		}
	}

	for (int Y = -Top; Y < Height + Bottom; Y++)
	{
		if (Y < 0)
		{
			if (Edge == EdgeMode::Tile)
				ColPos[Y + Top] = 0;
			else
			{
				YY = -Y;
				while (YY >= Height) YY -= Height;
				ColPos[Y + Top] = YY;
			}
		}
		else if (Y >= Height)
		{
			if (Edge == EdgeMode::Tile)
				ColPos[Y + Top] = Height - 1;
			else
			{
				YY = Height - (Y - Height + 2);
				while (YY < 0) YY += Height;
				ColPos[Y + Top] = YY;
			}
		}
		else
		{
			ColPos[Y + Top] = Y;
		}
	}
	return IP_RET_OK;
}

IP_RET __stdcall GetExpandImage(TMatrix *Src, TMatrix **Dest, int Left, int Right, int Top, int Bottom, EdgeMode Edge)
{
	if (Src == NULL) return IP_RET_ERR_NULLREFERENCE;
	if (Src->Data == NULL) return IP_RET_ERR_NULLREFERENCE;
	if (Src->Depth != IP_DEPTH_8U || Left < 0 || Right < 0 || Top < 0 || Bottom < 0) return IP_RET_ERR_ARGUMENTOUTOFRANGE;
	int X, Y, SrcWidth, SrcHeight, SrcStride, DstWidth, DstHeight, DstStride, Channel;
	unsigned char *LinePS, *LinePD;

	SrcWidth = Src->Width; SrcHeight = Src->Height; DstWidth = SrcWidth + Left + Right; DstHeight = SrcHeight + Top + Bottom;

	if (*Dest != NULL)
	{
		if ((*Dest)->Data == NULL) return IP_RET_ERR_NULLREFERENCE;
		if ((*Dest)->Depth != IP_DEPTH_8U || (*Dest)->Width != DstWidth || (*Dest)->Height != DstHeight) return IP_RET_ERR_ARGUMENTOUTOFRANGE;
	}
	else
	{
		IP_RET Ret = IP_CreateMatrix(DstWidth, DstHeight, Src->Depth, Src->Channel, Dest);
		if (Ret != IP_RET_OK) return Ret;
	}

	SrcStride = Src->WidthStep; DstStride = (*Dest)->WidthStep;  Channel = Src->Channel;

	TMatrix *Row, *Col;
	GetValidCoordinate(SrcWidth, SrcHeight, Left, Right, Top, Bottom, Edge, &Row, &Col);		//	Get the coordinate offset
	int *RowPos = (int *)Row->Data, *ColPos = (int *)Col->Data;

	for (Y = 0; Y < SrcHeight; Y++)
	{
		LinePD = (*Dest)->Data + (Y + Top) * DstStride;
		LinePS = Src->Data + Y * SrcStride;
		for (X = 0; X < Left; X++)
		{
			memcpy(LinePD, LinePS + RowPos[X] * Channel, Channel);								//	The left pixel
			LinePD += Channel;																	//	Move pixels
		}
		memcpy(LinePD, LinePS, SrcWidth * Channel);												//	The middle of the pixel
		LinePD += SrcWidth * Channel;															//	Move pixels
		for (X = Left + SrcWidth; X < Left + SrcWidth + Right; X++)
		{
			memcpy(LinePD, LinePS + RowPos[X] * Channel, Channel);								//	The right pixel
			LinePD += Channel;
		}
	}
	for (Y = 0; Y < Top; Y++)
	{
		memcpy((*Dest)->Data + Y * DstStride, (*Dest)->Data + (Top + ColPos[Y]) * DstStride, DstStride);			//	Copy line directly
	}

	for (Y = Top + SrcHeight; Y < Top + SrcHeight + Bottom; Y++)
	{
		memcpy((*Dest)->Data + Y * DstStride, (*Dest)->Data + (Top + ColPos[Y]) * DstStride, DstStride);
	}
	IP_FreeMatrix(&Row);
	IP_FreeMatrix(&Col);
	return IP_RET_OK;
}