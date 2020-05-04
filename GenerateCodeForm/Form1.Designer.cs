namespace GenerateCodeForm
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
            this.ccbFishType = new System.Windows.Forms.ComboBox();
            this.ccbFishName = new System.Windows.Forms.ComboBox();
            this.label2 = new System.Windows.Forms.Label();
            this.ccbCodeType = new System.Windows.Forms.ComboBox();
            this.label3 = new System.Windows.Forms.Label();
            this.txtOriginDiskSerialNumber = new System.Windows.Forms.TextBox();
            this.txtEncryptSerialNumber = new System.Windows.Forms.TextBox();
            this.label4 = new System.Windows.Forms.Label();
            this.ccbLoseScoreType = new System.Windows.Forms.ComboBox();
            this.ccbWinScoreType = new System.Windows.Forms.ComboBox();
            this.label5 = new System.Windows.Forms.Label();
            this.label6 = new System.Windows.Forms.Label();
            this.txtCodeIndex = new System.Windows.Forms.TextBox();
            this.label7 = new System.Windows.Forms.Label();
            this.txtRandCode = new System.Windows.Forms.TextBox();
            this.btnGenerateCode = new System.Windows.Forms.Button();
            this.label8 = new System.Windows.Forms.Label();
            this.txtQueryCode = new System.Windows.Forms.TextBox();
            this.txtResult = new System.Windows.Forms.TextBox();
            this.lbConnectStatus = new System.Windows.Forms.Label();
            this.label9 = new System.Windows.Forms.Label();
            this.label10 = new System.Windows.Forms.Label();
            this.lbCodeType = new System.Windows.Forms.Label();
            this.txtMutipCode = new System.Windows.Forms.TextBox();
            this.btnShowToolsForm = new System.Windows.Forms.Button();
            this.label11 = new System.Windows.Forms.Label();
            this.cbbIsHideCode = new System.Windows.Forms.ComboBox();
            this.SuspendLayout();
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(12, 27);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(35, 12);
            this.label1.TabIndex = 0;
            this.label1.Text = "系列:";
            // 
            // ccbFishType
            // 
            this.ccbFishType.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.ccbFishType.FormattingEnabled = true;
            this.ccbFishType.Location = new System.Drawing.Point(74, 24);
            this.ccbFishType.Name = "ccbFishType";
            this.ccbFishType.Size = new System.Drawing.Size(168, 20);
            this.ccbFishType.TabIndex = 1;
            this.ccbFishType.SelectedIndexChanged += new System.EventHandler(this.ccbFishType_SelectedIndexChanged);
            // 
            // ccbFishName
            // 
            this.ccbFishName.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.ccbFishName.FormattingEnabled = true;
            this.ccbFishName.Location = new System.Drawing.Point(324, 24);
            this.ccbFishName.Name = "ccbFishName";
            this.ccbFishName.Size = new System.Drawing.Size(168, 20);
            this.ccbFishName.TabIndex = 3;
            this.ccbFishName.SelectedIndexChanged += new System.EventHandler(this.ccbFishName_SelectedIndexChanged);
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Location = new System.Drawing.Point(269, 27);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(35, 12);
            this.label2.TabIndex = 2;
            this.label2.Text = "名字:";
            // 
            // ccbCodeType
            // 
            this.ccbCodeType.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.ccbCodeType.FormattingEnabled = true;
            this.ccbCodeType.Location = new System.Drawing.Point(572, 24);
            this.ccbCodeType.Name = "ccbCodeType";
            this.ccbCodeType.Size = new System.Drawing.Size(100, 20);
            this.ccbCodeType.TabIndex = 5;
            this.ccbCodeType.SelectedIndexChanged += new System.EventHandler(this.ccbCodeType_SelectedIndexChanged);
            // 
            // label3
            // 
            this.label3.AutoSize = true;
            this.label3.Location = new System.Drawing.Point(519, 27);
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size(47, 12);
            this.label3.TabIndex = 4;
            this.label3.Text = "码分类:";
            // 
            // txtOriginDiskSerialNumber
            // 
            this.txtOriginDiskSerialNumber.Location = new System.Drawing.Point(76, 62);
            this.txtOriginDiskSerialNumber.Name = "txtOriginDiskSerialNumber";
            this.txtOriginDiskSerialNumber.Size = new System.Drawing.Size(166, 21);
            this.txtOriginDiskSerialNumber.TabIndex = 7;
            this.txtOriginDiskSerialNumber.TextChanged += new System.EventHandler(this.txtOriginDiskSerialNumber_TextChanged);
            // 
            // txtEncryptSerialNumber
            // 
            this.txtEncryptSerialNumber.Location = new System.Drawing.Point(324, 64);
            this.txtEncryptSerialNumber.Name = "txtEncryptSerialNumber";
            this.txtEncryptSerialNumber.Size = new System.Drawing.Size(166, 21);
            this.txtEncryptSerialNumber.TabIndex = 9;
            this.txtEncryptSerialNumber.TextChanged += new System.EventHandler(this.txtEncryptSerialNumber_TextChanged);
            // 
            // label4
            // 
            this.label4.AutoSize = true;
            this.label4.Location = new System.Drawing.Point(269, 107);
            this.label4.Name = "label4";
            this.label4.Size = new System.Drawing.Size(23, 12);
            this.label4.TabIndex = 10;
            this.label4.Text = "输:";
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
            this.ccbLoseScoreType.Location = new System.Drawing.Point(323, 104);
            this.ccbLoseScoreType.Name = "ccbLoseScoreType";
            this.ccbLoseScoreType.Size = new System.Drawing.Size(169, 20);
            this.ccbLoseScoreType.TabIndex = 11;
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
            this.ccbWinScoreType.Location = new System.Drawing.Point(72, 104);
            this.ccbWinScoreType.Name = "ccbWinScoreType";
            this.ccbWinScoreType.Size = new System.Drawing.Size(168, 20);
            this.ccbWinScoreType.TabIndex = 13;
            // 
            // label5
            // 
            this.label5.AutoSize = true;
            this.label5.Location = new System.Drawing.Point(12, 109);
            this.label5.Name = "label5";
            this.label5.Size = new System.Drawing.Size(23, 12);
            this.label5.TabIndex = 12;
            this.label5.Text = "赢:";
            // 
            // label6
            // 
            this.label6.AutoSize = true;
            this.label6.Location = new System.Drawing.Point(12, 150);
            this.label6.Name = "label6";
            this.label6.Size = new System.Drawing.Size(35, 12);
            this.label6.TabIndex = 14;
            this.label6.Text = "次数:";
            // 
            // txtCodeIndex
            // 
            this.txtCodeIndex.Location = new System.Drawing.Point(70, 147);
            this.txtCodeIndex.Name = "txtCodeIndex";
            this.txtCodeIndex.Size = new System.Drawing.Size(100, 21);
            this.txtCodeIndex.TabIndex = 15;
            // 
            // label7
            // 
            this.label7.AutoSize = true;
            this.label7.Location = new System.Drawing.Point(519, 65);
            this.label7.Name = "label7";
            this.label7.Size = new System.Drawing.Size(47, 12);
            this.label7.TabIndex = 16;
            this.label7.Text = "随机码:";
            // 
            // txtRandCode
            // 
            this.txtRandCode.Location = new System.Drawing.Point(572, 62);
            this.txtRandCode.Name = "txtRandCode";
            this.txtRandCode.Size = new System.Drawing.Size(100, 21);
            this.txtRandCode.TabIndex = 17;
            // 
            // btnGenerateCode
            // 
            this.btnGenerateCode.Location = new System.Drawing.Point(200, 142);
            this.btnGenerateCode.Name = "btnGenerateCode";
            this.btnGenerateCode.Size = new System.Drawing.Size(87, 29);
            this.btnGenerateCode.TabIndex = 18;
            this.btnGenerateCode.Text = "生成";
            this.btnGenerateCode.UseVisualStyleBackColor = true;
            this.btnGenerateCode.Click += new System.EventHandler(this.btnGenerateCode_Click);
            // 
            // label8
            // 
            this.label8.AutoSize = true;
            this.label8.Location = new System.Drawing.Point(12, 209);
            this.label8.Name = "label8";
            this.label8.Size = new System.Drawing.Size(47, 12);
            this.label8.TabIndex = 19;
            this.label8.Text = "取次码:";
            // 
            // txtQueryCode
            // 
            this.txtQueryCode.Font = new System.Drawing.Font("SimSun", 35F);
            this.txtQueryCode.Location = new System.Drawing.Point(72, 189);
            this.txtQueryCode.Name = "txtQueryCode";
            this.txtQueryCode.ReadOnly = true;
            this.txtQueryCode.Size = new System.Drawing.Size(228, 61);
            this.txtQueryCode.TabIndex = 20;
            // 
            // txtResult
            // 
            this.txtResult.Font = new System.Drawing.Font("SimSun", 35F);
            this.txtResult.Location = new System.Drawing.Point(70, 272);
            this.txtResult.Name = "txtResult";
            this.txtResult.Size = new System.Drawing.Size(230, 61);
            this.txtResult.TabIndex = 21;
            // 
            // lbConnectStatus
            // 
            this.lbConnectStatus.AutoSize = true;
            this.lbConnectStatus.ForeColor = System.Drawing.Color.Red;
            this.lbConnectStatus.Location = new System.Drawing.Point(305, 150);
            this.lbConnectStatus.Name = "lbConnectStatus";
            this.lbConnectStatus.Size = new System.Drawing.Size(65, 12);
            this.lbConnectStatus.TabIndex = 22;
            this.lbConnectStatus.Text = "加载中……";
            // 
            // label9
            // 
            this.label9.AutoSize = true;
            this.label9.Location = new System.Drawing.Point(12, 67);
            this.label9.Name = "label9";
            this.label9.Size = new System.Drawing.Size(59, 12);
            this.label9.TabIndex = 23;
            this.label9.Text = "未加密ID:";
            // 
            // label10
            // 
            this.label10.AutoSize = true;
            this.label10.Location = new System.Drawing.Point(269, 67);
            this.label10.Name = "label10";
            this.label10.Size = new System.Drawing.Size(47, 12);
            this.label10.TabIndex = 24;
            this.label10.Text = "加密ID:";
            // 
            // lbCodeType
            // 
            this.lbCodeType.AutoSize = true;
            this.lbCodeType.Location = new System.Drawing.Point(12, 287);
            this.lbCodeType.Name = "lbCodeType";
            this.lbCodeType.Size = new System.Drawing.Size(35, 12);
            this.lbCodeType.TabIndex = 26;
            this.lbCodeType.Text = "Text:";
            // 
            // txtMutipCode
            // 
            this.txtMutipCode.Font = new System.Drawing.Font("SimSun", 9F);
            this.txtMutipCode.Location = new System.Drawing.Point(324, 189);
            this.txtMutipCode.Multiline = true;
            this.txtMutipCode.Name = "txtMutipCode";
            this.txtMutipCode.Size = new System.Drawing.Size(348, 144);
            this.txtMutipCode.TabIndex = 27;
            // 
            // btnShowToolsForm
            // 
            this.btnShowToolsForm.Location = new System.Drawing.Point(572, 150);
            this.btnShowToolsForm.Name = "btnShowToolsForm";
            this.btnShowToolsForm.Size = new System.Drawing.Size(75, 23);
            this.btnShowToolsForm.TabIndex = 29;
            this.btnShowToolsForm.Text = "工具箱";
            this.btnShowToolsForm.UseVisualStyleBackColor = true;
            this.btnShowToolsForm.Click += new System.EventHandler(this.btnShowToolsForm_Click);
            // 
            // label11
            // 
            this.label11.AutoSize = true;
            this.label11.Location = new System.Drawing.Point(519, 109);
            this.label11.Name = "label11";
            this.label11.Size = new System.Drawing.Size(71, 12);
            this.label11.TabIndex = 30;
            this.label11.Text = "是否水印码:";
            // 
            // cbbIsHideCode
            // 
            this.cbbIsHideCode.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.cbbIsHideCode.FormattingEnabled = true;
            this.cbbIsHideCode.Items.AddRange(new object[] {
            "否",
            "是"});
            this.cbbIsHideCode.Location = new System.Drawing.Point(589, 102);
            this.cbbIsHideCode.Name = "cbbIsHideCode";
            this.cbbIsHideCode.Size = new System.Drawing.Size(58, 20);
            this.cbbIsHideCode.TabIndex = 31;
            // 
            // Form1
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 12F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(682, 361);
            this.Controls.Add(this.cbbIsHideCode);
            this.Controls.Add(this.label11);
            this.Controls.Add(this.btnShowToolsForm);
            this.Controls.Add(this.txtMutipCode);
            this.Controls.Add(this.lbCodeType);
            this.Controls.Add(this.label10);
            this.Controls.Add(this.label9);
            this.Controls.Add(this.lbConnectStatus);
            this.Controls.Add(this.txtResult);
            this.Controls.Add(this.txtQueryCode);
            this.Controls.Add(this.label8);
            this.Controls.Add(this.btnGenerateCode);
            this.Controls.Add(this.txtRandCode);
            this.Controls.Add(this.label7);
            this.Controls.Add(this.txtCodeIndex);
            this.Controls.Add(this.label6);
            this.Controls.Add(this.ccbWinScoreType);
            this.Controls.Add(this.label5);
            this.Controls.Add(this.ccbLoseScoreType);
            this.Controls.Add(this.label4);
            this.Controls.Add(this.txtEncryptSerialNumber);
            this.Controls.Add(this.txtOriginDiskSerialNumber);
            this.Controls.Add(this.ccbCodeType);
            this.Controls.Add(this.label3);
            this.Controls.Add(this.ccbFishName);
            this.Controls.Add(this.label2);
            this.Controls.Add(this.ccbFishType);
            this.Controls.Add(this.label1);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.SizableToolWindow;
            this.KeyPreview = true;
            this.Name = "Form1";
            this.Text = "打码器";
            this.FormClosing += new System.Windows.Forms.FormClosingEventHandler(this.Form1_FormClosing);
            this.Load += new System.EventHandler(this.Form1_Load);
            this.KeyDown += new System.Windows.Forms.KeyEventHandler(this.Form1_KeyDown);
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.ComboBox ccbFishType;
        private System.Windows.Forms.ComboBox ccbFishName;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.ComboBox ccbCodeType;
        private System.Windows.Forms.Label label3;
        private System.Windows.Forms.TextBox txtOriginDiskSerialNumber;
        private System.Windows.Forms.TextBox txtEncryptSerialNumber;
        private System.Windows.Forms.Label label4;
        private System.Windows.Forms.ComboBox ccbLoseScoreType;
        private System.Windows.Forms.ComboBox ccbWinScoreType;
        private System.Windows.Forms.Label label5;
        private System.Windows.Forms.Label label6;
        private System.Windows.Forms.TextBox txtCodeIndex;
        private System.Windows.Forms.Label label7;
        private System.Windows.Forms.TextBox txtRandCode;
        private System.Windows.Forms.Button btnGenerateCode;
        private System.Windows.Forms.Label label8;
        private System.Windows.Forms.TextBox txtQueryCode;
        private System.Windows.Forms.TextBox txtResult;
        private System.Windows.Forms.Label lbConnectStatus;
        private System.Windows.Forms.Label label9;
        private System.Windows.Forms.Label label10;
        private System.Windows.Forms.Label lbCodeType;
        private System.Windows.Forms.TextBox txtMutipCode;
        private System.Windows.Forms.Button btnShowToolsForm;
        private System.Windows.Forms.Label label11;
        private System.Windows.Forms.ComboBox cbbIsHideCode;
    }
}

