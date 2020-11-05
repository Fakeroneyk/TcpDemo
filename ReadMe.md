- 项目启动
    - sh build.sh
    - cd Bin/
    - ./server  ./client

- 客户端启动后  键入 1 或者 2  发送两种协议。目前仅支持这两种

- Common/TcpConnection.h  修改SERVER_IP的值，改变服务器地址。

- 服务器根据uin返回数据，修改客户端代码 

```
void send_msg(int32_t uin, int32_t cmd_id, google::protobuf::Message &msg);
//调用时修改第一个参数来区分不同客户端
```
