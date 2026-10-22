# Socket 网络编程

这是我写得比较散、但内容也比较多的一组 Linux C 网络编程练习。从最初的 TCP、UDP 收发，到广播、组播、Unix Domain Socket，再往后试过并发服务器和 I/O 多路复用。

代码按主题放在目录里：

- `TCP`、`UDP`、`Unix_socket`：连接与收发；
- `Concurrent`：进程、线程处理客户端；
- `IOmodel`：非阻塞、`select`、`poll`、`epoll`，还有 IPv6 示例；
- `broadcast`、`multicast`、`sockopt`：补充实验；
- `Photo_Transfer`：一次文件传输尝试。

我保留了当年的文件名，包括其中的拼写错误，不想为了整理把原始练习改得面目全非。

这里的服务端只是实验用代码，地址、端口、输入处理等都有需要重新检查的地方。不要直接放到公网跑。
