
# 介绍
&emsp;&emsp;该http服务器编写于2020年2月19日，重构于2021年7月18日；未重构前的版本，是笔者在大四春招时写的，代码比较随心，没有标准
不规范；2021年7月18日重构时，是笔者正准备辞掉毕业后的第一份工作为接下来的找工作而准备的，这次的代码算是比较标准规范的，笔者所在
的公司有着比较严格编码规范标准，所以，笔者也养成了规范编码格式的习惯。  
&emsp;&emsp;关于2021年07月18日用C++11重构整个项目的过程做一个简单的介绍。就笔者重构完整个项目的心得来说，从零开始写一个项目比重构一个已有的项目简单得多；重构的过程中，主要做了几个方面的事情:  
&emsp;&emsp;1. 首先，思考项目的设计模型，思考需要哪些类，思考类的作用。  
&emsp;&emsp;2. 根据1中的想法编写一个简要的设计文档，规划整个http服务端的流程，用草稿纸画出具体的流程视图，最终确认整个服务端的重构设计方案，明确所有类的作用，并将这些编写成一份简要的设计文档。  
&emsp;&emsp;3. 根据2中的设计文档开始具体的编码工作，编码工作结束后进行调试测试。  
&emsp;&emsp;4. 整理项目文档，整理项目代码，进行项目总结，最终归档入库。  
&emsp;&emsp;综合来说，重构后的项目，其实更像是一个网络库，实现http的get请求功能只是作为网络库的一次实践。  

# 目的
&emsp;&emsp;笔者知识的实践，C++学习的升华，找工作的准备，分享的意义。  

# 编译
Linux终端下将当前工作目录切换至项目目录的build目录，然后执行  
`cmake ..`  

执行后会配置生成makefile，再执行  
`make`  
即可生成server程序；  

清除所有编译输出，可以执行  
`make clean`  
  
如果CMakeList.txt有更新，请删除掉build目录下的所有文件后重新生成makefile，即执行  
`cmake ..`  
  
编译结束后，会在bin目录生成server程序，赋予其执行权即可执行  
`chmod 777 ../bin/server`  
  
以笔者为例，笔者电脑上运行server程序的虚拟机地址192.168.31.212为例，在笔者电脑上打开浏览器输入  
192.168.31.212:8888即可发起请求获取index.html，浏览器加载呈现。  
 
# 遇到的问题
&emsp;&emsp;[遇到的问题.txt](https://github.com/xyq-c-cpp/study/blob/master/WebServer/Resource/%E9%81%87%E5%88%B0%E7%9A%84%E9%97%AE%E9%A2%98.txt)

# 总结
&emsp;&emsp;其实，重构完整个项目之后，是有一定的收获；但，总感觉还是少了点什么...  
  
# 尾语
&emsp;&emsp;愿我们能保持一颗持续学习的心，加以实践，锤炼技术；

# 附录
&emsp;&emsp;[readme在线编辑器](http://mahua.jser.me/)  
&emsp;&emsp;[readme语法实例](https://www.cnblogs.com/S-volcano/p/9454194.html#:~:text=readme%20%E8%AF%AD%E6%B3%95.%20README.%20%E8%AF%A5%E6%96%87%E4%BB%B6%E7%94%A8%E6%9D%A5%E6%B5%8B%E8%AF%95%E5%92%8C%E5%B1%95%E7%A4%BA%E4%B9%A6%E5%86%99README%E7%9A%84%E5%90%84%E7%A7%8Dmarkdown%E8%AF%AD%E6%B3%95%E3%80%82.,GitHub%E7%9A%84markdown%E8%AF%AD%E6%B3%95%E5%9C%A8%E6%A0%87%E5%87%86%E7%9A%84markdown%E8%AF%AD%E6%B3%95%E5%9F%BA%E7%A1%80%E4%B8%8A%E5%81%9A%E4%BA%86%E6%89%A9%E5%85%85%EF%BC%8C%E7%A7%B0%E4%B9%8B%E4%B8%BA%60GitHub%20Flavored%20Markdown%60%E3%80%82.%20%E7%AE%80%E7%A7%B0%60GFM%60%EF%BC%8CGFM%E5%9C%A8GitHub%E4%B8%8A%E6%9C%89%E5%B9%BF%E6%B3%9B%E5%BA%94%E7%94%A8%EF%BC%8C%E9%99%A4%E4%BA%86README%E6%96%87%E4%BB%B6%E5%A4%96%EF%BC%8Cissues%E5%92%8Cwiki%E5%9D%87%E6%94%AF%E6%8C%81markdown%E8%AF%AD%E6%B3%95%E3%80%82.)