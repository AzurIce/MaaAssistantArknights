## v4.10.6-beta.1

- 新增 `生息演算` 简单流程支持 @WLLEGit
- 修复 基建 会客室 不收取线索的问题 @Sed66666
- 修复 上传 企鹅物流错误时，任务卡很久的问题 @MistEO
- 修复 界面 更新公告语言设置错误 @horror-proton
- 修复 界面 检查更新按钮连续点击时的错误 @ABA2396
- 修复 界面 导航选项冲突 @ABA2396

- 新增 发版镜像 @GalvinGao

### For overseas

- Updated Korean translation @178619

## v4.10.5

- 修复 保全派驻 任务流程错误 @MistEO
- 修复 请求更新失败时的崩溃问题 @ABA2396
- 修复 系统语言为繁中时，启动时崩溃的问题 @MistEO @dantmnf
- 添加 部分日志打印 @horror-proton

## v4.10.4

- 新增 adb-lite 连接方式 @hguandl
- 新增 mac 版本 自动战斗抄作业功能 @hguandl
- 新增 界面 退出时自动保存上次位置 @moomiji @ABA2396
- 更新 自定义基建内置作业 @ABA2396 @martinwang2002 @MistEO
- 重构 界面 定时功能 @ABA2396
- 优化 界面 关卡名输入支持中文 @ABA2396
- 优化 界面 启动设置项 @Sed66666 @ABA2396
- 优化 `保全派驻` 内置作业 @HMZyueshen
- 优化 肉鸽 Pin Up 相关提示，优化分队选项 @ABA2396
- 优化 肉鸽 助战刷新次数 @WLLEGit
- 修复 界面 关卡名被重置的问题 @ABA2396
- 修复 OTA 包编码错误 @horror-proton
- 修复 mac 版本日志创建错误 @hguandl
- 修复了一些乱七八糟的小 bug 太多了以至于我懒得写（ @MistEO @horror-proton @ABA2396
- 更新 文档 @WWPXX233 @martinwang2002 @Rbqwow


### For overseas

- 新增 繁中服 火蓝之心导航、掉落识别、仓库识别支持 @vonnoq
- 修复 繁中服 基建识别 @vonnoq
- Supported trial mode for the EN client @RiichiTsumo
- Supported some new feautres for the JP Client @liuyifan-eric
- Fixed the operator names and recruitment tags recognition error in the JP client @cat122802501 @liuyifan-eric

### For develops

- Added `SingleStep` Task API @MistEO

## v4.10.3

- 修复 `保全派驻` 无限重试、第四关前卡住、通关后卡住、战斗结束仍在执行的问题 @MistEO
- 修复 `保全派驻` 内置作业文件名乱码问题 @horror-proton
- 更新 繁中服 第十章导航功能 @vonnoq

## v4.10.2

- 新增 `保全派驻` 内置通关作业 @HMZyueshen @AnnoyingFlowers
  均同时兼容标准和应急模式，请选择 `resource/copilot/` 下对应文件，并参考说明使用
- 修复 `登临意` 活动关卡导航错误 @ABA2396
- 修复 `保全派驻` 应急模式卡住的问题 @MistEO
- 优化 `保全派驻` 调配干员按钮的优先级 @MistEO
- 优化 傀影肉鸽 部分地图策略 @WWPXX233
- 更新 蓝叠模拟器 相关文档 @wordlesswind

## v4.10.1

- 更新 `登临意` 活动关卡资源 @MistEO
- 新增 `登临意` 活动关卡导航 @ABA2396
- 修复 `保全派驻` 抄作业一系列 bug @MistEO  
  目前仍未支持自动编队，请手动编队完成后于“开始部署”界面开始任务。敬请期待后续优化~
- 优化 界面 关卡导航选项及提示 @ABA2396
- 修复 工作目录和临时目录同为中文时，加载资源报错的问题 @MistEO
- 更新 文档 排版 @BTreeNewBee

## v4.10.0

- 新增支持 `引航者试炼` 抄作业功能，新增 `MoveCamera` 字段以移动镜头 @MistEO @horror-proton  
  请参考 [战斗流程文档](https://github.com/MaaAssistantArknights/MaaAssistantArknights/blob/master/docs/3.3-%E6%88%98%E6%96%97%E6%B5%81%E7%A8%8B%E5%8D%8F%E8%AE%AE.md) 使用
- 初步支持 `保全派驻` 抄作业功能，请参考 [保全派驻协议](https://github.com/MaaAssistantArknights/MaaAssistantArknights/blob/master/docs/3.7-%E4%BF%9D%E5%85%A8%E6%B4%BE%E9%A9%BB%E5%8D%8F%E8%AE%AE.md) 使用 @MistEO  
  目前 bug 非常多，仅供尝鲜。暂不支持自动编队，请手动编队完成后于“开始部署”界面开始任务。敬请期待后续优化~
- 新增 界面 新版本下载进度显示 @moomiji
- 新增 界面 活动剩余时间提示 @ABA2396
- 优化 资源加载 速度 @dantmnf @MistEO
- 重构 干员头像缓存机制 @MistEO
- 重构 界面 关卡导航列表从 web 获取 @MistEO @ABA2396
- 重写 掉落识别/仓库识别 数量检测算法 @horror-proton @MistEO
- 修复 自动战斗/肉鸽 不停点 CD 干员的问题 @MistEO
- 修复 自动战斗/肉鸽 偶现干员识别错误 @MistEO
- 修复 肉鸽 助战功能 翻页后不识别的问题 @WLLEGit
- 修复 界面 关卡选择为 null 时的崩溃问题 @ABA2396
- 优化 日志、配置文件、缓存文件 等目录结构 @MistEO
- 优化 界面 自定义基建日志打印 @MistEO
- 更新 自定义基建 内置作业界面 @ABA2396
- 更新 文档，更新 加群链接 @MistEO @ntgmc @DavidWang19

### For overseas

- Updated game data @MistEO
- Added translations for drop settings @ABA2396

### For develops

- Optimized the Rust interface @KevinT3Hu
- Updated Linux tutorial @horror-proton
