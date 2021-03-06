# GLDBManagerDemo

# Release Note 

[Release Note](ReleaseNote.md)

## 简介

`GLDBManager`是基于`FMDB`和`YYModel`的写的轻量级数据库插件. 在 Demo 中演示了如何使用它.

设计思想和特点:

设计初衷也就是特点, 就是非常方便使用. CURD都可以直接对对象进行操作, 不需要写SQL语句, 全部依赖 Runtime 自动生成.

主要功能:

1. 自动创建数据库: 根据 Model 属性,自动建表.
2. 自动升级数据库: 在 Model 中修改属性名字/添加属性/修改主键后, 自动更新表的字段.
3. Model 可以嵌套, 即 Model 中可以包含 Model;
4. Model 可以设置是 `自增长`或`唯一主键`类型.
5. 仿 `CoreData`, 可以`直接对模型进行增删改查`. 也提供基础的手动执行SQL语句的方法.
6. 数据库支持类型: `所有常数型变量(NSInteger/int/float/bool...)`, `NSDate`, `NSData`, `NSArray`,`NSMutableArray`,`NSDictionary`,`NSMutableDictionary`,`实现了YYModel协议的嵌套对象`

组件介绍:

1. `GLDBManager` 提供对多个数据库的管理, 自带一个默认数据库, 另外可根据业务需求添加多个数据库. 比如, `根据大文件读写/高低频/高低优先级,来拆分数据库`.
2. `GLDatabase` 提供对数据库的 `增/改/查/删(CURD)`操作, 自带读写线程.
3. `GLDBModel` 模型要归档的基础实现.
4. `GLDBPersistProtocol` 协议, 数据库归档以及 CURD 的基础协议.

实现原理:

使用了Runtime, 能够根据当前的Model类自动生成SQL语句, 从而实现自动建表和自动更新, 在数据库表结构改变的情况下升级数据库保留旧数据, 自动插入新字段.

![DemoImage](https://github.com/GrayLand119/GLDBManagerDemo/blob/master/GLDBManagerDemo.jpg)

目前数据库支持5个类型, 会通过 `runtime` 自动生成:

* 所有整型类型都存储为 `INTEGER` 类型.
* 所有浮点类型都存储为 `REAL` 类型.
* 字符串类型和所有对象类型(JSON)存储为 `TEXT` 类型.
* `NSDate`/`NSData`/`NSArray`/`NSMutableArray`/`NSDictionary`/`NSMutableDictionary`/`NSObject<YYModel>`都会转化为`NSData`并存储为`BLOB`类型,并在读取时使用`runtime`自动还原成对应的类型.
* 除了以上类型以外的其他类型都存储 `NONE` 类型.

## 安装

```
source 'https://github.com/GrayLand119/GLSpecs.git'
source 'https://github.com/CocoaPods/Specs.git'

...

pod 'GLDBManager'
```


或者: 

```
pod 'GLDBManager', :git => 'https://github.com/GrayLand119/GLDBManager.git'
```


## 使用方法

```objc

// 1. 打开or自动创建数据库
_dbManager = [GLDBManager defaultManager];
if ([_dbManager openDefaultDatabase]) {
    NSLog(@"打开数据库 成功!");
}else {
    NSLog(@"打开数据库 失败!");
}

// 2. 注册需要归档的对象
// 注册过程: 2.1 检查表是否存在/不存在自动创建 ---> 2.2 检查表字段和Model的字段是否匹配,不匹配则自动添加字段
// 2.2 若实现了手动升级方法, 则执行手动升级方法
[_dbManager.defaultDB registTablesWithModels:@[Car.class, OtherModel.class]];

// 3. 添加插入数据
Car *car = [Car new];
car.name = [NSString stringWithFormat:@"Car-%ld", arc4random_uniform(100)];
car.age  = arc4random_uniform(120) + 10;

[_dbManager.defaultDB insertModel:car completion:^(GLDatabase *database, id<GLDBPersistProtocol> model, NSString *sql, BOOL successfully, NSString *errorMsg) {

    NSLog(@"Insert: %@", successfully?@"Success":@"Failed");
    if (errorMsg) {
    NSLog(@"Error:%@", errorMsg);
    }
}];

// 4. 更新数据
car.age = arc4random_uniform(5);
car.name = [NSString stringWithFormat:@"c%@", car.name];
// 方法1
[_dbManager.defaultDB updateModelWithModel:car withCompletion:^(GLDatabase *database, id<GLDBPersistProtocol> model, NSString *sql, BOOL successfully, NSString *errorMsg) {
    NSLog(@"Update %@", successfully?@"Successed!":@"Failed!");
}];
// 方法2
[_dbManager.defaultDB updateInTable:[Car tableName]
                              withBindingValues:@{@"age":@10,
                                                  @"name":@"A63 AMG"}
                                      condition:@"modelId = 1"
                                     completion:^(GLDatabase *database, id<GLDBPersistProtocol> model, NSString *sql, BOOL successfully, NSString *errorMsg) {
                                         
                                     }];

// 5. 查询数据
[_dbManager.defaultDB findModelWithClass:[Car class] condition:@"age > 0" completion:^(GLDatabase *database, NSMutableArray<id<GLDBPersistProtocol>> *models, NSString *sql) {
    NSLog(@"%@", models);
}];

// 6. 删除数据
// 方法1
[_dbManager.defaultDB updateModelWithModel:car withCompletion:^(GLDatabase *database, id<GLDBPersistProtocol> model, NSString *sql, BOOL successfully, NSString *errorMsg) {
    NSLog(@"Update %@", successfully?@"Successed!":@"Failed!");
}];
// 方法2
[_dbManager.defaultDB deleteInTable:[Car tableName] withCondition:@"age = 5" completion:^(GLDatabase *database, BOOL successfully, NSString *errorMsg) {
}];

```

## 多线程

做了简单的读写分线程, 由于`FMDatabaseQueue`已经做了读写隔离, 在读/写表时加了锁, 是线程安全的, 所以就不再加锁控制. 

`GLDatabase` 创建时自带以下属性:

```objc
@property (nonatomic, strong) dispatch_queue_t readQueue; // 读线程
@property (nonatomic, strong) dispatch_queue_t writeQueue; // 写线程
@property (nonatomic, strong) dispatch_queue_t completionQueue; // 回调的主线程


- (instancetype)init {
    if(self = [super init]) {
        _readQueue = dispatch_queue_create("com.gldb.readqueue", DISPATCH_QUEUE_CONCURRENT);
        _writeQueue = dispatch_queue_create("com.gldb.writequeue", DISPATCH_QUEUE_CONCURRENT);
        _completionQueue = dispatch_get_main_queue();
    }
    return self;
}
```
使用 `GLDatabase`时, 默认的 CURD 方法都是在后台线程运行的, 不需要做额外操作.

只有两个底层的基础方法是在默认线程的.

```objc
/**
* @brief 执行查询功能的 SQL, 默认当前线程
*/
- (NSMutableArray *)executeQueryWithSQL:(NSString *)sql completion:(GLDatabaseExcuteCompletion)completion;

/**
* @brief 执行更新功能的 SQL, 默认当前线程
*/
- (void)excuteUpdateWithSQL:(NSString *)sql completion:(GLDatabaseExcuteCompletion)completion;
```

手动使用多线程运行可以像以下这样写:

```objc
// 查询操作
dispatch_async(_dbManager.defaultDB.readQueue, ^{
    // Do Query...
});

// 更新操作
dispatch_async(_dbManager.defaultDB.writeQueue, ^{
    // Do Update...
});
```
