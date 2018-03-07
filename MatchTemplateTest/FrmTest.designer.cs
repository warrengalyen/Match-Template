namespace MatchTemplateTest
{
    unsafe partial class FrmTest
    {
        /// <summary>
        /// Required designer variables.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// Clean up all the resources in use.
        /// </summary>
        /// <param name="disposing">True if the managed resource should be released; otherwise, false.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows Form Designer generated code

        /// <summary>
        /// Method Required for Designer Support
        /// Do not modify the contents of this method using a code editor.
        /// </summary>
        private void InitializeComponent()
        {
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(FrmTest));
            this.CmdOpen = new System.Windows.Forms.Button();
            this.CmdSave = new System.Windows.Forms.Button();
            this.PicDest = new System.Windows.Forms.PictureBox();
            this.PicSrc = new System.Windows.Forms.PictureBox();
            this.LblInfo = new System.Windows.Forms.Label();
            this.CmdProcess = new System.Windows.Forms.Button();
            ((System.ComponentModel.ISupportInitialize)(this.PicDest)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.PicSrc)).BeginInit();
            this.SuspendLayout();
            // 
            // CmdOpen
            // 
            this.CmdOpen.Location = new System.Drawing.Point(4, 7);
            this.CmdOpen.Name = "CmdOpen";
            this.CmdOpen.Size = new System.Drawing.Size(75, 31);
            this.CmdOpen.TabIndex = 21;
            this.CmdOpen.Text = "Open image";
            this.CmdOpen.UseVisualStyleBackColor = true;
            this.CmdOpen.Click += new System.EventHandler(this.CmdOpen_Click);
            // 
            // CmdSave
            // 
            this.CmdSave.Location = new System.Drawing.Point(85, 7);
            this.CmdSave.Name = "CmdSave";
            this.CmdSave.Size = new System.Drawing.Size(75, 31);
            this.CmdSave.TabIndex = 35;
            this.CmdSave.Text = "Save image";
            this.CmdSave.UseVisualStyleBackColor = true;
            this.CmdSave.Click += new System.EventHandler(this.CmdSave_Click);
            // 
            // PicDest
            // 
            this.PicDest.Image = ((System.Drawing.Image)(resources.GetObject("PicDest.Image")));
            this.PicDest.Location = new System.Drawing.Point(12, 55);
            this.PicDest.Name = "PicDest";
            this.PicDest.Size = new System.Drawing.Size(960, 540);
            this.PicDest.SizeMode = System.Windows.Forms.PictureBoxSizeMode.AutoSize;
            this.PicDest.TabIndex = 34;
            this.PicDest.TabStop = false;
            // 
            // PicSrc
            // 
            this.PicSrc.Image = ((System.Drawing.Image)(resources.GetObject("PicSrc.Image")));
            this.PicSrc.Location = new System.Drawing.Point(1005, 55);
            this.PicSrc.Name = "PicSrc";
            this.PicSrc.Size = new System.Drawing.Size(39, 36);
            this.PicSrc.SizeMode = System.Windows.Forms.PictureBoxSizeMode.AutoSize;
            this.PicSrc.TabIndex = 33;
            this.PicSrc.TabStop = false;
            // 
            // LblInfo
            // 
            this.LblInfo.AutoSize = true;
            this.LblInfo.Location = new System.Drawing.Point(269, 15);
            this.LblInfo.Name = "LblInfo";
            this.LblInfo.Size = new System.Drawing.Size(25, 13);
            this.LblInfo.TabIndex = 36;
            this.LblInfo.Text = "      ";
            // 
            // CmdProcess
            // 
            this.CmdProcess.Location = new System.Drawing.Point(166, 7);
            this.CmdProcess.Name = "CmdProcess";
            this.CmdProcess.Size = new System.Drawing.Size(75, 31);
            this.CmdProcess.TabIndex = 37;
            this.CmdProcess.Text = "Process";
            this.CmdProcess.UseVisualStyleBackColor = true;
            this.CmdProcess.Click += new System.EventHandler(this.CmdProcess_Click);
            // 
            // FrmTest
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(1257, 706);
            this.Controls.Add(this.CmdProcess);
            this.Controls.Add(this.LblInfo);
            this.Controls.Add(this.CmdSave);
            this.Controls.Add(this.PicDest);
            this.Controls.Add(this.PicSrc);
            this.Controls.Add(this.CmdOpen);
            this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
            this.MinimizeBox = false;
            this.Name = "FrmTest";
            this.StartPosition = System.Windows.Forms.FormStartPosition.CenterScreen;
            this.Text = "Match Template Test";
            this.Load += new System.EventHandler(this.FrmTest_Load);
            ((System.ComponentModel.ISupportInitialize)(this.PicDest)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.PicSrc)).EndInit();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Button CmdOpen;
        private System.Windows.Forms.PictureBox PicSrc;
        private System.Windows.Forms.PictureBox PicDest;
        private System.Windows.Forms.Button CmdSave;
        private System.Windows.Forms.Label LblInfo;
        private System.Windows.Forms.Button CmdProcess;

    }
}

