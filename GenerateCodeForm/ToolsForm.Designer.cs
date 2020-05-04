namespace GenerateCodeForm
{
    partial class ToolsForm
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
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(ToolsForm));
            this.groupBox1 = new System.Windows.Forms.GroupBox();
            this.btnGenerateChildCodeTools = new System.Windows.Forms.Button();
            this.label10 = new System.Windows.Forms.Label();
            this.ccbFishName = new System.Windows.Forms.ComboBox();
            this.txtEncryptSerialNumber = new System.Windows.Forms.TextBox();
            this.label2 = new System.Windows.Forms.Label();
            this.ccbFishType = new System.Windows.Forms.ComboBox();
            this.label1 = new System.Windows.Forms.Label();
            this.gbCreateNewGame = new System.Windows.Forms.GroupBox();
            this.txtJsonText = new System.Windows.Forms.TextBox();
            this.btnUpload = new System.Windows.Forms.Button();
            this.txtFishH = new System.Windows.Forms.TextBox();
            this.label7 = new System.Windows.Forms.Label();
            this.txtFish0 = new System.Windows.Forms.TextBox();
            this.label6 = new System.Windows.Forms.Label();
            this.cbbGenerateCodeEnum = new System.Windows.Forms.ComboBox();
            this.label5 = new System.Windows.Forms.Label();
            this.groupBox3 = new System.Windows.Forms.GroupBox();
            this.btnDecodeFile = new System.Windows.Forms.Button();
            this.groupBox1.SuspendLayout();
            this.gbCreateNewGame.SuspendLayout();
            this.groupBox3.SuspendLayout();
            this.SuspendLayout();
            // 
            // groupBox1
            // 
            this.groupBox1.Controls.Add(this.btnGenerateChildCodeTools);
            this.groupBox1.Controls.Add(this.label10);
            this.groupBox1.Controls.Add(this.ccbFishName);
            this.groupBox1.Controls.Add(this.txtEncryptSerialNumber);
            this.groupBox1.Controls.Add(this.label2);
            this.groupBox1.Controls.Add(this.ccbFishType);
            this.groupBox1.Controls.Add(this.label1);
            this.groupBox1.Location = new System.Drawing.Point(21, 12);
            this.groupBox1.Name = "groupBox1";
            this.groupBox1.Size = new System.Drawing.Size(253, 189);
            this.groupBox1.TabIndex = 35;
            this.groupBox1.TabStop = false;
            this.groupBox1.Text = "定制打码器";
            // 
            // btnGenerateChildCodeTools
            // 
            this.btnGenerateChildCodeTools.Location = new System.Drawing.Point(76, 131);
            this.btnGenerateChildCodeTools.Name = "btnGenerateChildCodeTools";
            this.btnGenerateChildCodeTools.Size = new System.Drawing.Size(112, 48);
            this.btnGenerateChildCodeTools.TabIndex = 36;
            this.btnGenerateChildCodeTools.Text = "生成";
            this.btnGenerateChildCodeTools.UseVisualStyleBackColor = true;
            this.btnGenerateChildCodeTools.Click += new System.EventHandler(this.btnGenerateChildCodeTools_Click);
            // 
            // label10
            // 
            this.label10.AutoSize = true;
            this.label10.Location = new System.Drawing.Point(2, 95);
            this.label10.Name = "label10";
            this.label10.Size = new System.Drawing.Size(47, 12);
            this.label10.TabIndex = 37;
            this.label10.Text = "加密ID:";
            // 
            // ccbFishName
            // 
            this.ccbFishName.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.ccbFishName.FormattingEnabled = true;
            this.ccbFishName.Location = new System.Drawing.Point(57, 63);
            this.ccbFishName.Name = "ccbFishName";
            this.ccbFishName.Size = new System.Drawing.Size(168, 20);
            this.ccbFishName.TabIndex = 39;
            // 
            // txtEncryptSerialNumber
            // 
            this.txtEncryptSerialNumber.Location = new System.Drawing.Point(57, 92);
            this.txtEncryptSerialNumber.Name = "txtEncryptSerialNumber";
            this.txtEncryptSerialNumber.Size = new System.Drawing.Size(166, 21);
            this.txtEncryptSerialNumber.TabIndex = 36;
            this.txtEncryptSerialNumber.TextChanged += new System.EventHandler(this.txtEncryptSerialNumber_TextChanged);
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Location = new System.Drawing.Point(6, 66);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(35, 12);
            this.label2.TabIndex = 38;
            this.label2.Text = "名字:";
            // 
            // ccbFishType
            // 
            this.ccbFishType.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.ccbFishType.FormattingEnabled = true;
            this.ccbFishType.Location = new System.Drawing.Point(57, 31);
            this.ccbFishType.Name = "ccbFishType";
            this.ccbFishType.Size = new System.Drawing.Size(168, 20);
            this.ccbFishType.TabIndex = 37;
            this.ccbFishType.SelectedIndexChanged += new System.EventHandler(this.ccbFishType_SelectedIndexChanged);
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(6, 34);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(35, 12);
            this.label1.TabIndex = 36;
            this.label1.Text = "系列:";
            // 
            // gbCreateNewGame
            // 
            this.gbCreateNewGame.Controls.Add(this.txtJsonText);
            this.gbCreateNewGame.Controls.Add(this.btnUpload);
            this.gbCreateNewGame.Controls.Add(this.txtFishH);
            this.gbCreateNewGame.Controls.Add(this.label7);
            this.gbCreateNewGame.Controls.Add(this.txtFish0);
            this.gbCreateNewGame.Controls.Add(this.label6);
            this.gbCreateNewGame.Controls.Add(this.cbbGenerateCodeEnum);
            this.gbCreateNewGame.Controls.Add(this.label5);
            this.gbCreateNewGame.Location = new System.Drawing.Point(280, 18);
            this.gbCreateNewGame.Name = "gbCreateNewGame";
            this.gbCreateNewGame.Size = new System.Drawing.Size(456, 465);
            this.gbCreateNewGame.TabIndex = 36;
            this.gbCreateNewGame.TabStop = false;
            this.gbCreateNewGame.Text = "生成新游戏";
            // 
            // txtJsonText
            // 
            this.txtJsonText.Location = new System.Drawing.Point(15, 20);
            this.txtJsonText.Multiline = true;
            this.txtJsonText.Name = "txtJsonText";
            this.txtJsonText.Size = new System.Drawing.Size(412, 287);
            this.txtJsonText.TabIndex = 51;
            this.txtJsonText.Text = resources.GetString("txtJsonText.Text");
            // 
            // btnUpload
            // 
            this.btnUpload.Location = new System.Drawing.Point(101, 425);
            this.btnUpload.Name = "btnUpload";
            this.btnUpload.Size = new System.Drawing.Size(75, 23);
            this.btnUpload.TabIndex = 50;
            this.btnUpload.Text = "上传";
            this.btnUpload.UseVisualStyleBackColor = true;
            this.btnUpload.Click += new System.EventHandler(this.btnUpload_Click);
            // 
            // txtFishH
            // 
            this.txtFishH.Location = new System.Drawing.Point(78, 389);
            this.txtFishH.Name = "txtFishH";
            this.txtFishH.Size = new System.Drawing.Size(139, 21);
            this.txtFishH.TabIndex = 49;
            this.txtFishH.Click += new System.EventHandler(this.txtFishH_Click);
            // 
            // label7
            // 
            this.label7.AutoSize = true;
            this.label7.Location = new System.Drawing.Point(13, 394);
            this.label7.Name = "label7";
            this.label7.Size = new System.Drawing.Size(35, 12);
            this.label7.TabIndex = 48;
            this.label7.Text = "FishH";
            // 
            // txtFish0
            // 
            this.txtFish0.Location = new System.Drawing.Point(77, 354);
            this.txtFish0.Name = "txtFish0";
            this.txtFish0.Size = new System.Drawing.Size(139, 21);
            this.txtFish0.TabIndex = 47;
            this.txtFish0.Click += new System.EventHandler(this.txtFish0_Click);
            // 
            // label6
            // 
            this.label6.AutoSize = true;
            this.label6.Location = new System.Drawing.Point(12, 359);
            this.label6.Name = "label6";
            this.label6.Size = new System.Drawing.Size(35, 12);
            this.label6.TabIndex = 46;
            this.label6.Text = "Fish0";
            // 
            // cbbGenerateCodeEnum
            // 
            this.cbbGenerateCodeEnum.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.cbbGenerateCodeEnum.FormattingEnabled = true;
            this.cbbGenerateCodeEnum.Items.AddRange(new object[] {
            "em_正版千里马",
            "em_盗版千里马",
            "em_正版海龙龟",
            "em_正版聚宝盆",
            "em_正版鳄鱼王",
            "em_明牌鱼招财猫",
            "em_幽灵潜艇",
            "em_常山赵子龙",
            "em_聚宝盆20180525",
            "em_聚宝盆盗版3",
            "em_恭喜发财之悟空闹海20180603",
            "em_皇家龙舟20180604",
            "em_金猴20180604",
            "em_金鸡下蛋之三色金龙鱼20180604",
            "em_牛霸江湖20180604",
            "em_万马奔腾新20180604",
            "em_月光宝盒20180604",
            "em_战狼20180604",
            "em_宝莲灯钻石美人鱼20180605",
            "em_Currency_UnExist_RandCode",
            "em_Currency_Exist_RandCode"});
            this.cbbGenerateCodeEnum.Location = new System.Drawing.Point(77, 321);
            this.cbbGenerateCodeEnum.Name = "cbbGenerateCodeEnum";
            this.cbbGenerateCodeEnum.Size = new System.Drawing.Size(139, 20);
            this.cbbGenerateCodeEnum.TabIndex = 45;
            // 
            // label5
            // 
            this.label5.AutoSize = true;
            this.label5.Location = new System.Drawing.Point(12, 327);
            this.label5.Name = "label5";
            this.label5.Size = new System.Drawing.Size(59, 12);
            this.label5.TabIndex = 44;
            this.label5.Text = "打码Enum:";
            // 
            // groupBox3
            // 
            this.groupBox3.Controls.Add(this.btnDecodeFile);
            this.groupBox3.Location = new System.Drawing.Point(25, 207);
            this.groupBox3.Name = "groupBox3";
            this.groupBox3.Size = new System.Drawing.Size(253, 121);
            this.groupBox3.TabIndex = 37;
            this.groupBox3.TabStop = false;
            this.groupBox3.Text = "解密日志工具";
            // 
            // btnDecodeFile
            // 
            this.btnDecodeFile.Location = new System.Drawing.Point(76, 47);
            this.btnDecodeFile.Name = "btnDecodeFile";
            this.btnDecodeFile.Size = new System.Drawing.Size(75, 23);
            this.btnDecodeFile.TabIndex = 0;
            this.btnDecodeFile.Text = "选择文件";
            this.btnDecodeFile.UseVisualStyleBackColor = true;
            this.btnDecodeFile.Click += new System.EventHandler(this.btnDecodeFile_Click);
            // 
            // ToolsForm
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 12F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(748, 487);
            this.Controls.Add(this.groupBox3);
            this.Controls.Add(this.gbCreateNewGame);
            this.Controls.Add(this.groupBox1);
            this.Name = "ToolsForm";
            this.Text = "ToolsForm";
            this.Load += new System.EventHandler(this.ToolsForm_Load);
            this.groupBox1.ResumeLayout(false);
            this.groupBox1.PerformLayout();
            this.gbCreateNewGame.ResumeLayout(false);
            this.gbCreateNewGame.PerformLayout();
            this.groupBox3.ResumeLayout(false);
            this.ResumeLayout(false);

        }

        #endregion
        private System.Windows.Forms.GroupBox groupBox1;
        private System.Windows.Forms.ComboBox ccbFishType;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.ComboBox ccbFishName;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.Label label10;
        private System.Windows.Forms.TextBox txtEncryptSerialNumber;
        private System.Windows.Forms.Button btnGenerateChildCodeTools;
        private System.Windows.Forms.GroupBox gbCreateNewGame;
        private System.Windows.Forms.Label label5;
        private System.Windows.Forms.ComboBox cbbGenerateCodeEnum;
        private System.Windows.Forms.Label label6;
        private System.Windows.Forms.TextBox txtFish0;
        private System.Windows.Forms.TextBox txtFishH;
        private System.Windows.Forms.Label label7;
        private System.Windows.Forms.Button btnUpload;
        private System.Windows.Forms.TextBox txtJsonText;
        private System.Windows.Forms.GroupBox groupBox3;
        private System.Windows.Forms.Button btnDecodeFile;
    }
}