#include "Utility.h"

/// <summary>
/// Multiplication of SSE-based byte data.
/// </summary>
/// <param name="Kernel">Matrix that needs convolution</param>
/// <param name="Conv">Convolution matrix.</param>
/// <param name="Length">The length of all elements of the array.</param>
/// <remarks> 1: SSE optimization is used.
///	https://msdn.microsoft.com/en-us/library/t5h7783k(v=vs.90).aspx

int MultiplySSE(unsigned char* Kernel, unsigned char *Conv, int Length)
{
	int Y, Sum;
	__m128i vsum = _mm_set1_epi32(0);
	__m128i vk0 = _mm_set1_epi8(0);
	for (Y = 0; Y <= Length - 16; Y += 16)
	{
		__m128i v0 = _mm_loadu_si128((__m128i*)(Kernel + Y));				// Corresponding to the movdqu instruction, no 16 byte alignment is required
		__m128i v0l = _mm_unpacklo_epi8(v0, vk0);
		__m128i v0h = _mm_unpackhi_epi8(v0, vk0);							//	The purpose of these two lines is to load them into two 128-bit registers for the following 16-bit SSE function call of _mm_madd_epi16 (the role of vk0 is mainly to set the upper 8 bits to 0)				
		__m128i v1 = _mm_loadu_si128((__m128i*)(Conv + Y));
		__m128i v1l = _mm_unpacklo_epi8(v1, vk0);
		__m128i v1h = _mm_unpackhi_epi8(v1, vk0);
		vsum = _mm_add_epi32(vsum, _mm_madd_epi16(v0l, v1l));				//	With _mm_madd_epi16 it is possible to multiply eight 16-bit numbers at a time and then add the results of the two 16 to a 32 number, r0: = (a0 * b0) + (a1 * b1) https://msdn.microsoft.com/en-us/library/yht36sa6(v=vs.90).aspx
		vsum = _mm_add_epi32(vsum, _mm_madd_epi16(v0h, v1h));
	}
	for (; Y <= Length - 8; Y += 8)
	{
		__m128i v0 = _mm_loadl_epi64((__m128i*)(Kernel + Y));
		__m128i v0l = _mm_unpacklo_epi8(v0, vk0);
		__m128i v1 = _mm_loadl_epi64((__m128i*)(Conv + Y));
		__m128i v1l = _mm_unpacklo_epi8(v1, vk0);
		vsum = _mm_add_epi32(vsum, _mm_madd_epi16(v0l, v1l));
	}
	vsum = _mm_add_epi32(vsum, _mm_srli_si128(vsum, 8));
	vsum = _mm_add_epi32(vsum, _mm_srli_si128(vsum, 4));
	Sum = _mm_cvtsi128_si32(vsum);											// MOVD function, Moves the least significant 32 bits of a to a 32-bit integer：   r := a0

	for (; Y < Length; Y++)
	{
		Sum += Kernel[Y] * Conv[Y];
	}
	return Sum;
}

/// <summary>
/// The local square sum of the image is calculated, the speed has been optimized and 1 and 3 channel images are supported.
/// </summary>
/// <param name="Src">Source image for square sum。</param>
/// <param name="Dest">Square sum data, need to be saved using int type matrix, the size is Src->Width - SizeX + 1, Src->Height - SizeY + 1, and the data is allocated inside the program.</param>
/// <param name="SizeX">The size of the template used in the horizontal direction, if it is a radius mode, corresponds to 2 * Radius + 1.</param>
/// <param name="SizeY">The size of the template used in the vertical direction, if it is a radius mode, corresponds to 2 * Radius + 1. </param>
/// <remarks> 1:An optimization algorithm similar to BoxBlur is used.</remarks>

