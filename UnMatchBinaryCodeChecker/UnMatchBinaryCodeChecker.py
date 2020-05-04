# -*- coding: utf-8 -*-
import os;

VecDirs = os.walk("E:\\BaiduNetdiskDownload\\捕鱼\\海王传说系列\\");
for root, dirs, files in VecDirs:
    for DirName in dirs:
        FullDir = os.path.join(root, DirName);
        FullFilePath = FullDir + "\\Output.txt";
        if not os.path.exists(FullFilePath):
            print("{0} 不存在'Output.txt'".format(DirName))
            continue;

        
        print("-------{0}------------\n".format(DirName));
        with open(FullFilePath, "r", encoding="utf-8") as hFile:
            for Line in hFile:
                if Line.find("UnMatch OpCod") == -1 and Line.find("Faild") == -1:
                    continue;
        
                
                print(Line + "\n");
        

input();