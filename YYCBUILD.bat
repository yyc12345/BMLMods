:: Build BML
py github_action_gen_props.py -t BML -p "F:\Ballance\BallanceBML\ModLoader\Mods" -s ".bmod" -i "D:\CppLib\BML\include" -l "D:\CppLib\BML\lib\Release" -n BML.lib
MSBuild BMLMods.sln -p:Configuration=Release -p:Platform=x86

:: Build BMLP
py github_action_gen_props.py -t BMLP -p "F:\Ballance\BallanceBMLP\ModLoader\Mods" -s ".bmodp" -i "D:\CppLib\BMLP\Include;D:\CppLib\BMLP\Include\BMLPlus\Virtools" -l "D:\CppLib\BMLP\Lib" -n "BMLPlus.lib;ImGui.lib;VxMath.lib;CK2.lib"
MSBuild BMLMods.sln -p:Configuration=Release -p:Platform=x86
