# esp32 电子时钟代码

## 架构

全局初始化四个对象：

- 时钟对象：提供精准的定时
- 渲染器对象：提供显示渲染实现
- 事件队列：管理按钮事件
- 用户交互对象：指示渲染及事件处理

同时初始化三个任务：

- 显示渲染任务：循环向tm1650提交渲染数据（考虑改为当buffer变化时再提交修改）
- 事件扫描任务：循环扫描按钮状态，发现变化则压入事件队列
- 事件传播任务：循环从事件队列中取出事件，传递给用户交互对象
- 用户交互传递任务：循环向渲染器提交渲染数据

用户交互对象负责存储每个用户交互界面的内容，其包括

- 显示指示器
- 事件处理器
