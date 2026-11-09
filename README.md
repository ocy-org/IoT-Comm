"# IoT-Comm" 
# modbus
  Modbus协议是工业领域通用的开放式通信协议‌，核心用于不同设备间的标准化数据交互，覆盖从底层设备控制到上层平台接入的全链路场景，适用于物联网下设备的数据采集。<br>
项目modbus目录中的代码是QT的主站（modbus client）案例，这个案例已包含常用的4种数据区。它是基于串口(QModbusRtuSerialMaster)或者TCP协议(QModbusTcpClient)进行通信的应用层协议。<br>
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
个人博客上分享有详细的介绍 https://blog.csdn.net/liangyuna8787/article/details/152811310?spm=1001.2014.3001.5502
