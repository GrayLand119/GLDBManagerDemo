# 1.0.4

 1. autoIncrement 类型的 Model, 插入时不自动生成 primaryKey 的值;
 2. 关闭数据库方法在同步任务中执行, 确保回调时所有数据都插入完毕;
 3. 添加插入数组的方法;