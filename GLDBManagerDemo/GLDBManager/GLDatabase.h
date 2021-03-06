//
//  GLDatabase.h
//  GLDBManagerDemo
//
//  Created by GrayLand on 16/7/15.
//  Copyright © 2016年 GrayLand. All rights reserved.
//

/* =============================================================
                数据库模型的基类, 定义一些通用接口
 =============================================================*/

#import <Foundation/Foundation.h>
#import <FMDB/FMDB.h>
#import "GLDBModel.h"

#ifdef DEBUG
#   define DBLog(fmt, ...)  NSLog((@"%s [Line %d] >>>\n" fmt), __PRETTY_FUNCTION__, __LINE__, ##__VA_ARGS__)
#else
#   define DBLog(...)
#endif

NS_ASSUME_NONNULL_BEGIN

@class GLDatabase;

typedef void (^_Nullable GLDatabaseCloseCompletion)(GLDatabase *database, BOOL successfully);
typedef void (^_Nullable GLDatabaseUpdateCompletion)(GLDatabase *database, id<GLDBPersistProtocol> _Nullable model, NSString * _Nullable sql, BOOL successfully, NSString * _Nullable errorMsg);
typedef void (^_Nullable GLDatabaseDeleteCompletion)(GLDatabase *database, BOOL successfully, NSString * _Nullable errorMsg);
typedef void (^GLDatabaseUpgradeCompletion)(GLDatabase *database, NSString *sql, BOOL successfully);
typedef void (^GLDatabaseQueryCompletion)(GLDatabase *database, NSMutableArray <id <GLDBPersistProtocol>> * _Nullable models, NSString * _Nullable sql);
typedef void (^_Nullable GLDatabaseExcuteCompletion)(GLDatabase *database, id _Nullable respond, BOOL successfully, NSString * _Nullable errorMsg);

@interface GLDatabase : NSObject

@property (nonatomic, strong) NSString *path;///< 数据库路径
@property (nonatomic, assign) BOOL isOpened;///< 是否已打开

@property (nonatomic, strong) FMDatabaseQueue *dbQueue;
@property (nonatomic, strong) dispatch_queue_t readQueue;
@property (nonatomic, strong) dispatch_queue_t writeQueue;
@property (nonatomic, strong) dispatch_queue_t completionQueue;

@property (nonatomic, strong) NSArray *allTableCached;


/**
 * @brief 打开数据库文件
 */
- (void)openDatabaseWithPath:(NSString * _Nonnull)path;

/**
 * @brief 关闭数据库, 有 completion 异步, 无 completion 则同步
 */
- (void)closeDatabaseWithCompletion:(GLDatabaseCloseCompletion)completion;

#pragma mark - Helper

/**
 * @brief 获取所有表名称
 */
- (NSArray <NSString *> *)getAllTableNameUsingCache:(BOOL)usingCache;

/**
 * @brief 获取表的所有列的信息
 */
- (NSArray <NSDictionary *> *)getAllColumnsInfoInTable:(NSString *)table;

#pragma mark - Create

/**
 * @brief 注册: 根据Model自动创建表, 若有新字段则自动添加, 若有自定义升级则使用自定义升级
 */
- (void)registTablesWithModels:(NSArray <Class <GLDBPersistProtocol>> *)models;

#pragma mark - Basic

/**
 * @brief 执行查询功能的 SQL, 默认当前线程
 */
-  (NSMutableArray *)executeQueryWithSQL:(NSString *)sql completion:(GLDatabaseExcuteCompletion)completion;

/**
 * @brief 执行更新功能的 SQL, 默认当前线程
 */
- (void)excuteUpdateWithSQL:(NSString *)sql completion:(GLDatabaseExcuteCompletion)completion;

#pragma mark - Insert

/**
 * @brief 插入 Model
 */
- (void)insertModel:(id <GLDBPersistProtocol>)model completion:(GLDatabaseUpdateCompletion)completion;

/**
 * @brief 插入 Model
 * @param isUpdateWhenExist YES-当插入对象已存在时, 如果是使用 primaryKey, 则更新, 反之则返回错误.
 */
- (void)insertModel:(id <GLDBPersistProtocol>)model isUpdateWhenExist:(BOOL)isUpdateWhenExist completion:(GLDatabaseUpdateCompletion)completion;

/**
 插入 Models, 目前仅支持 NSString 和 NSNumber及NSNumber支持的(NSInteger, float等)属性
 */
- (void)insertMassOfModels:(NSArray <id <GLDBPersistProtocol>> *)models completion:(GLDatabaseUpdateCompletion)completion;

#pragma mark - Query

/**
 * @brief 查询, 异步
 * @param condition e.g. : @"age > 10", @"name = Mike" ...
 */
- (void)findModelWithClass:(Class <GLDBPersistProtocol>)class condition:(NSString * _Nullable)condition completion:(GLDatabaseQueryCompletion)completion;

/**
 * @brief 查询, 同步
 * @param condition e.g. : @"age > 10", @"name = Mike" ...
 */
- (NSMutableArray <id <GLDBPersistProtocol>> *)findModelWithClass:(Class)class condition:(NSString * _Nullable)condition;

#pragma mark - Update

/**
 * @brief 全量更新 Model, 更方便. autoIncrement=YES, 使用modelId 匹配, autoIncrement=NO, 使用 primaryKey匹配.
 */
- (void)updateModelWithModel:(id <GLDBPersistProtocol>)model withCompletion:(GLDatabaseUpdateCompletion)completion;

/**
 * @brief 全量更新 Model, 更方便.
 */
- (void)updateModelWithModel:(id <GLDBPersistProtocol>)model withCondition:(NSString * _Nullable)condition completion:(GLDatabaseUpdateCompletion)completion;

/**
 * @brief 手动更新, 效率更高.
 * @param bindingValues A Binding Dictionary that key=propertyName, value=propertyValue.
 */
- (void)updateInTable:(NSString * _Nonnull)table withBindingValues:(NSDictionary * _Nonnull)bindingValues condition:(NSString * _Nonnull)condition completion:(GLDatabaseUpdateCompletion)completion;

#pragma mark - Delete

/**
 * @brief 删除 Model
 */
- (void)deleteModelWithModel:(id <GLDBPersistProtocol>)model completion:(GLDatabaseDeleteCompletion)completion;

/**
 * @brief 删除 Model, 通过 condition, 若 condition == nil, 则删除所有.
 */
- (void)deleteInTable:(NSString *)table withCondition:(NSString * _Nullable)condition completion:(GLDatabaseDeleteCompletion)completion;

@end

NS_ASSUME_NONNULL_END
