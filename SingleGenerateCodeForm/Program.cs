using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Security.Principal;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace SingleGenerateCodeForm
{
    static class Program
    {
        /// <summary>
        /// The main entry point for the application.
        /// </summary>
        [STAThread]
        static void Main()
        {
            if (!IsAdministrator())
            {
                MessageBox.Show("必须运行在管理员权限下! 请以右键管理员模式运行!");
                return;
            }
            if (!File.Exists(System.Environment.CurrentDirectory + @"\SingleGenerateCodeDLL.dll"))
            {
                MessageBox.Show("缺少文件[SingleGenerateCodeDLL.dll]!");
                return;
            }
            if (!File.Exists(System.Environment.CurrentDirectory + @"\Config.ini"))
            {
                MessageBox.Show("缺少文件[Config.ini]!");
                return;
            }
            Application.EnableVisualStyles();
            Application.SetCompatibleTextRenderingDefault(false);
            Application.Run(new Form1());
        }

        public static bool IsAdministrator()
        {
            return (new WindowsPrincipal(WindowsIdentity.GetCurrent())).IsInRole(WindowsBuiltInRole.Administrator);
        }
    }
}
