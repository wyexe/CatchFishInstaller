using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace GenerateCodeForm.Code
{
    public class ConfigWriter
    {
        public static void CreateSingleGenerateCode(String EncryptDiskSerialNumberText, String FishTypeText, String FishNameText, String IdentifyText, bool IsExistRandCode)
        {
            try
            {
                if (!RemoveResFile())
                    return;


                CreateResFile("SingleGenerateCodeDLL.dll", global::GenerateCodeForm.Resource.SingleGenerateCodeDLL);
                CreateResFile("SingleGenerateCodeForm.exe", global::GenerateCodeForm.Resource.SingleGenerateCodeForm);
                CreateConfigFile(EncryptDiskSerialNumberText, FishTypeText, FishNameText, IdentifyText, IsExistRandCode);
            }
            catch (Exception ex)
            {
                MessageBox.Show("CreateSingleGenerateCode:" + ex.Message);
            }
        }


        private static void CreateResFile(String FileName, byte[] Buffer)
        {
            try
            {
                FileStream FsSingleGenerateCodeDLL = new FileStream(Path.Combine(System.Environment.CurrentDirectory, Path.Combine(Path.Combine(System.Environment.CurrentDirectory, "Release"), FileName)), FileMode.CreateNew);
                FsSingleGenerateCodeDLL.Write(Buffer, 0, Buffer.Length);
                FsSingleGenerateCodeDLL.Close();
            }
            catch (Exception ex)
            {
                MessageBox.Show("创建文件失败:" + ex.Message);
            }
        }

        private static void CreateConfigFile(String EncryptDiskSerialNumberText, String FishTypeText, String FishNameText, String IdentifyText, bool IsExistRandCode)
        {
            String ConfigPath = Path.Combine(System.Environment.CurrentDirectory, Path.Combine(Path.Combine(System.Environment.CurrentDirectory, "Release"), "Config.ini"));
            FileStream FsSingleGenerateCodeDLL = new FileStream(ConfigPath, FileMode.OpenOrCreate);
            FsSingleGenerateCodeDLL.Close();


            Code.API.WritePrivateProfileString("Config", "Type", FishTypeText, ConfigPath);
            Code.API.WritePrivateProfileString("Config", "Name", FishNameText, ConfigPath);
            Code.API.WritePrivateProfileString("Config", "EncryptDiskSerialNumber", EncryptDiskSerialNumberText, ConfigPath);
            Code.API.WritePrivateProfileString("Config", "IdentifyText", IdentifyText, ConfigPath);
            Code.API.WritePrivateProfileString("Config", "IsExistRandCode", IsExistRandCode ? "1" : "0", ConfigPath);
        }


        private static bool RemoveResFile()
        {
            try
            {
                if (!Directory.Exists(Path.Combine(System.Environment.CurrentDirectory, "Release")))
                {
                    Directory.CreateDirectory(Path.Combine(System.Environment.CurrentDirectory, "Release"));
                }

    
                File.Delete(Path.Combine(System.Environment.CurrentDirectory, Path.Combine(System.Environment.CurrentDirectory, Path.Combine(Path.Combine(System.Environment.CurrentDirectory, "Release"), "SingleGenerateCodeDLL.dll"))));
                File.Delete(Path.Combine(System.Environment.CurrentDirectory, Path.Combine(System.Environment.CurrentDirectory, Path.Combine(Path.Combine(System.Environment.CurrentDirectory, "Release"), "SingleGenerateCodeForm.exe"))));
                return true;
            }
            catch (Exception ex)
            {
                MessageBox.Show("删除文件失败:" + ex.Message);
            }
            return false;
        }
    }
}
