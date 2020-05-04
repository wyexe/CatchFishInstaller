using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;

namespace GenerateCodeForm.Code
{
    public class API
    {
        [DllImport("GenerateCodeDLL.dll", CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
        public static extern int Initialize([In] String Text);


        [DllImport("GenerateCodeDLL.dll", CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
        public static extern void GetVecFishInfo([Out] StringBuilder sbText);


        [DllImport("GenerateCodeDLL.dll", CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
        public static extern void GetVecCodeType([Out] StringBuilder sbText);


        [DllImport("GenerateCodeDLL.dll", CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
        public static extern void SetGenerateCodeParam([In] String KeyName, [In] String KeyValue);


        [DllImport("GenerateCodeDLL.dll", CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
        public static extern int GenerateCode(StringBuilder sbText);


        [DllImport("GenerateCodeDLL.dll", CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
        public static extern int GenerateTestCode([In] String GameType, [In] String GameName, [In] String wsOriginDiskSerailNumber, [Out] StringBuilder EncryptDiskSerialNumber, [Out] StringBuilder CodeText);


        [DllImport("GenerateCodeDLL.dll", CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
        public static extern int CreateChildGenerateCode([In] String EncryDiskSerialNumber, [In] String FishTypeText, [In] String FishNameText, [Out] StringBuilder sbText);

        [DllImport("kernel32.dll", SetLastError = true)]
        [return: MarshalAs(UnmanagedType.Bool)]
        public static extern bool TerminateProcess(IntPtr hProcess, uint uExitCode);


        [DllImport("kernel32", CharSet = CharSet.Unicode, CallingConvention = CallingConvention.StdCall)]
        public static extern long WritePrivateProfileString(string section, string key, string val, string filePath);


        [DllImport("kernel32", CharSet = CharSet.Unicode, CallingConvention = CallingConvention.StdCall)]
        public static extern int GetPrivateProfileString(string section, string key, string def, System.Text.StringBuilder retVal, int size, string filePath);

        [DllImport("GenerateCodeDLL.dll", CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
        public static extern int CreateNewFishGame([In] String Fish0Path, [In] String FishHPath, [In] int EnumValue);


        [DllImport("GenerateCodeDLL.dll", CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
        public static extern void SetCreateNewFishGameParam([In] String KeyName, [In] String KeyValue);

        [DllImport("GenerateCodeDLL.dll", CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
        public static extern int DecodeLogFile([In] String FilePath);
    }
}
