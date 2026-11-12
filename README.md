"# IoT-Comm" 
# modbus
  Modbus协议是工业领域通用的开放式通信协议‌，核心用于不同设备间的标准化数据交互，覆盖从底层设备控制到上层平台接入的全链路场景，适用于物联网下设备的数据采集。<br>
项目modbus目录中的代码是QT的主站（modbus client）案例，这个案例已包含常用的4种数据区。在QT自带的serialbus模块中，它是基于串口(QModbusRtuSerialMaster)或者TCP协议(QModbusTcpClient)进行通信的应用层协议。<br>
如果在开发上位机，并且需要基于串口模式，而PC机通常没有串口，此时可以用一个USB转串口的适配设备完成物理链路。<br>
<img width="759" height="560" alt="image" src="https://github.com/user-attachments/assets/dfa70139-bdda-4033-a971-2135d4a5b935" /><br>
从站可以安装Modbus Slave进行配套测试。<br>
<img width="570" height="645" alt="image" src="https://github.com/user-attachments/assets/04002ede-76d1-4c8d-9562-8d4058790231" /><br>
个人博客上分享有详细的介绍 https://blog.csdn.net/liangyuna8787/article/details/170243427?spm=1001.2014.3001.5502
# serial
  工业设备用RS-232/485串口进行组网通信的比较多，QT中有专门的serialport模块(QSerialPort)实现与工业设备进行数据通信。<br>
  <img width="1075" height="635" alt="b131e68a-3dbf-4051-9412-fede35e47f5b" src="https://github.com/user-attachments/assets/bb9e4014-bce0-417c-b87a-81a8627146d6" /><br>
<img width="1080" height="625" alt="4d3e1c4d-2ddf-4087-84e1-f584e764852c" src="https://github.com/user-attachments/assets/227f7dcf-bf08-478d-87f2-b7c6acc7bc97" /><br>
## 串口模拟工具vspd
  为了方便测试，vspd可以在windows系统上创建一对COM串口，方便进行代码调试。<br>
<img width="752" height="495" alt="image" src="https://github.com/user-attachments/assets/bd2a34c4-934c-424f-af48-9fd2859fe697" /><br>
## windows自带的串口工具【串口调试助手】
  开启vspd创建一对虚拟串口，就可以使用serial目录中的应用程序可以打开两个来联调测试，也可以和【串口调试助手】联调测试。<br>
<img width="1100" height="841" alt="image" src="https://github.com/user-attachments/assets/b3544ed5-b46a-4053-ba5f-5692a8bfbcf9" /><br>
个人博客上分享有详细的介绍 https://blog.csdn.net/liangyuna8787/article/details/152811310?spm=1001.2014.3001.5502<br>
另外，你有需要通过串口侦听第三方公司的设备，抓取报文时，可以看看我的技术博客分享：【DIY】RS232串口监听头制作<br>
https://blog.csdn.net/liangyuna8787/article/details/153656587?spm=1001.2014.3001.5502<br>
<img width="905" height="922" alt="image" src="https://github.com/user-attachments/assets/980bcdb6-e0cf-46e3-9b36-e5641f2c4d86" /><br>
<img width="431" height="474" alt="image" src="https://github.com/user-attachments/assets/31e342b3-3c73-4fc6-8651-b210a7a9e02b" />
<img width="381" height="375" alt="image" src="https://github.com/user-attachments/assets/7d231fa2-baee-4559-8750-3029473d41dc" />

# CAN
串行总线协议，与Modbus一样，在QT自带的serialbus模块中，主要是QCanBus、QCanBusDevice、QCanBusFrame这3个子模块。项目can目录是QT自带的案例。<br>
## 使用场景
在做上位机应用时，通常是通过转换适配器（以太网-CAN 网关、USB-CAN）实现物理链路，与Modbus的串口模式类似。<br>
<img width="515" height="434" alt="image" src="https://github.com/user-attachments/assets/48190385-3e49-4cae-8ec9-e13aeee13878" /><br>
# mqtt
 mqtt是一个订阅/发布模式的存储/转发职能的网络中间件，它包含broket服务端和自定义开发客户端，可以下载开源的Mosquitto，支持匿名/账号方式。<br>
 ## 使用场景
 如果采集设备本身支持以太网协议，是可以主动把数据推送给mqtt的，如果是采集设备数据只支持前面3种（modbus、serial、can）直接链路协议，可以增加一个专用网关统一采集数据之后，再推送给MQTT，我分享的另外一个项目就是需要网关设备进行统一采集的物联网项目，https://github.com/ocy-org/IoT-Switch<br>
 项目mqtt目录，是使用了开源的qtmqtt编译的库进行开发的，可订阅和推送，以下图片是与IoT-Switch联调的效果<br>
 <img width="940" height="694" alt="0a072bb4ad47c35b37f5de59eb634f8c" src="https://github.com/user-attachments/assets/95672729-02b7-4097-98c8-ecd1d83775bc" />
 个人博客上分享有mqtt的详细介绍 https://blog.csdn.net/liangyuna8787/article/details/166465769?spm=1001.2014.3001.5502
