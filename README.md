# TCPClientPlugin
TCPClient make easy to connect and comunicate with your server via TCP Socket.<br>
You can easily implement all the functions by learning how to use 4 blueprints.<br>
Because of It is only maden by C++17 and unreal libray, any device that supports multi-threading can be used.<br>

### features
- All features can be created using blueprints.
- Only c++ can be used for optimization.
- Connect to the server via TCP Socket
- Supports multiple connections with various servers.
- Support type : bool, int8, int16, int32, int64, float, double(c++ only), String(utf-8) and bytes array.
- Each packet is managed with a Blueprint or C++ Class.
- Not Polling method. It can save cpu usage.

# 1. 4 Basic Classes
![image](https://user-images.githubusercontent.com/108503849/200352327-aadb6ed7-bc99-48d6-9f6d-32f7caf0b46d.png)<br>
- **TCPClientSubsystem** : Subsystem is responsible for creating and destroying sessions. You can get the sessions that you created through this.<br>
- **TCPClientSessionBase** : Session sends and receives packets while maintaining connectivity with the server.<br>
- **TCPRecvPacketBase** : Use this to define the packets that you want to receive from the server.<br>
- **TCPSendPacketBase** : Use this to define the packets to be sent to the server.<br>

# 2. Protocol
## Packet Policy
![packetPolicy](https://user-images.githubusercontent.com/108503849/200338239-f9c90ad4-9d46-40e6-812d-13c37dc71914.png)<br>
You can modify [TCPPacketHeader.h](https://github.com/grpht/TCPClientPlugin/blob/master/Source/TCPClientPlugin/Private/TCPPacketHeader.h) file to change the packet policy.<br>
When you want to add other variable into header, you have to change 'SendPacket' method in [TCPSessionBase.cpp](https://github.com/grpht/TCPClientPlugin/blob/master/Source/TCPClientPlugin/Private/TCPSessionBase.cpp)

## String Policy
![stringPolicy](https://user-images.githubusercontent.com/108503849/200338328-4ec15dec-2a46-4d3c-879f-a4597cd45889.png)<br>
You can modify [TCPBufferReader.cpp](https://github.com/grpht/TCPClientPlugin/blob/master/Source/TCPClientPlugin/Private/TCPBufferReader.cpp) and [TCPBufferWriter.cpp](https://github.com/grpht/TCPClientPlugin/blob/master/Source/TCPClientPlugin/Private/TCPBufferWriter.cpp) files to change the string policy.

# 3. How To Use

> ## Defining Packet
#### 1) Create 'SendPacket' Blueprint
![image](https://user-images.githubusercontent.com/108503849/200339020-42fece0b-70bc-45a2-9e3a-0ebaecfdae02.png)
![image](https://user-images.githubusercontent.com/108503849/200510060-73fc8bbe-c369-47cb-8743-df812ba40532.png)<br>
#### 2) When open blueprint, you can set 'Packet Id' on details panel. It is used to distinguish this packet from other packets.<br>
![image](https://user-images.githubusercontent.com/108503849/200339842-6ec9b2c7-f283-4100-8926-0e7b7f05ec68.png)<br>
#### 3) Add variables as you want
![image](https://user-images.githubusercontent.com/108503849/200340367-681ba094-eccc-42b4-bc85-43bb8ee6ca91.png)<br>
#### 4) Override 'Serialize' function and write bytes of your variable to the packet when it sended
![image](https://user-images.githubusercontent.com/108503849/200340590-dc04a77d-0078-4725-a01f-1486c76e01a4.png)  
![image](https://user-images.githubusercontent.com/108503849/200341146-588da009-329b-4703-87fb-19c0732a116b.png)<br>
#### 5) 'RecvPacket' has same process, but the difference is overriding 'Desereialize' function, and using Reading method to setting your member vairable when bytes are received from server.
![image](https://user-images.githubusercontent.com/108503849/200515824-21b1087e-40dc-46e3-8a1e-f3d0c0738c17.png)


> ## Defining Session
#### 1) Create Session blueprint
![image](https://user-images.githubusercontent.com/108503849/200344093-1decd65c-484f-47b1-9d7d-024da30abe09.png)<br>
#### 2) Open Session blueprint, you can set SessionName, Ip, Port. 'SessionName' is required when get the session from Subsystem. So, you must input unique value
![image](https://user-images.githubusercontent.com/108503849/200344987-8c284dbf-a874-4384-bcaf-833a74c99542.png)<br>
#### 3) Override 'OnStart', and call 'RegisterRecvPacket' to register what you created packet. then it's instance is thrown automatically when it receive bytes from server via 'OnRecvPacket' callback method.
![image](https://user-images.githubusercontent.com/108503849/200346180-d154b96d-0024-4a3b-978a-166459c6d215.png)<br>
#### 4) Override 'OnRecvPacket' and divide it by ID and handle it appropriately, I will recommand you to using Dispatcher
![image](https://user-images.githubusercontent.com/108503849/200347348-4cdc19d6-cefe-486c-95ed-09f93844d446.png)<br>

> ## Connecting Session
#### 1) Connect the Session using 'ConnectSession' method of 'TCPClientSubsystem'. This process looks complicated, but it is standardized thus can be used as it is.
![image](https://user-images.githubusercontent.com/108503849/200348793-8294f34b-6a41-437f-9e54-3651ed43c833.png)

> ## Get Session
#### 1) You can achieve Session from 'TCPClientSubsystem' by SessionName
![image](https://user-images.githubusercontent.com/108503849/200349650-70b56a92-ba03-4aad-9351-2523c65b50b6.png)
#### 2) However you cannot know when sesssion is connected, but you want to use this after connected, try to use 'GetSessionLazy'
![image](https://user-images.githubusercontent.com/108503849/200350244-0c3683e1-2905-4f93-b15f-b70d7e182d96.png)

> ## Sending Packet
#### 1) You can use 'CreateSendPacket' method to instantiate SendPacket, and set member after casting it.
#### 2) Call 'SendPacket' method of Session. Then packet is sended to server that relative with session
![image](https://user-images.githubusercontent.com/108503849/200351028-988f0171-4beb-4ce6-88eb-cec58a60ab8c.png)

# 4. Life Cycle of Session 
![image](https://user-images.githubusercontent.com/108503849/200497378-5dfc9123-c077-4a71-8d7d-7e119b843b2f.png)

