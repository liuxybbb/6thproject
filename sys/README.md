## sys检验方法：
	1. make --> 编译模块
	2. sudo insmod sys.ko --> 加载模块
	3. cat /sys/kernel/Tom/delay --> 查看sysfs里的值
	4. echo 99 > /sys/kernel/Tom/delay --> 向sysfs中写入99
	5. cat /sys/kernel/Tom/delay --> 查看sysfs里的值
