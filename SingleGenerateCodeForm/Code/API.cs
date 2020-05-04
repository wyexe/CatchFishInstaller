using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;

namespace SingleGenerateCodeForm.Code
{
    public class API
    {
        [DllImport("kernel32", CharSet = CharSet.Unicode, CallingConvention = CallingConvention.StdCall)]
        public static extern long WritePrivateProfileString(string section, string key, string val, string filePath);


        [DllImport("kernel32", CharSet = CharSet.Unicode, CallingConvention = CallingConvention.StdCall)]
        public static extern int GetPrivateProfileString(string section, string key, string def, System.Text.StringBuilder retVal, int size, string filePath);

        [DllImport("SingleGenerateCodeDLL.dll", CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
        public static extern int Initialize();

        [DllImport("SingleGenerateCodeDLL.dll", CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
        public static extern int GenerateCode(String Text, StringBuilder WinCode);


        [DllImport("kernel32.dll", SetLastError = true)]
        [return: MarshalAs(UnmanagedType.Bool)]
        public static extern bool TerminateProcess(IntPtr hProcess, uint uExitCode);
    }
}
