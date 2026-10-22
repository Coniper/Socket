# Unix Socket 网络编程练习

Linux C 语言套接字编程学习记录，按通信方式与专题组织：

- [TCP](./TCP)、[UDP](./UDP)、[Unix_socket](./Unix_socket)：基础通信。
- [Concurrent](./Concurrent)：进程与线程并发服务端。
- [IOmodel](./IOmodel)：非阻塞、`select`、`poll`、`epoll`，包括 IPv6 示例。
- [broadcast](./broadcast)、[multicast](./multicast)：广播与组播。
- [sockopt](./sockopt)：Socket 选项。
- [Photo_Transfer](./Photo_Transfer)：文件/图片传输实验。

这是教学实验代码，不是经过安全审查的网络服务框架。运行服务端前请检查监听地址、端口和网络环境，不要直接暴露到公网。
