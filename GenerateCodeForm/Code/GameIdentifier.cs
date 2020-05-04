using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace GenerateCodeForm.Code
{
    public class CGameIdentifier
    {
        public String Series { get; set; }
        public String Name { get; set; }
        public String SerialNumber { get; set; }
        public String GameFullPath { get; set; }
        public String GameCRC32 { get; set; }
        public String ReleaseDirectory { get; set; }
        public int IsExistRandCode { get; set; }
        public IList<String> IdentifierDirectory { get; set; }
        public IList<String> BinaryCode { get; set; }
        public String SpecifyInjectorPath { get; set; }
        public int EnumValue { get; set; }
    }
}
