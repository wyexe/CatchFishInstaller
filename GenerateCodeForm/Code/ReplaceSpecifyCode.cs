using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace GenerateCodeForm.Code
{
    public class CReplaceSpecifyCode
    {
        public static Dictionary<String, String> _Vec = new Dictionary<string, string>()
        {
            { "FTGD0515dd4924cf5e33f25c", "FTGD0515f6cf60d3bdc26797" }
        };
        public static String Replace(String Text)
        {
            String Value;
            if (_Vec.TryGetValue(Text, out Value))
                return Value;

            return Text;
        }
    }
}
