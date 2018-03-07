using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Drawing.Imaging;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using System.Runtime.InteropServices;
using System.Diagnostics;

namespace MatchTemplateTest
{
    public unsafe partial class FrmTest : Form
    {
        enum IP_DEPTH
        {
            IP_DEPTH_8U = 0,
            IP_DEPTH_8S = 1,
            IP_DEPTH_16S = 2,
            IP_DEPTH_32S = 3,
            IP_DEPTH_32F = 4,
            IP_DEPTH_64F = 5,
        };

        enum IP_RET
        {
            IP_RET_OK,
            IP_RET_ERR_OUTOFMEMORY,
            IP_RET_ERR_STACKOVERFLOW,
            IP_RET_ERR_NULLREFERENCE,
            IP_RET_ERR_ARGUMENTOUTOFRANGE,
            IP_RET_ERR_PARAMISMATCH,
            IP_RET_ERR_DIVIDEBYZERO,
            IP_RET_ERR_INDEXOUTOFRANGE,
            IP_RET_ERR_NOTSUPPORTED,
            IP_RET_ERR_OVERFLOW,
            IP_RET_ERR_FILENOTFOUND,
            IP_RET_ERR_UNKNOWN
        };
  

        [StructLayout(LayoutKind.Sequential)]   
        public unsafe struct TMatrix
        {
            public int Width;
            public int Height;
            public int WidthStep;
            public int Channel;
            public int Depth;
            public byte* Data;
            public int Reserved;

            public TMatrix(int Width, int Height, int WidthStep, int Depth, int Channel, byte* Scan0)
            {
                this.Width = Width;
                this.Height = Height;
                this.WidthStep = WidthStep;
                this.Depth = Depth;
                this.Channel = Channel;
                this.Data = Scan0;
                this.Reserved = 0;
            }
        };
        

        [DllImport("MatchTemplate.dll", CallingConvention = CallingConvention.StdCall, CharSet = CharSet.Unicode, ExactSpelling = true)]
        private static extern IP_RET MatchTemplate(ref TMatrix Src, ref TMatrix Template, ref TMatrix *Dest);

        [DllImport("MatchTemplate.dll", CallingConvention = CallingConvention.StdCall, CharSet = CharSet.Unicode, ExactSpelling = true)]
        private static extern IP_RET MinMaxLoc(ref TMatrix Src, ref int Min_PosX, ref int Min_PosY, ref int Max_PosX, ref int Max_PosY);

        [DllImport("MatchTemplate.dll", CallingConvention = CallingConvention.StdCall, CharSet = CharSet.Unicode, ExactSpelling = true)]
        private static extern IP_RET IP_FreeMatrix(TMatrix** DstImg);

        Bitmap SrcBmp, DstBmp;
        TMatrix SrcImg, DestImg;

        public FrmTest()
        {
            InitializeComponent();
        }
     

        private void CmdOpen_Click(object sender, EventArgs e)
        {

            OpenFileDialog openFileDialog = new OpenFileDialog();
            openFileDialog.Filter = "All files(*.*) |*.*|Bitmap files (*.Bitmap)|*.Bmp|Jpeg files (*.jpg)|*.jpg|Png files (*.png)|*.png";
            if (openFileDialog.ShowDialog() == DialogResult.OK)
            {
                Bitmap Bmp = (Bitmap)Bitmap.FromFile(openFileDialog.FileName);
                SrcBmp = (Bitmap)Bmp.Clone();
                Bmp.Dispose();
                PicSrc.Image = SrcBmp;
            }
        }
            


        private void FrmTest_Load(object sender, EventArgs e)
        {
            SrcBmp = (Bitmap)PicSrc.Image;
            DstBmp = (Bitmap)PicDest.Image;
     
        }

        private void CmdSave_Click(object sender, EventArgs e)
        {

            SaveFileDialog saveFileDialog = new SaveFileDialog();
            saveFileDialog.Filter = "Bitmap files (*.Bitmap)|*.Bmp|Jpeg files (*.jpg)|*.jpg|Png files (*.png)|*.png";
            saveFileDialog.FilterIndex = 3;
            saveFileDialog.RestoreDirectory = true;
            if (saveFileDialog.ShowDialog() == DialogResult.OK)
            {
                if (saveFileDialog.FilterIndex == 1)
                    DstBmp.Save(saveFileDialog.FileName, ImageFormat.Bmp);
                else if (saveFileDialog.FilterIndex == 2)
                    DstBmp.Save(saveFileDialog.FileName, ImageFormat.Jpeg);
                else if (saveFileDialog.FilterIndex == 3)
                    DstBmp.Save(saveFileDialog.FileName, ImageFormat.Png);
            }
        }

        void ShowResult()
        {
           
        }



        private void CmdProcess_Click(object sender, EventArgs e)
        {
             int Min_PosX = 0, Min_PosY = 0, Max_PosX = 0, Max_PosY = 0;

            BitmapData SrcBmpData = SrcBmp.LockBits(new Rectangle(0, 0, SrcBmp.Width, SrcBmp.Height), ImageLockMode.ReadWrite, PixelFormat.Format24bppRgb);
            BitmapData DstBmpData = DstBmp.LockBits(new Rectangle(0, 0, DstBmp.Width, DstBmp.Height), ImageLockMode.ReadWrite, PixelFormat.Format24bppRgb);
       
            Stopwatch Sw = new Stopwatch();
            Sw.Start();

            SrcImg = new TMatrix(SrcBmp.Width, SrcBmp.Height, SrcBmpData.Stride, (int)IP_DEPTH.IP_DEPTH_8U, 3, (byte*)SrcBmpData.Scan0);
            DestImg = new TMatrix(DstBmp.Width, DstBmp.Height, DstBmpData.Stride, (int)IP_DEPTH.IP_DEPTH_8U, 3, (byte*)DstBmpData.Scan0);

            TMatrix* Dest = null;
            MatchTemplate(ref DestImg, ref SrcImg, ref Dest);
            MinMaxLoc(ref *Dest, ref Min_PosX, ref Min_PosY, ref Max_PosX, ref  Max_PosY);
            IP_FreeMatrix(&Dest);
            SrcBmp.UnlockBits(SrcBmpData);
            DstBmp.UnlockBits(DstBmpData);

            Pen P = new Pen(Color.Red);
            Graphics G = Graphics.FromImage(DstBmp);
            G.DrawRectangle(P, new Rectangle(Min_PosX, Min_PosY, SrcImg.Width, SrcImg.Height));
            P.Dispose();
            G.Dispose();
        
            LblInfo.Text = "Image size: " + PicSrc.Image.Width.ToString() + " X " + PicSrc.Image.Height.ToString() + "\nAlgorithm processing time "  + Sw.ElapsedMilliseconds.ToString() + " ms";
            PicDest.Refresh();
        }

      
       

    }
}
