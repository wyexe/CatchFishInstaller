namespace SingleGenerateCodeForm
{
    partial class Form1
    {
        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// Clean up any resources being used.
        /// </summary>
        /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
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
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            this.label1 = new System.Windows.Forms.Label();
            this.lbFishType = new System.Windows.Forms.Label();
            this.label3 = new System.Windows.Forms.Label();
            this.lbFishName = new System.Windows.Forms.Label();
            this.label5 = new System.Windows.Forms.Label();
            this.lbDiskID = new System.Windows.Forms.Label();
            this.ccbWinScoreType = new System.Windows.Forms.ComboBox();
            this.label7 = new System.Windows.Forms.Label();
            this.ccbLoseScoreType = new System.Windows.Forms.ComboBox();
            this.label8 = new System.Windows.Forms.Label();
            this.lbCodeType = new System.Windows.Forms.Label();
            this.txtResult = new System.Windows.Forms.TextBox();
            this.txtQueryCode = new System.Windows.Forms.TextBox();
            this.label9 = new System.Windows.Forms.Label();
            this.btnGenerateCode = new System.Windows.Forms.Button();
            this.txtCodeIndex = new System.Windows.Forms.TextBox();
            this.label10 = new System.Windows.Forms.Label();
            this.txtRandCode = new System.Windows.Forms.TextBox();
            this.lbRandCode = new System.Windows.Forms.Label();
            this.SuspendLayout();
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(17, 27);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(35, 12);
            this.label1.TabIndex = 0;
            this.label1.Text = "系列:";
            // 
            // lbFishType
            // 
            this.lbFishType.AutoSize = true;
            this.lbFishType.Location = new System.Drawing.Point(68, 27);
            this.lbFishType.Name = "lbFishType";
            this.lbFishType.Size = new System.Drawing.Size(41, 12);
            this.lbFishType.TabIndex = 1;
            this.lbFishType.Text = "......";
            // 
            // label3
            // 
            this.label3.AutoSize = true;
            this.label3.Location = new System.Drawing.Point(297, 27);
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size(35, 12);
            this.label3.TabIndex = 2;
            this.label3.Text = "名字:";
            // 
            // lbFishName
            // 
            this.lbFishName.AutoSize = true;
            this.lbFishName.Location = new System.Drawing.Point(349, 27);
            this.lbFishName.Name = "lbFishName";
            this.lbFishName.Size = new System.Drawing.Size(41, 12);
            this.lbFishName.TabIndex = 3;
            this.lbFishName.Text = "......";
            // 
            // label5
            // 
            this.label5.AutoSize = true;
            this.label5.Location = new System.Drawing.Point(17, 63);
            this.label5.Name = "label5";
            this.label5.Size = new System.Drawing.Size(23, 12);
            this.label5.TabIndex = 4;
            this.label5.Text = "ID:";
            // 
            // lbDiskID
            // 
            this.lbDiskID.AutoSize = true;
            this.lbDiskID.Location = new System.Drawing.Point(68, 63);
            this.lbDiskID.Name = "lbDiskID";
            this.lbDiskID.Size = new System.Drawing.Size(41, 12);
            this.lbDiskID.TabIndex = 5;
            this.lbDiskID.Text = "......";
            // 
            // ccbWinScoreType
            // 
            this.ccbWinScoreType.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.ccbWinScoreType.FormattingEnabled = true;
            this.ccbWinScoreType.Items.AddRange(new object[] {
            "1000",
            "2000",
            "3000",
            "5000",
            "8000",
            "10000",
            "15000",
            "20000",
            "100000"});
            this.ccbWinScoreType.Location = new System.Drawing.Point(68, 97);
            this.ccbWinScoreType.Name = "ccbWinScoreType";
            this.ccbWinScoreType.Size = new System.Drawing.Size(168, 20);
            this.ccbWinScoreType.TabIndex = 17;
            this.ccbWinScoreType.SelectedIndexChanged += new System.EventHandler(this.ccbWinScoreType_SelectedIndexChanged);
            // 
            // label7
            // 
            this.label7.AutoSize = true;
            this.label7.Location = new System.Drawing.Point(17, 102);
            this.label7.Name = "label7";
            this.label7.Size = new System.Drawing.Size(23, 12);
            this.label7.TabIndex = 16;
            this.label7.Text = "赢:";
            // 
            // ccbLoseScoreType
            // 
            this.ccbLoseScoreType.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.ccbLoseScoreType.FormattingEnabled = true;
            this.ccbLoseScoreType.Items.AddRange(new object[] {
            "0",
            "1000",
            "3000",
            "5000",
            "10000"});
            this.ccbLoseScoreType.Location = new System.Drawing.Point(351, 97);
            this.ccbLoseScoreType.Name = "ccbLoseScoreType";
            this.ccbLoseScoreType.Size = new System.Drawing.Size(169, 20);
            this.ccbLoseScoreType.TabIndex = 15;
            // 
            // label8
            // 
            this.label8.AutoSize = true;
            this.label8.Location = new System.Drawing.Point(297, 100);
            this.label8.Name = "label8";
            this.label8.Size = new System.Drawing.Size(23, 12);
            this.label8.TabIndex = 14;
            this.label8.Text = "输:";
            // 
            // lbCodeType
            // 
            this.lbCodeType.AutoSize = true;
            this.lbCodeType.Location = new System.Drawing.Point(17, 285);
            this.lbCodeType.Name = "lbCodeType";
            this.lbCodeType.Size = new System.Drawing.Size(47, 12);
            this.lbCodeType.TabIndex = 33;
            this.lbCodeType.Text = "赢分码:";
            // 
            // txtResult
            // 
            this.txtResult.Font = new System.Drawing.Font("SimSun", 35F);
            this.txtResult.Location = new System.Drawing.Point(68, 270);
            this.txtResult.Name = "txtResult";
            this.txtResult.Size = new System.Drawing.Size(230, 61);
            this.txtResult.TabIndex = 32;
            // 
            // txtQueryCode
            // 
            this.txtQueryCode.Font = new System.Drawing.Font("SimSun", 35F);
            this.txtQueryCode.Location = new System.Drawing.Point(68, 187);
            this.txtQueryCode.Name = "txtQueryCode";
            this.txtQueryCode.ReadOnly = true;
            this.txtQueryCode.Size = new System.Drawing.Size(228, 61);
            this.txtQueryCode.TabIndex = 31;
            // 
            // label9
            // 
            this.label9.AutoSize = true;
            this.label9.Location = new System.Drawing.Point(17, 207);
            this.label9.Name = "label9";
            this.label9.Size = new System.Drawing.Size(47, 12);
            this.label9.TabIndex = 30;
            this.label9.Text = "取次码:";
            // 
            // btnGenerateCode
            // 
            this.btnGenerateCode.Location = new System.Drawing.Point(198, 140);
            this.btnGenerateCode.Name = "btnGenerateCode";
            this.btnGenerateCode.Size = new System.Drawing.Size(87, 29);
            this.btnGenerateCode.TabIndex = 29;
            this.btnGenerateCode.Text = "生成";
            this.btnGenerateCode.UseVisualStyleBackColor = true;
            this.btnGenerateCode.Click += new System.EventHandler(this.btnGenerateCode_Click);
            // 
            // txtCodeIndex
            // 
            this.txtCodeIndex.Location = new System.Drawing.Point(68, 145);
            this.txtCodeIndex.Name = "txtCodeIndex";
            this.txtCodeIndex.Size = new System.Drawing.Size(100, 21);
            this.txtCodeIndex.TabIndex = 28;
            // 
            // label10
            // 
            this.label10.AutoSize = true;
            this.label10.Location = new System.Drawing.Point(17, 148);
            this.label10.Name = "label10";
            this.label10.Size = new System.Drawing.Size(35, 12);
            this.label10.TabIndex = 27;
            this.label10.Text = "次数:";
            // 
            // txtRandCode
            // 
            this.txtRandCode.Location = new System.Drawing.Point(351, 61);
            this.txtRandCode.Name = "txtRandCode";
            this.txtRandCode.Size = new System.Drawing.Size(100, 21);
            this.txtRandCode.TabIndex = 35;
            // 
            // lbRandCode
            // 
            this.lbRandCode.AutoSize = true;
            this.lbRandCode.Location = new System.Drawing.Point(298, 64);
            this.lbRandCode.Name = "lbRandCode";
            this.lbRandCode.Size = new System.Drawing.Size(47, 12);
            this.lbRandCode.TabIndex = 34;
            this.lbRandCode.Text = "随机码:";
            // 
            // Form1
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 12F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(533, 381);
            this.Controls.Add(this.txtRandCode);
            this.Controls.Add(this.lbRandCode);
            this.Controls.Add(this.lbCodeType);
            this.Controls.Add(this.txtResult);
            this.Controls.Add(this.txtQueryCode);
            this.Controls.Add(this.label9);
            this.Controls.Add(this.btnGenerateCode);
            this.Controls.Add(this.txtCodeIndex);
            this.Controls.Add(this.label10);
            this.Controls.Add(this.ccbWinScoreType);
            this.Controls.Add(this.label7);
            this.Controls.Add(this.ccbLoseScoreType);
            this.Controls.Add(this.label8);
            this.Controls.Add(this.lbDiskID);
            this.Controls.Add(this.label5);
            this.Controls.Add(this.lbFishName);
            this.Controls.Add(this.label3);
            this.Controls.Add(this.lbFishType);
            this.Controls.Add(this.label1);
            this.Name = "Form1";
            this.Text = "Form1";
            this.FormClosing += new System.Windows.Forms.FormClosingEventHandler(this.Form1_FormClosing);
            this.Load += new System.EventHandler(this.Form1_Load);
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.Label lbFishType;
        private System.Windows.Forms.Label label3;
        private System.Windows.Forms.Label lbFishName;
        private System.Windows.Forms.Label label5;
        private System.Windows.Forms.Label lbDiskID;
        private System.Windows.Forms.ComboBox ccbWinScoreType;
        private System.Windows.Forms.Label label7;
        private System.Windows.Forms.ComboBox ccbLoseScoreType;
        private System.Windows.Forms.Label label8;
        private System.Windows.Forms.Label lbCodeType;
        private System.Windows.Forms.TextBox txtResult;
        private System.Windows.Forms.TextBox txtQueryCode;
        private System.Windows.Forms.Label label9;
        private System.Windows.Forms.Button btnGenerateCode;
        private System.Windows.Forms.TextBox txtCodeIndex;
        private System.Windows.Forms.Label label10;
        private System.Windows.Forms.TextBox txtRandCode;
        private System.Windows.Forms.Label lbRandCode;
    }
}

