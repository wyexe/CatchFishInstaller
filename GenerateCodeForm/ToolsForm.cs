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
using Newtonsoft.Json;

namespace GenerateCodeForm
{
    public partial class ToolsForm : Form
    {
        private Form1 _FatherForm = null;
        public ToolsForm(Form1 FatherForm)
        {
            _FatherForm = FatherForm;
            InitializeComponent();
        }
        
        private void ToolsForm_Load(object sender, EventArgs e)
        {
            InitializeComboBoxFishType();
            //gbCreateNewGame.Visible = Environment.UserName.ToLower().IndexOf("wyexe") != -1;
        }


        private void InitializeComboBoxFishType()
        {
            ccbFishType.Items.Clear();
            _FatherForm._VecFishType.ForEach(itm => ccbFishType.Items.Add(itm.CatchFishTypeName));
        }

        private void InitializeComboBoxFishName(String FishTypeName, int Index = -1)
        {
            ccbFishName.Items.Clear();
            foreach (var item in _FatherForm._VecFishType.Where(itm => itm.CatchFishTypeName == FishTypeName))
            {
                item.VecFishName.ForEach(itm => ccbFishName.Items.Add(itm.CatchFishName));
            }
        }

        private void ccbFishType_SelectedIndexChanged(object sender, EventArgs e)
        {
            if (ccbFishType.SelectedIndex != -1)
            {
                InitializeComboBoxFishName(ccbFishType.Items[ccbFishType.SelectedIndex].ToString());
            }
        }

        private void btnGenerateChildCodeTools_Click(object sender, EventArgs e)
        {
            String EncryptDiskSerialNumberText = txtEncryptSerialNumber.Text.Trim();
            if (EncryptDiskSerialNumberText.IndexOf("FTGC") != -1 && EncryptDiskSerialNumberText.Length > 8)
            {
                EncryptDiskSerialNumberText = EncryptDiskSerialNumberText.Substring(8);
            }


            if (EncryptDiskSerialNumberText.Length == 0)
            {
                MessageBox.Show("加密ID呢?");
                return;
            }
            else if (ccbFishType.SelectedIndex == -1)
            {
                MessageBox.Show("类型呢?");
                return;
            }
            else if (ccbFishName.SelectedIndex == -1)
            {
                MessageBox.Show("名字呢?");
                return;
            }



            String FishTypeText = ccbFishType.Items[ccbFishType.SelectedIndex].ToString();
            String FishNameText = ccbFishName.Items[ccbFishName.SelectedIndex].ToString();
            btnGenerateChildCodeTools.Enabled = false;
            Task.Factory.StartNew(() => 
            {
                StringBuilder sbText = new StringBuilder(1024);
                if (Code.API.CreateChildGenerateCode(EncryptDiskSerialNumberText, FishTypeText, FishNameText, sbText) == 0)
                {
                    MessageBox.Show("生成失败!");
                    Code.CCommon.SafeInoke(this, () => { btnGenerateChildCodeTools.Enabled = true; });
                    return;
                }


                var ResultArray = sbText.ToString().Split(new String[] { "\r\n" }, StringSplitOptions.RemoveEmptyEntries);
                MessageBox.Show("准备生成子打码器!");
                Code.ConfigWriter.CreateSingleGenerateCode(EncryptDiskSerialNumberText, FishTypeText, FishNameText, ResultArray[0], ResultArray[1] == "1");
                MessageBox.Show("生成完毕!");
                Code.CCommon.SafeInoke(this, () => { btnGenerateChildCodeTools.Enabled = true; });
            });
            
        }

        private void txtEncryptSerialNumber_TextChanged(object sender, EventArgs e)
        {
            String Text = txtEncryptSerialNumber.Text.Trim();
            if (Text.Length >= 8)
            {
                String FishSerialNumberFlagText = Text.Substring(0, 4);
                if (FishSerialNumberFlagText == "FTGC")
                {
                    String FishSerialNumberText = Text.Substring(4, 4);
                    Text = Text.Substring(8);
                    foreach (var FishTypeContent in _FatherForm._VecFishType)
                    {
                        foreach (var FishNameContent in FishTypeContent.VecFishName)
                        {
                            if (FishNameContent.IdentifierSerialNumber == FishSerialNumberText)
                            {
                                _FatherForm.ComboBoxSelectedIndexChanged(ccbFishType, FishTypeContent.CatchFishTypeName);
                                InitializeComboBoxFishName(FishTypeContent.CatchFishTypeName);
                                _FatherForm.ComboBoxSelectedIndexChanged(ccbFishName, FishNameContent.CatchFishName);
                                return;
                            }
                        }
                    }
                }
            }
        }


        private int GetEnumValueByccbEnum()
        {
            return cbbGenerateCodeEnum.SelectedIndex + 1;
        }