IP_RET GetLocalSquareSum(TMatrix *Src, TMatrix **Dest, int SizeX, int SizeY)
{
	if (Src == NULL || Src->Data == NULL) return IP_RET_ERR_NULLREFERENCE;
	if (Src->Depth != IP_DEPTH_8U || Src->Channel == 4) return IP_RET_ERR_NOTSUPPORTED;
	if (SizeX < 0 || SizeY < 0) return IP_RET_ERR_ARGUMENTOUTOFRANGE;

	int X, Y, Z, SrcW, SrcH, DestW, DestH, LastIndex, NextIndex, Sum;
	int *ColSum, *LinePD;
	unsigned char *SamplePS, *LastAddress, *NextAddress;
	IP_RET Ret = IP_RET_OK;

	SrcW = Src->Width, SrcH = Src->Height;
	DestW = SrcW - SizeX + 1, DestH = SrcH - SizeY + 1;

	Ret = IP_CreateMatrix(DestW, DestH, IP_DEPTH_32S, 1, Dest);
	if (Ret != IP_RET_OK) goto Done;
	ColSum = (int*)IP_AllocMemory(SrcW * sizeof(int), true);
	if (ColSum == NULL) { Ret = IP_RET_ERR_OUTOFMEMORY; goto Done; }

	if (Src->Channel == 1)
	{
		for (Y = 0; Y < DestH; Y++)
		{
			LinePD = (int *)((*Dest)->Data + Y * (*Dest)->WidthStep);
			if (Y == 0)
			{
				for (X = 0; X < SrcW; X++)
				{
					Sum = 0;
					for (Z = 0; Z < SizeY; Z++)
					{
						SamplePS = Src->Data + Z * Src->WidthStep + X;
						Sum += SamplePS[0] * SamplePS[0];
					}
					ColSum[X] = Sum;
				}
			}
			else
			{
				LastAddress = Src->Data + (Y - 1) * Src->WidthStep;
				NextAddress = Src->Data + (Y + SizeY - 1) * Src->WidthStep;
				for (X = 0; X < SrcW; X++)
				{
					ColSum[X] -= LastAddress[X] * LastAddress[X] - NextAddress[X] * NextAddress[X];
				}
			}
			for (X = 0; X < DestW; X++)
			{
				if (X == 0)
				{
					Sum = 0;
					for (Z = 0; Z < SizeX; Z++)
					{
						Sum += ColSum[Z];
					}
				}
				else
				{
					Sum -= ColSum[X - 1] - ColSum[X + SizeX - 1];
				}
				LinePD[X] = Sum;
			}
		}
	}
	else if (Src->Channel == 3)
	{
		for (Y = 0; Y < DestH; Y++)
		{
			LinePD = (int *)((*Dest)->Data + Y * (*Dest)->WidthStep);
			if (Y == 0)
			{
				for (X = 0; X < SrcW; X++)
				{
					Sum = 0;
					for (Z = 0; Z < SizeY; Z++)
					{
						SamplePS = Src->Data + Z * Src->WidthStep + X * 3;			//	Three channels are added together
						Sum += SamplePS[0] * SamplePS[0] + SamplePS[1] * SamplePS[1] + SamplePS[2] * SamplePS[2];
					}
					ColSum[X] = Sum;
				}
			}
			else
			{
				LastAddress = Src->Data + (Y - 1) * Src->WidthStep;
				NextAddress = Src->Data + (Y + SizeY - 1) * Src->WidthStep;
				for (X = 0; X < SrcW; X++)
				{
					ColSum[X] += NextAddress[0] * NextAddress[0] + NextAddress[1] * NextAddress[1] + NextAddress[2] * NextAddress[2] - LastAddress[0] * LastAddress[0] - LastAddress[1] * LastAddress[1] - LastAddress[2] * LastAddress[2];
					LastAddress += 3;
					NextAddress += 3;
				}
			}
			for (X = 0; X < DestW; X++)
			{
				if (X == 0)
				{
					Sum = 0;
					for (Z = 0; Z < SizeX; Z++)
					{
						Sum += ColSum[Z];
					}
				}
				else
				{
					Sum -= ColSum[X - 1] - ColSum[X + SizeX - 1];
				}
				LinePD[X] = Sum;
			}
		}
	}
Done:
	IP_FreeMemory(ColSum);
	return Ret;
}

/// <summary>
/// SSE based image convolution algorithm
/// </summary>
/// <param name="Src">The data structure of the source image that needs to be processed</param>
/// <param name="Conv">Convolution matrix, which must be image data. </param>
/// <param name="Dest">The data structure of the saved convolution result must be an int matrix。</param>
/// <remarks> 1: SSE optimization is used.
/// <remarks> 2: The SSE multiplication of the byte array is much faster than the ordinary SSE speed.</remarks>
/// <remarks> 3: Testing using FFT will be slower. </remarks>

