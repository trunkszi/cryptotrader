### 初始化项目配置

#### 配置项目平台
```shell
# windows arch x64 debug mode 
xmake f -y -p windows -a x64 -m debug 
# windows arch x64  release mode
xmake f -y -p windows -a x64 -m release 

# windows arch x86 debug mode
xmake f -y -p windows -a x86 -m debug 

# windows arch x86 release mode
xmake f -y -p windows -a x86 -m debug 

# linux arch x86_64 debug mode
xmake f -y -p linux -a x86_64 -m debug 

# linux arch x86_64 release mode
xmake f -y -p linux -a x86_64 -m debug 
```

#### 生成IDE配置文件
##### 生成 vistual studio 配置文件
```shell
# vistual studio 2022
xmake project -k vs2022 -m "release,debug"  -a "x86,x64" -y

# vistual studio 2019
xmake project -k vs2019 -m "release,debug"  -a "x86,x64" -y
```

##### 生成 clion CMakeList.txt 配置文件
```shell
xmake project -k cmake -m "debug"  -a "x64" -y

xmake project -k cmake -m "debug"  -a "x86_64" -y
```

#### windows 服务模式
```shell
./cryptoserver.exe --install
```

##### windows 服务模式运行报错1063
1. 无权限service用户无控制权限
   + `Service Security Editor` 是windows service权限管理工具、[下载链接](https://www.coretechnologies.com/products/ServiceSecurityEditor/ServiceSecurityEditor.exe)
   + `Service Security Editor` [使用说明](https://www.coretechnologies.com/products/ServiceSecurityEditor/#grant-start-stop-service)
2. 未注册服务
   + 默认情况下执行`./cryptoserver.exe --install 或 ./cryptoserver.exe --service` 会自动注册服务、自动注册服务参数如下:
   + ./cryptoserver.exe --install --name "cryptotrader" --display "cryptoserver" --mode auto --path argv[0]
   ```shell
   ./cryptoserver.exe --install  --name "cryptotrader" --displayname "cryptotrader" --description "the crypto trader server"
   ```

#### linux 服务模式
```shell
./cryptoserver.exe --daemon
```

#### 日志目录
+ windows
  1. windows服务模式下默认写入C:\cryptotrader目录
  2. windows命令行模式下默认写入./logs目录
+ linux
  1. linux 命令行模式下默认写入./logs目录
  2. linux daemon 需要通过 --log_dir 参数指定日志目录、建议写入/var/log/cryptotrader目录
+ 日志文件名称与可执行文件名称相同、日志文件自动旋转、文件默认大小256Mib、文件最大数量8个、超过8个自动覆盖最旧文件


#### 命令行参数
```shell
# 查看命令行参数列表
./cryptotrader --         
--also_log_to_local: #0 if true, also log to local file when write-cb is set
         type: bool          default: false
         from: src\log\log.cc
--check: Check if the service is installed
         type: bool          default: false
         from: C:\Users\Administrator\Desktop\cryptotrader\include\flags.h
--co_debug_log: #1 enable debug log for coroutine library
         type: bool          default: false
         from: src\co\scheduler.cc
--co_sched_num: #1 number of coroutine schedulers, default: os::cpunum()
         type: uint32        default: os::cpunum()
         from: src\co\scheduler.cc
--co_stack_size: #1 size of the stack shared by coroutines, default: 1M
         type: uint32        default: 1024 * 1024
         from: src\co\scheduler.cc
--config: .path of config file
         type: string        default: ""
         from: src\flag.cc
--cout: #0 also logging to terminal
         type: bool          default: false
         from: src\log\log.cc
--daemon: #0 run program as a daemon
         type: bool          default: false
         from: src\flag.cc
--depends: Set service dependency, default: "", format: "service1\service2\service3"
         type: string        default: ""
         from: C:\Users\Administrator\Desktop\cryptotrader\include\flags.h
--desc: Specify the content of the service description
         type: string        default: ""
         from: C:\Users\Administrator\Desktop\cryptotrader\include\flags.h
--disable_co_exit: .disable co::exit if true
         type: bool          default: false
         from: src\co\scheduler.cc
--disable_hook_sleep: #1 disable hook sleep if true
         type: bool          default: false
         from: src\co\hook_win.cc
--display: Specify the SCM Manager display name
         type: string        default: "cryptotrader"
         from: C:\Users\Administrator\Desktop\cryptotrader\include\flags.h
--help: .help info
         type: string        default: ""
         from: src\flag.cc
--hook_log: #1 enable log for hook if true
         type: bool          default: false
         from: src\co\hook_win.cc
--install: Run as install service
         type: bool          default: false
         from: C:\Users\Administrator\Desktop\cryptotrader\include\flags.h
--log_compress: #0 if true, compress rotated log files with xz
         type: bool          default: false
         from: src\log\log.cc
--log_daily: #0 if true, enable daily log rotation
         type: bool          default: false
         from: src\log\log.cc
--log_dir: #0 log dir, will be created if not exists
         type: string        default: "logs"
         from: src\log\log.cc
--log_file_name: #0 name of log file, using exename if empty
         type: string        default: ""
         from: src\log\log.cc
--log_flush_ms: #0 flush the log buffer every n ms
         type: uint32        default: 128
         from: src\log\log.cc
--max_log_buffer_size: #0 max size of log buffer, default: 32MB
         type: uint32        default: 32 << 20
         from: src\log\log.cc
--max_log_file_num: #0 max number of log files
         type: uint32        default: 8
         from: src\log\log.cc
--max_log_file_size: #0 max size of log file, default: 256MB
         type: int64         default: 256 << 20
         from: src\log\log.cc
--max_log_size: #0 max size of a single log
         type: int32         default: 4096
         from: src\log\log.cc
--min_log_level: #0 write logs at or above this level, 0-4 (debug|info|warning|error|fatal)
         type: int32         default: 0
         from: src\log\log.cc
--mkconf: .generate config file
         type: bool          default: false
         from: src\flag.cc
--mode: Set the service startup mode, options [auto, manaul], default: auto
         type: string        default: "auto"
--service: Run as service mode
         type: bool          default: false
         from: C:\Users\Administrator\Desktop\cryptotrader\include\flags.h
--syslog: #0 add syslog header to each log if true
         type: bool          default: false
         from: src\log\log.cc
--uninstall: Run as uninstall service
         type: bool          default: false
         from: C:\Users\Administrator\Desktop\cryptotrader\include\flags.h
--user: Set the user to which the service belongs, the default is the current user
         type: string        default: ""
         from: C:\Users\Administrator\Desktop\cryptotrader\include\flags.h

```