linux命令

raspi-config                                                   进入树莓派设置
bash ./XX.sh                                                   启动这个目录下的shell脚本
cat  /sys/class/thermal/thermal_zone0/temp   查看CPU温度（除以1000）
vcgencmd measure_temp                                查看CPU温度
rm -rf 文件名                                                 递归删除文件夹



============================================================================================
vi/vim命令

i    插入
dd   删除这一行
ndd  删除向下的n行，n为数字，如20dd
x    删除光标所在字符
yy   复制当前的一行
nyy  复制向下的n行，如20yy
p    小写p，粘贴在光标的下一行
P    大写p，粘贴在光标的上一行
0或[Home]  移动到这一行第一个字符
$或[End]   移动到这一行最后一个字符
G     移动到文件最后一行
gg    移动到文件第一行
n<Enter>   向下移动n行
u    复原前一个动作
[Ctrl]+r  重做上一个动作 
.    小数点，重复上一个动作
esc  退出插入模式
:q    退出
:q！  强制退出
:w    保存
:wq   保存并退出
:/关键字符串          vi中用于查找，之后用n移到下一个 
:set ff?           查看当前文本的模式类型，一般为dos,unix
:set ff=dos        设置为dos模式，也可以转换为unix模式

============================================================================================================
git命令

git add .                          有新文件加入时使用这一句

git fetch origin master    获取最新版本到本地;git merge origin master  将本地和远程合并
git pull origin master     合并以上两个命令，只需要这一条

git commit -a -m "Ubuntu0或者raspi或者Win再加上日期"      “”内的内容是表示这是谁修改的，必须先执行这一句，再push上服务器
git push origin master                                                                将本地版本推送到github

git log
git log --pretty=oneline                  查看历史版本，查询之后按q退出

git reflog                                         记录你的每一次命令

git reset --hard HEAD^                     上一个版本就是  HEAD^，上上一个版本就是  HEAD^^，往上100个版本写成  HEAD~100。
git reset --hard 3628164                    回退指定版本，“3628164”是版本的commit id的前几位
git push -f origin master                        将本地版本推送到github，由于是旧版本，github禁止push，需要强制-f

假設 A 是當前版本，B 是上個版本，退回 B 再提交一個 C 版本，那麼 C 不會擁有 A 以及 A 基礎上提交的版本修改的內容。C(HEAD^)是B， A(HEAD^)也是B,  類似多了一個分支一樣。
