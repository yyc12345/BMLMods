# Ballance OptiFine

Ballance高清修复（不是）

## 目的

此Mod的作用是为一些老旧地图添加阴影，渐变柱子等。效果不一定是最佳，且可能会因为开启而破坏了关卡原有体验的情况。并且此插件也不是为了让制图者可以偷懒不加影子和渐变柱子而存在的，作为一个合格的制图者，您应该至始至终尽力维护您的地图，测试地图时也别忘记关闭此插件。此Mod仅仅是为了给旧地图一个更好的体验。

## 用法

此Mod的设置全部在Mod菜单中，且至少需要BML 0.3.29版本才可以运行。

### OptiFine设置项

* GlobalEnable：此Mod的全局启用键，方便整体开关Mod
* Shadow：启用阴影修复，启用后将按照一定规则（见后文）来自动为地图加上阴影
* Material：启用材质修复：启用后按照一定规则修复材质错误，渐变柱子的修复在这里
* CK2_3DOverride：启用CK2_3D设置覆盖，启用后，将会把CK2_3D设置强制改为32bit ARGB8888模式，这与直接修改CK2_3D.ini等价。此选项的启用和关闭需要重新启动游戏来应用。启用此选项后会自动解决材质问题，并且Ballance界面将会变得更加平滑。

### Shadow设置项

Shadow设置项控制阴影的产生

* Shadow：被归入Shadow组的物体将被强制产生阴影
* Phys_Floors：被归入Phys_Floors组的物体将被强制产生阴影
* Phys_FloorRails：被归入Phys_FloorRails组的物体将被强制产生阴影

### Material设置项

Material设置项控制哪些材质需要被修复

* Column_beige_fade.tga：柱子渐变
* Laterne_Schatten.tga：灯阴影
* Laterne_Verlauf.tga：灯光线散射
* Modul18_Gitter.tga：风扇网格
* Stick_Bottom.tga：终点彩柱渐变