        private void btnUpload_Click(object sender, EventArgs e)
        {
            int EnumVersion = 0;
            String Fish0Path = String.Empty, FishHPath = String.Empty, JsonText = String.Empty;

            EnumVersion = GetEnumValueByccbEnum();
            if (EnumVersion < 1)
            {
                MessageBox.Show("前3个参数必填");
                return;
            }



            Fish0Path = txtFish0.Text.Trim();
            FishHPath = txtFishH.Text.Trim();
            JsonText = txtJsonText.Text.Trim();
            if (!File.Exists(Fish0Path))
            {
                MessageBox.Show(String.Format("Fish0 的路径[{0}] 不存在!", Fish0Path));
                return;
            }
            if (!File.Exists(FishHPath))
            {
                MessageBox.Show(String.Format("FishH 的路径[{0}] 不存在!", FishHPath));
                return;
            }


            btnUpload.Enabled = false;
            Task.Factory.StartNew(() => 
            {
                var GameIdentifier = JsonConvert.DeserializeObject<Code.CGameIdentifier>(JsonText);
                Code.API.SetCreateNewFishGameParam("Series", GameIdentifier.Series);
                Code.API.SetCreateNewFishGameParam("Name", GameIdentifier.Name);
                Code.API.SetCreateNewFishGameParam("SerialNumber",GameIdentifier.SerialNumber);
                Code.API.SetCreateNewFishGameParam("GameFullPath",GameIdentifier.GameFullPath);
                Code.API.SetCreateNewFishGameParam("GameCRC32", GameIdentifier.GameCRC32);
                Code.API.SetCreateNewFishGameParam("ReleaseDirectory",GameIdentifier.ReleaseDirectory);
                Code.API.SetCreateNewFishGameParam("IsExistRandCode",GameIdentifier.IsExistRandCode.ToString());
                Code.API.SetCreateNewFishGameParam("SpecifyInjectorPath",GameIdentifier.SpecifyInjectorPath);
                Code.API.SetCreateNewFishGameParam("EnumValue",GameIdentifier.EnumValue.ToString());
                GameIdentifier.IdentifierDirectory.ToList().ForEach(itm => { Code.API.SetCreateNewFishGameParam("IdentifierDirectory", itm); });

                if (GameIdentifier.BinaryCode.Count % 2 != 0)
                {
                    MessageBox.Show(String.Format("BinaryCode.Count[{0}] % 2 != 0", GameIdentifier.BinaryCode.Count));
                    return;
                }


                for (int i = 0; i < GameIdentifier.BinaryCode.Count; i+=2)
                {
                    Code.API.SetCreateNewFishGameParam("BinaryCode", String.Format("{0}-{1}", GameIdentifier.BinaryCode[i], GameIdentifier.BinaryCode[i + 1]));
                }
                 if(Code.API.CreateNewFishGame(Fish0Path, FishHPath, EnumVersion) != 1)
                     MessageBox.Show("创建失败!");
                 else
                     MessageBox.Show("创建成功!");


                Code.CCommon.SafeInoke(this, () => { btnUpload.Enabled = true; });
            });
        }

        private void txtFish0_Click(object sender, EventArgs e)
        {
            var dlg = new OpenFileDialog();
            dlg.InitialDirectory = System.Environment.CurrentDirectory;
            dlg.Filter = "*.dll|*.*";
            dlg.Multiselect = false;
            if (dlg.ShowDialog() == DialogResult.OK)
            {
                if (dlg.FileName.ToLower().IndexOf("fish0.dll") == -1)
                {
                    MessageBox.Show("无效的[Fish0.dll]");
                    return;
                }
                txtFish0.Text = dlg.FileName;
            }
        }

        private void txtFishH_Click(object sender, EventArgs e)
        {
            var dlg = new OpenFileDialog();
            dlg.InitialDirectory = System.Environment.CurrentDirectory;
            dlg.Filter = "*.dll|*.*";
            dlg.Multiselect = false;
            if (dlg.ShowDialog() == DialogResult.OK)
            {
                if (dlg.FileName.ToLower().IndexOf("fishh.dll") == -1)
                {
                    MessageBox.Show("无效的[FishH.dll]");
                    return;
                }
                txtFishH.Text = dlg.FileName;
            }
        }

        private void btnDecodeFile_Click(object sender, EventArgs e)
        {
            var dlg = new OpenFileDialog();
            dlg.InitialDirectory = System.Environment.CurrentDirectory;
            dlg.Filter = "*.dat|*.*";
            dlg.Multiselect = false;
            if (dlg.ShowDialog() == DialogResult.OK && Code.API.DecodeLogFile(dlg.FileName) == 1)
            {
                MessageBox.Show("生成'DecodeFile.txt'完毕!");
            }
        }
    }
}