IP_RET FastConv2(TMatrix *Src, TMatrix *Conv, TMatrix **Dest)
{
	if (Src == NULL || Conv == NULL) return IP_RET_ERR_NULLREFERENCE;
	if (Src->Data == NULL || Conv->Data == NULL) return IP_RET_ERR_NULLREFERENCE;
	if (Src->Channel != Conv->Channel || Src->Depth != Conv->Depth) return IP_RET_ERR_PARAMISMATCH;
	if (Src->Depth != IP_DEPTH_8U || Src->Channel == 4) return IP_RET_ERR_NOTSUPPORTED;

	int X, Y, Length, *LinePD;
	int SrcW, SrcH, DestW, DestH, ConvW, ConvH;
	unsigned char *LinePS, *CurKer, *Conv16, *Kernel;
	IP_RET Ret = IP_RET_OK;

	SrcW = Src->Width, SrcH = Src->Height, ConvW = Conv->Width, ConvH = Conv->Height;
	DestW = SrcW - ConvW + 1, DestH = SrcH - ConvH + 1, Length = ConvW * ConvH * Src->Channel;

	Ret = IP_CreateMatrix(DestW, DestH, IP_DEPTH_32S, 1, Dest);
	if (Ret != IP_RET_OK) goto Done;
	Conv16 = (unsigned char *)IP_AllocMemory(ConvW * ConvH * Src->Channel);		//	Save the convolution matrix to remove potentially useless data in the original Conv, and note that there is no 16 byte alignment here.
	if (Conv16 == NULL) { Ret = IP_RET_ERR_OUTOFMEMORY; goto Done; }
	Kernel = (unsigned char *)IP_AllocMemory(ConvW * SrcH * Src->Channel);		//	Save the 16 byte aligned convolution kernel matrix to facilitate the use of SSE
	if (Kernel == NULL) { Ret = IP_RET_ERR_OUTOFMEMORY; goto Done; }

	for (Y = 0; Y < ConvH; Y++) 
		memcpy(Conv16 + Y * ConvW * Src->Channel, Conv->Data + Y * Conv->WidthStep, ConvW * Src->Channel);	//	Replicating the data of the convolution matrix
	for (Y = 0; Y < SrcH; Y++)	
		memcpy(Kernel + Y * ConvW * Src->Channel, Src->Data + Y * Src->WidthStep, ConvW * Src->Channel);	//	Calculation of the convolution core data to be sampled in the first column of all pixels

	for (X = 0; X < DestW; X++)
	{
		if (X != 0)													//	If it's not the first column, you need to update the data of the convolution kernel
		{
			memcpy(Kernel, Kernel + Src->Channel, (ConvW * SrcH - 1) * Src->Channel);			//	Move data forward
			LinePS = Src->Data + (X + ConvW - 1) * Src->Channel;
			CurKer = Kernel + (ConvW - 1) * Src->Channel;
			if (Src->Channel == 1)
			{
				for (Y = 0; Y < SrcH; Y++)
				{
					CurKer[0] = LinePS[0];								//	Refresh the next element
					CurKer += ConvW;
					LinePS += Src->WidthStep;
				}
			}
			else
			{
				for (Y = 0; Y < SrcH; Y++)
				{
					CurKer[0] = LinePS[0];								//	Refresh the next element
					CurKer[1] = LinePS[1];
					CurKer[2] = LinePS[2];
					CurKer += ConvW * 3;
					LinePS += Src->WidthStep;
				}
			}
		}

		CurKer = Kernel, LinePD = (int *)(*Dest)->Data + X;
		for (Y = 0; Y < DestH; Y++)										//	Update in the direction of the column
		{
			LinePD[0] = MultiplySSE(Conv16, CurKer, Length);			//	The color images are also just added together.	
			CurKer += ConvW * Src->Channel;
			LinePD += DestW;
		}
	}

Done:
	IP_FreeMemory(Conv16);
	IP_FreeMemory(Kernel);
	return Ret;
}

int GetPowerSum(TMatrix *Src)
{
	if (Src == NULL || Src->Data == NULL) return 0;
	if (Src->Depth != IP_DEPTH_8U) return 0;

	int X, Y, Sum, Width = Src->Width, Height = Src->Height;
	unsigned char *LinePS;

	if (Src->Channel == 1)
	{
		for (Y = 0, Sum = 0; Y < Height; Y++)
		{
			LinePS = Src->Data + Y * Src->WidthStep;
			for (X = 0; X < Width; X++)
			{
				Sum += LinePS[X] * LinePS[X];
			}
		}
	}
	else
	{
		for (Y = 0, Sum = 0; Y < Height; Y++)
		{
			LinePS = Src->Data + Y * Src->WidthStep;
			for (X = 0; X < Width; X++)
			{
				Sum += LinePS[0] * LinePS[0] + LinePS[1] * LinePS[1] + LinePS[2] * LinePS[2];
				LinePS += 3;
			}
		}
	}
	return Sum;
}

