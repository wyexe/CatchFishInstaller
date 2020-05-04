using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Media;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace FishMail
{
    public partial class Form1 : Form
    {
        public Form1()
        {
            InitializeComponent();
        }

        public const int WM_COPYDATA = 0x4A;
        public Queue<String> _Queue = new Queue<string>();
        public struct COPYDATASTRUCT
        {
            public IntPtr dwData;
            public int cbData;
            [MarshalAs(UnmanagedType.LPWStr)]
            public string lpData;
        }

        [System.Security.Permissions.PermissionSet(System.Security.Permissions.SecurityAction.Demand, Name = "FullTrust")]
        protected override void WndProc(ref Message m)
        {
            if (m.Msg == WM_COPYDATA)
            {
                COPYDATASTRUCT CopyStruct = new COPYDATASTRUCT();
                CopyStruct = (COPYDATASTRUCT)((Message)m).GetLParam(CopyStruct.GetType());
                if (CopyStruct.dwData.ToInt32() == 0x4C6F68)
                {
                    Task.Factory.StartNew(() => { lock (_Queue) { _Queue.Enqueue(CopyStruct.lpData); } });
                }
            }

            base.WndProc(ref m);
        }

        private void Form1_Load(object sender, EventArgs e)
        {
            Task.Factory.StartNew(MailWorker);
        }

        private void MailWorker()
        {
            string Text;
            while (true)
            {
                Thread.Sleep(2000);
                lock (_Queue)
                {
                    if (_Queue.Count == 0)
                    {
                        continue;
                    }

                    Text = _Queue.Dequeue();
                }


                var Mail = new EMail
                {
                    host = "smtp.qq.com",
                    isbodyHtml = false,
                    mailFrom = "12345@qq.com",
                    mailPwd = "123456",
                    mailSubject = "Fish",
                    mailBody = Text,
                    mailToArray = new string[] {"123@qq.com"},
                    mailCcArray = new string[] {"123@hotmail.com", "123@qq.com" }
                };

                String ErrText = String.Empty;
                if (!Mail.Send(ref ErrText))
                {
                    listBox1.Invoke(new Action(() => { listBox1.Items.Add(ErrText); }));
                    //MessageBox.Show("Faild!");
                }
                else
                {
                    listBox1.Invoke(new Action(() => { listBox1.Items.Add("发送成功!"); }));
                }
            }
            // qq:   pfnjwcuuedwbeabg      pcssyblrfdczdjbh
           
        }
    }
}
