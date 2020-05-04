using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace SingleGenerateCodeForm
{
    public partial class Form1 : Form
    {
        public Form1()
        {
            InitializeComponent();
        }


        private String IdentifyText = String.Empty;
        private bool IsShowKillScore = false;
        private void Form1_Load(object sender, EventArgs e)
        {
            SetWindowEnable(false);

            Task.Factory.StartNew(() => 
            {
                try
                {
                    if (Code.API.Initialize() == 0)
                    {
                        MessageBox.Show("连接服务器失败!");
                        return;
                    }
                    SafeInoke(() =>
                    {
                        lbFishType.Text = ReadConfig("Type");
                        lbFishName.Text = ReadConfig("Name");
                        lbDiskID.Text = ReadConfig("EncryptDiskSerialNumber");
                        IdentifyText = ReadConfig("IdentifyText");
                        lbRandCode.Visible = txtRandCode.Visible = ReadConfig("IsExistRandCode") == "1";
                        IsShowKillScore = ReadConfig("IsShowKillScore") == "1";
                        SetWindowEnable(true);

                        if (IsShowKillScore)
                        {
                            ccbWinScoreType.Items.Clear();
                            ccbWinScoreType.Items.AddRange(new string[] { "杀分码", "1000", "2000", "3000", "5000", "8000", "10000", "15000", "20000", "100000" });
                        }

                        //ccbLoseScoreType.SelectedIndex = ccbWinScoreType.SelectedIndex = -1;
                    });
                }
                catch (Exception ex)
                {
                    MessageBox.Show("Form1_Load:" + ex.Message);
                }
            });
        }

        private String ReadConfig(String KeyName)
        {
            StringBuilder Text = new StringBuilder(1024);
            Code.API.GetPrivateProfileString("Config", KeyName, "0", Text, 1024, Path.Combine(System.Environment.CurrentDirectory, @"Config.ini"));
            return Text.ToString();
        }

        private void SetWindowEnable(bool IsShow)
        {
            foreach (Control itm in this.Controls)
            {
                itm.Enabled = IsShow;
            }
        }

        public void SafeInoke(Action Ptr)
        {
            this.Invoke(new MethodInvoker(delegate ()
            {
                Ptr();
            }));
        }

        private void btnGenerateCode_Click(object sender, EventArgs e)
        {
            if (ccbWinScoreType.SelectedIndex == -1 || ccbLoseScoreType.SelectedIndex == -1)
            {
                MessageBox.Show("不用选赢分和输分吗?");
                return;
            }


            try
            {
                // ID,WinScore,LoseScore,Count
                btnGenerateCode.Enabled = false;
                String Text = String.Format("{0},{1},{2},{3}", IdentifyText, ccbWinScoreType.Items[ccbWinScoreType.SelectedIndex].ToString(), ccbLoseScoreType.Items[ccbLoseScoreType.SelectedIndex].ToString(), txtCodeIndex.Text);
                Task.Factory.StartNew(() =>
                {
                    StringBuilder CodeText = new StringBuilder(1024);
                    if (Code.API.GenerateCode(Text, CodeText) != 1)
                    {
                        MessageBox.Show("生成失败");
                    }
                    else
                    {
                        var Vec = CodeText.ToString().Split(new String[] { "," }, StringSplitOptions.RemoveEmptyEntries).ToList();
                        txtQueryCode.Text = Vec[1];
                        txtResult.Text = Vec[0];
                    }


                    btnGenerateCode.Invoke(new Action(() => { btnGenerateCode.Enabled = true; }));
                });
            }
            catch (Exception ex)
            {
                MessageBox.Show("btnGenerateCode_Click:" + ex.Message);
            }
        }

        private void Form1_FormClosing(object sender, FormClosingEventArgs e)
        {
            IntPtr Self = new IntPtr(-1);
            Code.API.TerminateProcess(Self, 0);
        }

        private void ccbWinScoreType_SelectedIndexChanged(object sender, EventArgs e)
        {
            if (ccbWinScoreType.SelectedIndex != -1)
            {
                ccbLoseScoreType.Items.Clear();
                if (ccbWinScoreType.Items[ccbWinScoreType.SelectedIndex].ToString() == "杀分码")
                    ccbLoseScoreType.Items.AddRange(new string[] { "15%杀分码", "25%杀分码", "35%杀分码", "45%杀分码", "55%杀分码" });
                else
                    ccbLoseScoreType.Items.AddRange(new string[] { "0", "1000", "3000", "5000", "10000" });

            }
        }
    }
}
