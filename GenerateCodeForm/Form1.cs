using GenerateCodeForm.Code;
using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Text.RegularExpressions;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace GenerateCodeForm
{
    public partial class Form1 : Form
    {
        public Form1()
        {
            InitializeComponent();
        }

        public List<Code.CatchFishTypeContent> _VecFishType = new List<CatchFishTypeContent>();
        private List<String> _VecCodeType = new List<string>();
        private bool _IsSelfComputer = false;
        private void Form1_Load(object sender, EventArgs e)
        {
            cbbIsHideCode.SelectedIndex = 0;
            SetWindowVisiable(false, lbConnectStatus.Name);

            _IsSelfComputer = System.Environment.MachineName == "DESKTOP-KDUSIGB" || System.Environment.MachineName == "DESKTOP-F1HGJ85";
            Task.Factory.StartNew(() =>
            {
                try
                {
                    // Initialize to Connect Server
                    if (Code.API.Initialize(GetConnectString()) == 0)
                    {
                        MessageBox.Show("连接服务器失败!");
                        Application.Exit();
                        return;
                    }


                    // Get Vec Fish Type
                    StringBuilder sbText = new StringBuilder(1024 * 10/*10KB*/);
                    Code.API.GetVecFishInfo(sbText);
                    _VecFishType = Code.CatchFishTypeContent.ConvertTextToList(sbText.ToString());


                    // Get Vec Code Type
                    sbText.Clear();
                    Code.API.GetVecCodeType(sbText);
                    sbText.ToString().Trim().Split(new String[] { "\r\n" }, StringSplitOptions.RemoveEmptyEntries).ToList().ForEach(itm => _VecCodeType.Add(itm));


                    Code.CCommon.SafeInoke(this, () =>
                    {
                        InitializeControls();
                    });
                }
                catch (Exception ex)
                {
                    MessageBox.Show("Form1_Load:" + ex.Message);
                }
               
            });
        }

        private String GetConnectString()
        {
            if (!_IsSelfComputer)
            {
                return "L";
            }


            return MessageBox.Show("是否进入测试服务器?", "Tip", MessageBoxButtons.YesNo) == DialogResult.Yes ? "H" : "L";
        }

        private void SetWindowVisiable(bool IsShow, String ExceptName)
        {
            foreach (Control itm in this.Controls)
            {
                if (itm.Name != ExceptName)
                {
                    itm.Visible = IsShow;
                }
            }
        }




        #region InitializeControls
        private void InitializeControls()
        {
            // Show Controls
            SetWindowVisiable(false, "");
            SetWindowVisiable(true, lbConnectStatus.Name);


            // Initialize FishType
            InitializeComboBoxFishType();
            InitializeComboBoxCodeType();
            ccbWinScoreType.SelectedIndex = ccbLoseScoreType.SelectedIndex = 0;
            txtMutipCode.Visible = false;
        }


        private void InitializeComboBoxFishType()
        {
            ccbFishType.Items.Clear();
            _VecFishType.ForEach(itm => ccbFishType.Items.Add(itm.CatchFishTypeName));
        }

        private void InitializeComboBoxFishName(String FishTypeName, int Index = -1)
        {
            ccbFishName.Items.Clear();
            foreach (var item in _VecFishType.Where(itm => itm.CatchFishTypeName == FishTypeName))
            {
                item.VecFishName.ForEach(itm => ccbFishName.Items.Add(itm.CatchFishName));
            }
        }

        private void InitializeComboBoxCodeType()
        {
            ccbCodeType.Items.Clear();
            _VecCodeType.ForEach(itm => ccbCodeType.Items.Add(itm));
            ccbCodeType.SelectedIndex = 0;
            lbCodeType.Text = ccbCodeType.Items[0].ToString();
        }
        #endregion

        #region ComboBoxSelectedIndexChanged
        public void ComboBoxSelectedIndexChanged(ComboBox itm, String ItemName)
        {
            itm.SelectedIndex = itm.Items.IndexOf(ItemName);
        }
        #endregion

        private void ccbFishType_SelectedIndexChanged(object sender, EventArgs e)
        {
            if (ccbFishType.SelectedIndex != -1)
            {
                InitializeComboBoxFishName(ccbFishType.Items[ccbFishType.SelectedIndex].ToString());
            }
        }

        private void ccbFishName_SelectedIndexChanged(object sender, EventArgs e)
        {
            if (ccbFishName.SelectedIndex != -1)
            {
                String SelectedItemText = ccbFishName.Items[ccbFishName.SelectedIndex].ToString();
                txtRandCode.Visible = _VecFishType.Exists(itm => 
                {
                    return itm.VecFishName.Exists(Content => Content.CatchFishName == SelectedItemText && Content.IsExistRandCode );
                });
            }
        }

        private void txtOriginDiskSerialNumber_TextChanged(object sender, EventArgs e)
        {
            
        }

        private void txtEncryptSerialNumber_TextChanged(object sender, EventArgs e)
        {
            String Text = txtEncryptSerialNumber.Text.Trim();
            if (Text.Length >= 8)
            {
                String FishSerialNumberFlagText = Text.Substring(0, 4);
                if (FishSerialNumberFlagText == "FTGC" || FishSerialNumberFlagText == "FTGD" || FishSerialNumberFlagText == "==VV")
                {
                    String FishSerialNumberText = Text.Substring(4, 4);
                    Text = Text.Substring(8);
                    foreach (var FishTypeContent in _VecFishType)
                    {
                        foreach (var FishNameContent in FishTypeContent.VecFishName)
                        {
                            if (FishNameContent.IdentifierSerialNumber == FishSerialNumberText)
                            {
                                ComboBoxSelectedIndexChanged(ccbFishType, FishTypeContent.CatchFishTypeName);
                                InitializeComboBoxFishName(FishTypeContent.CatchFishTypeName);
                                ComboBoxSelectedIndexChanged(ccbFishName, FishNameContent.CatchFishName);
                                return;
                            }
                        }
                    }
                }
            }

        }

        private bool IsValidSerialNumberText(String Text)
        {
            //return new Regex(@"^[A-Za-z0-9]+$", RegexOptions.IgnoreCase).IsMatch(Text);
            return true;
        }

        private bool IsVaildControlParam()
        {
            if (ccbFishName.SelectedIndex == -1 || ccbFishType.SelectedIndex == -1)
            {
                MessageBox.Show("系列和名字必须选的...");
                return false;
            }
            else if (ccbCodeType.SelectedIndex == -1)
            {
                MessageBox.Show("码分类是必须要选的...");
                return false;
            }
            else if (ccbLoseScoreType.SelectedIndex == -1 || ccbWinScoreType.SelectedIndex  == -1)
            {
                MessageBox.Show("输分和赢分需要选一个!");
                return false;
            }


            if (txtCodeIndex.Text.Trim() == "")
            {
                MessageBox.Show("次数呢?");
                return false;
            }
            if (txtOriginDiskSerialNumber.Text.Trim() == "" && txtEncryptSerialNumber.Text.Trim() == "")
            {
                MessageBox.Show("加密ID 或者 未加密ID需要填一个!");
                return false;
            }
            if (txtOriginDiskSerialNumber.Text.Trim() != "" && txtEncryptSerialNumber.Text.Trim() != "")
            {
                MessageBox.Show("加密ID 或者 未加密ID只能填一个!");
                return false;
            }
            else if (txtOriginDiskSerialNumber.Text.Trim() != "" && !IsValidSerialNumberText(txtOriginDiskSerialNumber.Text.Trim()))
            {
                MessageBox.Show("未加密ID 不能瞎填呐!");
                return false;
            }
            else if (txtEncryptSerialNumber.Text.Trim() != "" && !IsValidSerialNumberText(txtEncryptSerialNumber.Text.Trim()))
            {
                MessageBox.Show("加密ID 不能瞎填呐!");
                return false;
            }
            if (txtRandCode.Visible && txtRandCode.Text.Trim() == "")
            {
                MessageBox.Show("随机码忘记填了?");
                return false;
            }


            return true;
        }

        private void btnGenerateCode_Click(object sender, EventArgs e)
        {
            try
            {
                if (!IsVaildControlParam())
                    return;

                btnGenerateCode.Enabled = false;

                lbCodeType.Text = ccbCodeType.Items[ccbCodeType.SelectedIndex].ToString();
                var EncryptDiskSerialNumberText = CReplaceSpecifyCode.Replace(txtEncryptSerialNumber.Text.Trim());
                if (EncryptDiskSerialNumberText.IndexOf("FTGC", StringComparison.Ordinal) != -1 && EncryptDiskSerialNumberText.Length > 8)
                {
                    EncryptDiskSerialNumberText = EncryptDiskSerialNumberText.Substring(8);
                    txtMutipCode.Visible = true;
                    txtMutipCode.Text = EncryptDiskSerialNumberText;
                }
                if (EncryptDiskSerialNumberText.IndexOf("FTGD", StringComparison.Ordinal) != -1 && EncryptDiskSerialNumberText.Length > 8)
                {
                    var tmpText = String.Empty;
                    EncryptDiskSerialNumberText = EncryptDiskSerialNumberText.Substring(8);

                    for (var i = 0; i < EncryptDiskSerialNumberText.ToList().Count; i += 2)
                    {
                        var HexNumber = String.Empty;
                        HexNumber += EncryptDiskSerialNumberText[i];
                        HexNumber += EncryptDiskSerialNumberText[i + 1];

                        var dwValue = int.Parse(HexNumber, System.Globalization.NumberStyles.HexNumber) ^ 0xFF;
                        tmpText += dwValue.ToString("X2");
                    }

                    EncryptDiskSerialNumberText = tmpText.ToLower();
                    txtMutipCode.Visible = true;
                    txtMutipCode.Text = EncryptDiskSerialNumberText;
                }
                else if (EncryptDiskSerialNumberText.IndexOf("==VV", StringComparison.Ordinal) != -1 && EncryptDiskSerialNumberText.Length > 12)
                {
                    String tmpText = String.Empty;
                    EncryptDiskSerialNumberText.Substring(12).ToList().ForEach(itm =>
                    {
                        if (itm == 'a')
                            tmpText += 'z';
                        else if (itm == 'z')
                            tmpText += 'a';
                        else
                            tmpText += (char)(itm - 1);
                    });
                    EncryptDiskSerialNumberText = tmpText;
                    txtMutipCode.Visible = true;
                    txtMutipCode.Text = EncryptDiskSerialNumberText;
                }


                Code.API.SetGenerateCodeParam("FishType", ccbFishType.Items[ccbFishType.SelectedIndex].ToString());
                Code.API.SetGenerateCodeParam("FishName", ccbFishName.Items[ccbFishName.SelectedIndex].ToString());
                Code.API.SetGenerateCodeParam("CodeType", ccbCodeType.Items[ccbCodeType.SelectedIndex].ToString());
                Code.API.SetGenerateCodeParam("OriginDiskSerialNumber", txtOriginDiskSerialNumber.Text.Trim());
                Code.API.SetGenerateCodeParam("EncryptDiskSerialNumber", EncryptDiskSerialNumberText);
                Code.API.SetGenerateCodeParam("WinScore", ccbWinScoreType.Items[ccbWinScoreType.SelectedIndex].ToString());
                Code.API.SetGenerateCodeParam("LoseScore", ccbLoseScoreType.Items[ccbLoseScoreType.SelectedIndex].ToString());
                Code.API.SetGenerateCodeParam("CodeIndex", txtCodeIndex.Text.ToString());
                Code.API.SetGenerateCodeParam("RandCode", "-1");
                Code.API.SetGenerateCodeParam("IsHide", cbbIsHideCode.SelectedIndex != 0 ? "1" : "0");


                Task.Factory.StartNew(() =>
                {
                    StringBuilder sbText = new StringBuilder(1024);
                    if (Code.API.GenerateCode(sbText) == 1)
                    {
                        var VecText = sbText.ToString().Trim().Split(new String[] { "\r\n" }, StringSplitOptions.RemoveEmptyEntries).ToList();
                        if (VecText.Count >= 2)
                        {
                           // MessageBox.Show(VecText[0]);
                            Code.CCommon.SafeInoke(this, () =>
                            {
                                txtQueryCode.Text = VecText[0];
                                txtResult.Text = VecText[1];
                            });
                        }
                        else
                            MessageBox.Show("无效的格式:" + sbText.ToString());
                    }
                    else
                        MessageBox.Show("生成失败!");

                    Code.CCommon.SafeInoke(this, () => { btnGenerateCode.Enabled = true; });
                });
            }
            catch (Exception ex)
            {
                MessageBox.Show("btnGenerateCode_Click:" + ex.Message);
            }
        }

        private void Form1_FormClosing(object sender, FormClosingEventArgs e)
        {
            IntPtr Ptr = new IntPtr(-1);
            Code.API.TerminateProcess(Ptr, 0);
        }


        private void Form1_KeyDown(object sender, KeyEventArgs e)
        {
            if (e.KeyCode == Keys.Enter)
            {
                btnGenerateCode_Click(sender, e);
            }
        }

        private void ccbCodeType_SelectedIndexChanged(object sender, EventArgs e)
        {
            if (ccbCodeType.SelectedIndex != -1)
            {
                lbCodeType.Text = ccbCodeType.Items[ccbCodeType.SelectedIndex].ToString();
                if (lbCodeType.Text == "杀分码")
                {
                    ccbWinScoreType.Enabled = false;
                    ccbWinScoreType.SelectedIndex = 0;
                    ccbLoseScoreType.Items.Clear();
                    ccbLoseScoreType.Items.AddRange(new string[] { "10%杀分码", "20%杀分码", "30%杀分码", "40%杀分码", "50%杀分码" });
                    ccbLoseScoreType.SelectedIndex = 0;
                }
                else
                {
                    ccbWinScoreType.Enabled = true;
                    ccbWinScoreType.SelectedIndex = -1;
                    ccbLoseScoreType.Items.Clear();
                    ccbLoseScoreType.Items.AddRange(new string[] { "0" ,"1000", "3000", "5000", "10000" });
                }
            }
        }

        private void btnShowToolsForm_Click(object sender, EventArgs e)
        {
            new ToolsForm(this).ShowDialog();
        }

        private String FastEncryptSerialNumber()
        {
            StringBuilder EncryptDiskSerialNumber = new StringBuilder(1024);
            StringBuilder sbText = new StringBuilder(1024);
            if (Code.API.GenerateTestCode(ccbFishType.Items[ccbFishType.SelectedIndex].ToString(), ccbFishName.Items[ccbFishName.SelectedIndex].ToString(), txtOriginDiskSerialNumber.Text.Trim(), EncryptDiskSerialNumber, sbText) != 1)
            {
                MessageBox.Show("GenerateTestCode = FALSE!");
                return "";
            }



            var Arr = sbText.ToString().Split(new String[] { "," }, StringSplitOptions.RemoveEmptyEntries);
            if (Arr.Length != 2)
            {
                MessageBox.Show("Invalid  Code = " + sbText.ToString());
                return "";
            }


            txtQueryCode.Text = Arr[0];
            txtResult.Text = Arr[1];
            return EncryptDiskSerialNumber.ToString();
        }


        private String GetIdentifierSerailNumber(String FishType, String FishName)
        {
            foreach (var FishTypeContent in _VecFishType)
            {
                if (FishTypeContent.CatchFishTypeName != FishType)
                    continue;


                foreach (var FishNameContent in FishTypeContent.VecFishName)
                {
                    if (FishNameContent.CatchFishName == FishName)
                    {
                        return FishNameContent.IdentifierSerialNumber;
                    }
                }
            }

            return "";
        }

        private void btnGenerateTestCode_Click(object sender, EventArgs e)
        {
            if (ccbFishName.SelectedIndex == -1 || ccbFishType.SelectedIndex == -1)
            {
                MessageBox.Show("系列和名字必须选的...");
                return;
            }
            else if (txtOriginDiskSerialNumber.Text.Trim() == "")
            {
                MessageBox.Show("原始ID呢?...");
                return;
            }


            String IdentifierSerialNumber = GetIdentifierSerailNumber(ccbFishType.Items[ccbFishType.SelectedIndex].ToString(), ccbFishName.Items[ccbFishName.SelectedIndex].ToString());
            String Text = FastEncryptSerialNumber();
            txtEncryptSerialNumber.Text = Text == "" ? "" : String.Format("FTGC{0}{1}", IdentifierSerialNumber, Text);
        }

        private void btnNewGenerateTestCode_Click(object sender, EventArgs e)
        {
            if (ccbFishName.SelectedIndex == -1 || ccbFishType.SelectedIndex == -1)
            {
                MessageBox.Show("系列和名字必须选的...");
                return;
            }
            else if (txtOriginDiskSerialNumber.Text.Trim() == "")
            {
                MessageBox.Show("原始ID呢?...");
                return;
            }


            String IdentifierSerialNumber = GetIdentifierSerailNumber(ccbFishType.Items[ccbFishType.SelectedIndex].ToString(), ccbFishName.Items[ccbFishName.SelectedIndex].ToString());


            String EncryptDiskSerialNumberText = "";
            FastEncryptSerialNumber().ToList().ForEach(itm =>
            {
                if (itm == 'a')
                    EncryptDiskSerialNumberText += 'z';
                else if (itm == 'z')
                    EncryptDiskSerialNumberText += 'a';
                else
                    EncryptDiskSerialNumberText += (char)(itm + 1);
            });

            txtEncryptSerialNumber.Text = EncryptDiskSerialNumberText == "" ? "" : String.Format("==VV{0}VV=={1}", IdentifierSerialNumber, EncryptDiskSerialNumberText);
        }
    }
}