/// <summary>
/// Calculates the cumulative square difference of the image (speed optimized)
/// </summary>
/// <param name="Src">Source image, our template image finds the cumulative squared difference in this image.</param>
/// <param name="Template">The width and height of the template image should be smaller than the source image.</param>
/// <param name="Dest">The result image, the size must be Src->Width - Template->Width + 1, Src->Height - Template->Height + 1.</param>
/// <remarks> 1:Cumulative mean square variance is the cumulative sum of squares of the difference between two images corresponding to position pixels and: (a-b) ^2 = a^2 + b^2 - 2Ab.</remarks>
/// <remarks> 2:A (template) of the square is a fixed value, B (a small map source shown) square can be used to achieve a fast integral graph, a*b can achieve fast convolution.</remarks>

IP_RET __stdcall MatchTemplate(TMatrix *Src, TMatrix *Template, TMatrix **Dest)
{
	if (Src == NULL || Template == NULL) return IP_RET_ERR_NULLREFERENCE;
	if (Src->Data == NULL || Template->Data == NULL) return IP_RET_ERR_NULLREFERENCE;
	if (Src->Width <= Template->Width || Src->Height <= Template->Height || Src->Channel != Template->Channel || Src->Depth != Template->Depth) return IP_RET_ERR_PARAMISMATCH;
	if (Src->Depth != IP_DEPTH_8U || Src->Channel == 4) return IP_RET_ERR_NOTSUPPORTED;

	int X, Y, Width, Height, PowerSum;
	int *LinePL, *LinePC, *LinePD;
	TMatrix *LocalSquareSum = NULL, *XY = NULL;
	IP_RET Ret = IP_RET_OK;

	Width = Src->Width - Template->Width + 1, Height = Src->Height - Template->Height + 1;

	Ret = IP_CreateMatrix(Width, Height, IP_DEPTH_32S, 1, Dest);								//	Allocation of data
	if (Ret != IP_RET_OK) goto Done;
	Ret = GetLocalSquareSum(Src, &LocalSquareSum, Template->Width, Template->Height);
	if (Ret != IP_RET_OK) goto Done;
	Ret = FastConv2(Src, Template, &XY);
	if (Ret != IP_RET_OK) goto Done;
	PowerSum = GetPowerSum(Template);

	for (Y = 0; Y < Height; Y++)
	{
		LinePL = (int *)(LocalSquareSum->Data + Y * LocalSquareSum->WidthStep);
		LinePC = (int *)(XY->Data + Y * XY->WidthStep);
		LinePD = (int *)((*Dest)->Data + Y * (*Dest)->WidthStep);
		for (X = 0; X < Width; X++)
		{
			LinePD[X] = PowerSum + LinePL[X] - 2 * LinePC[X];					//	a^2 + b^2 - 2ab
		}
	}
Done:
	IP_FreeMatrix(&LocalSquareSum);
	IP_FreeMatrix(&XY);
	return Ret;
}

IP_RET __stdcall MinMaxLoc(TMatrix *Src, int &Min_PosX, int &Min_PosY, int &Max_PosX, int &Max_PosY)
{
	if (Src == NULL || Src->Data == NULL) return IP_RET_ERR_NULLREFERENCE;
	if (Src->Depth != IP_DEPTH_32S || Src->Channel != 1) return IP_RET_ERR_PARAMISMATCH;

	int X, Y, Width, Height, Min, Max, Value, *LinePS;

	Width = Src->Width, Height = Src->Height;
	Min = MaxValueI, Max = MinValueI;

	for (Y = 0; Y < Height; Y++)
	{
		LinePS = (int *)(Src->Data + Y * Src->WidthStep);
		for (X = 0; X < Width; X++)
		{
			Value = LinePS[X];
			if (Min > Value)
			{
				Min = Value;
				Min_PosX = X;
				Min_PosY = Y;
			}
			if (Max < Value)
			{
				Max = Value;
				Max_PosX = X;
				Max_PosY = Y;
			}
		}
	}
Done:
	return IP_RET_OK;
}