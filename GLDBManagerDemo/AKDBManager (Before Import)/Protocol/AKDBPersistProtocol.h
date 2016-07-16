//
//  AKDBPersistProtocol.h
//  GLDBManagerDemo
//
//  Created by GrayLand on 16/7/15.
//  Copyright © 2016年 GrayLand. All rights reserved.
//

#ifndef AKDBPersistProtocol_h
#define AKDBPersistProtocol_h

@protocol AKDBPersistProtocol

@required
@property (nonatomic, strong) NSString *modelId;




/**
 *  获取tableName
 *
 *  @return tableName
 */
+ (NSString *)tableName;

/**
 *  返回创建与该Model对应的表的SQL语句
 *
 *  @return 创建表的SQL语句
 */
+ (NSString *)creationSql;

/**
 *  返回更新与该Model对应的表的SQL语句
 *
 *  @return 更新表的SQL语句
 */
+ (NSArray *)upgradeSqls;

/**
 *  字典类型转Model类型
 *
 *  @param dic 字典类型
 *
 *  @return Model类型
 */
+ (id<AKDBPersistProtocol>)modelWithDatabaseDictionary:(NSDictionary *)dic;

/**
 *  Model类型转字典类型
 *
 *  @return 字典类型
 */
- (NSMutableDictionary *)toDatabaseDictionary;


@optional
+ (BOOL)propertyIsIgnored:(NSString *)propertyName;



@end


#endif /* AKDBPersistProtocol_h */

