# KyoceraAddressBook


## 关于
* 这是我在广州某公司任职期间的练习项目。
* 这是一个管理职员信息的地址薄，主要通过连接打印机存取地址薄信息，方便打印和传输。使用标准c++实现的MVC模式，并使用MFC的ControlList实现数据显示，数据通过连接打印机设备来读写。具体还是看docs里面的文件。
可以扩展实现自定义的model，代码中有大量注释。

## 功能
* 基于C++/STL(lib模块),MFC GUI(应用程序模块)。
* Model/View的设计模式(未使用Control层，不过已留有AbstractItemController模块)。
* 支持本地数据文件存取（纯文本格式）。
* 支持连接到打印机存取数据，支持Perry Dell E525W 类型的打印机。
* 支持打印机数据导出到本地。
* 支持添加/删除/修改/成员信息，并同步本地或打印机。
* 支持分组，支持组成员管理。
* 仅支持Windows （XP或以上）。

## 依赖
* 依赖打印机驱动dll，驱动在[Drivers](https://github.com/newdebug/KyoceraAddressBook/tree/master/Drivers)目录下。当前提供的驱动适用于Perry Dell E525W打印机。

## 如何编译
1. clone 整个项目。
2. 安装Visual Studio 2010~2013，打开FaxAddressBookUtility.sln。
3. 编译解决方案。
4. 编译生成在Build目录下。
5. 编译完成后会自动拷贝KyoceraAppd.exe文件和[Drivers](https://github.com/newdebug/KyoceraAddressBook/tree/master/Drivers)目录下的驱动到[Bin](https://github.com/newdebug/KyoceraAddressBook/tree/master/Bin)下。
6. 运行KyoceraAppd.exe。

## 项目说明
* AdressBook是核心库，用于地址薄数据的读写管理，优先编译出lib库，项目中默认编译出静态的lib，以用于应用程序引用。
	* 引入模块：AddressBook.h/EntryItem.h/AbstractModel.h。
	* 抽象模块AbstractModel被应用程序的View控件引用，以便显示数据，项目中已实现一个具体的Model，可以从AbstractModel扩展自定义的model。
* KyoceraApp是GUI应用程序，生成可执行文件，基于MFC实现，需引入AddressBook生成的AddressBook.lib库。
* Test模块是测试AddressBook各功能的。
* Drivers 目录存放打印机的驱动dll。
* Include 目录存放AddressBook的头文件AddressBook.h/EntryItem.h/AbstractModel.h。
* Docs 目录存放项目的设计文档，UML，MVC实现模式，应用程序界面截图等。

## 版权
* 仅用于个人学习练习，不得用于商业用途，你可以复制拷贝代码应用于你的项目，但请注明作者。

## 联系
* Email: yuri.young@qq.com
* QQ: 12319597
