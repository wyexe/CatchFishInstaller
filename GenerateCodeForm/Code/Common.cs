using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace GenerateCodeForm.Code
{
    public struct CatchFishNameContent
    {
        public String  CatchFishName { get; set; }
        public bool IsExistRandCode { get; set; }
        public String  IdentifierSerialNumber { get; set; }
    }


    public class CatchFishTypeContent
    {
        public String                       CatchFishTypeName { get; set; }
        public List<CatchFishNameContent>   VecFishName { get => _VecFishName; set => _VecFishName = value; }

        private List<CatchFishNameContent> _VecFishName = new List<CatchFishNameContent>();
        public static List<CatchFishTypeContent> ConvertTextToList(String Text)
        {
            var Vec = new List<CatchFishTypeContent>();

            // FishType,FishName,1,FisName,1
            Text.Trim().Split(new String[] { "\r\n" }, StringSplitOptions.RemoveEmptyEntries).ToList().ForEach(itm => 
            {
                var VecText = itm.Split(new String[] { "," }, StringSplitOptions.RemoveEmptyEntries).ToList();
                if (VecText.Count < 2)
                {
                    MessageBox.Show("文本解析错误:" + itm);
                    return;
                }


                var Content = new CatchFishTypeContent();
                Content.CatchFishTypeName = VecText[0].Trim();
                for (int i = 1; i < VecText.Count; i += 3)
                {
                    Content.VecFishName.Add(new CatchFishNameContent() { CatchFishName = VecText[i], IsExistRandCode = VecText[i + 1] == "1", IdentifierSerialNumber = VecText[i + 2] });
                }
                Vec.Add(Content);
            });

            return Vec;
        }
    }

    public class CCommon
    {
        public static void SafeInoke(Form fm, Action Ptr)
        {
            fm.Invoke(new MethodInvoker(delegate ()
            {
                Ptr();
            }));
        }
    }
}
