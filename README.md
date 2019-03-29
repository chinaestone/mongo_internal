基于mongodb 2.x.x版本  

### dump
导出一个collection的所有记录到文件collname.bson，可以被mongoresotre使用  
具体原理可以查看[这篇博客](http://www.cnblogs.com/tripleH/archive/2013/03/15/2958147.html)

### print_db_ns
打印一个库的所有namespace

### print_journal
打印journal文件  
具体原理可以查看[这篇博客](http://www.cnblogs.com/tripleH/archive/2013/03/21/2972678.html)

### 编译调试
```shell
cmake .
make

./dump db_data_dir db_name collection_name
> generate collection_name.bson
> bsondump collection_name.bson  # 以json格式查看dump出来的内容
```



