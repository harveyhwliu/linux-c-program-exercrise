#目标：依赖（条件）
#	命令

#阶段1、最简单的一种，每次增删文件需要都修改Makefile 文件
#app:01testGdb.c
#	gcc -o app 01testGdb.c
#阶段2
#app:01testGdb.o
#	gcc -o app 01testGdb.o
#01testGdb.o:01testGdb.c
#	gcc -c 01testGdb.c#注意这里后面不需要在写01testGdb.o 了，因为默认gcc -o 01testGdb.c就会产生01testGdb.o,加上的话就错了
#阶段3
# $@表示目标，$^表示所有依赖，$<表示依赖中的第一个
#OBJ=01testGdb.o
#app:$(OBJ)
#	gcc $(OBJ) -o app
##make -p 可以查看内嵌的配置命令等，如：%o:%c,默认的.o文件依赖对应名称的.c文件生成
#%o:%c
#	gcc -c $^ -o $@	
##gcc -c $< -o $@  #这里要生成的目标为$@,依赖可以是$<(所有依赖中的第一个)，也可以是$^(所有依赖),注意，这里的-o选项不可省略
##gcc -c $^ -o $@
#阶段四 函数： src=$(函数名 参数1,参数2)
#src=$(wildcard *.c)#wildcard字符串查找，查找当前目录下所有的.c文件
#obj=$(patsubst %.c,%.o,$(src))#patsubst模式查找并替换，在src变量中把所有.c结尾的替换成对应.o结尾的
#target=app
#
#$(target):$(obj)
#	gcc -o $@ $^
#%.o:%.c
#	gcc -c $< -o $@	
#test:
#	@echo $(src)
#	@echo $(obj)
#阶段五：定义更多的变量
CPPFLAGS=-I./myinclude#一般预编译参数，如指定头文件目录为include
CFLAGS=-O3 -Wall#编译时参数，是否添加调试信息,调试的时候使用-g,发布时不能使用-g
LDFLAGS=#加载时参数指定共享库，-L../lib -lmycalc 先指定目录在上级目录下lib下，库为mycalc
CC=gcc#指定编译器gcc

src=$(wildcard *.c)#wildcard字符串查找，查找当前目录下所有的.c文件
obj=$(patsubst %.c,%.o,$(src))#patsubst模式查找并替换，在src变量中把所有.c结尾的替换成对应.o结尾的
target=app

$(target):$(obj)
	$(CC) $(LDFLAGS) -o $@ $^
%.o:%.c
	$(CC) -c $^ $(CPPFLAGS) $(CFLAGS) -o $@	


#目标没有依赖，直接执行命令，以示更新 ,PHONY用来声明一个伪目标
.PHONY:clean
clean:
	-@rm -f app *.o
#命令出错，make不再继续执行，而用-表示失败后继续执行，即-rm -f;@不显示 命令本身
