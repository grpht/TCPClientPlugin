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
- **TCPClientSessionBase** : The session manages communication with the connected server.<br>
- **TCPRecvPacketBase** : It defines the packets to be received from the server.<br>
- **TCPSendPacketBase** : It defines the packets to be sent to the server.<br><br>
![image](https://github.com/grpht/TCPClientPlugin/assets/108503849/8c426018-fc6f-4414-aaaa-edc377a895cc)<br>

# 2. How To Use
> ## Step.1 - Defining Session
<details>
  <summary> Expand </summary>

  ### 1-1) Create Session blueprint in Content Drawer
  ![image](https://user-images.githubusercontent.com/108503849/200344093-1decd65c-484f-47b1-9d7d-024da30abe09.png)<br>
  
  ### 1-2) Open session blueprint, click 'class defaults' and setting session in details pannel
  ![image](https://github.com/grpht/TCPClientPlugin/assets/108503849/116f7b49-5468-426b-a9e9-da8d4f75c308)<br>
  
  - **Session Name** : 'SessionName' is required when get the session from Subsystem. So, you must input unique value
  - **Ip** : IP Address of server. If you check DNS, it will be treated as a DNS Address
  - **Port** : Port of server
  - **Custom Header** : You can set a custom header. If set to None, the default settings will be applied.
  - **packet to Receive** : Register your created RecvPacket. This will enable you to convert and respond to the byte messages received from the server.


    
  ### 1-3) Override 'OnRecvPacket' in Event Graph and divide it by PacketId and handle it appropriately
  ![image](https://user-images.githubusercontent.com/108503849/200347348-4cdc19d6-cefe-486c-95ed-09f93844d446.png)<br>
</details>

> ## Step.2 Defining RecvPacket
<details>
  <summary> Expand </summary>
  
  ### 2-1) Create 'RecvPacket' Blueprint in Content Drawer
  ![image](https://github.com/grpht/TCPClientPlugin/assets/108503849/193b96b2-031b-4b26-ba36-729ee46ec8ec)<br>
  ![image](https://github.com/grpht/TCPClientPlugin/assets/108503849/b829e7f7-bacf-41ac-b9a9-734ca3c3afd4)
  
  ### 2-2) Open blueprint, and set 'Packet Id' on details pannel
  ![image](https://github.com/grpht/TCPClientPlugin/assets/108503849/3f15c170-4e39-4cbe-970b-0f65f1136c19)
  - The session converts to the RecvPacket that matches this Id
  
  ### 2-3) Add variable as you want
  ![image](https://github.com/grpht/TCPClientPlugin/assets/108503849/9d121b35-576e-44e4-bd53-bae221fa39f4)
  
  ### 2-4) Override 'Desereialize' function, This defines how to convert the byte message received from the server.
  ![image](https://user-images.githubusercontent.com/108503849/200515824-21b1087e-40dc-46e3-8a1e-f3d0c0738c17.png)
  
  ### 2-5) Open the session and register this packet on details pannel
  ![image](https://github.com/grpht/TCPClientPlugin/assets/108503849/a3eef261-bb39-4b85-8640-99395a2a58ba)
</details>

> ## Step.3 Defining SendPacket
<details>
  <summary> Expand </summary>
  
  ### 3-1) Create 'SendPacket' Blueprint in Content Drawer
  ![image](https://user-images.githubusercontent.com/108503849/200339020-42fece0b-70bc-45a2-9e3a-0ebaecfdae02.png)<br>
  ![image](https://user-images.githubusercontent.com/108503849/200510060-73fc8bbe-c369-47cb-8743-df812ba40532.png)<br>
  
  ### 3-2) Open blueprint, you can set 'Packet Id' on details panel.
  ![image](https://user-images.githubusercontent.com/108503849/200339842-6ec9b2c7-f283-4100-8926-0e7b7f05ec68.png)<br>
  - This is the Id of the packet you will send, and it will be included in the header
  
  ### 3-3) Add variables as you want
  ![image](https://user-images.githubusercontent.com/108503849/200340367-681ba094-eccc-42b4-bc85-43bb8ee6ca91.png)<br>
  
  ### 3-4) Override 'Serialize' function. it defines how to convert the message into bytes.
  ![image](https://user-images.githubusercontent.com/108503849/200340590-dc04a77d-0078-4725-a01f-1486c76e01a4.png)  
  ![image](https://user-images.githubusercontent.com/108503849/200341146-588da009-329b-4703-87fb-19c0732a116b.png)<br>
  
  ### 3-5) Use it in other blueprint
  ![image](https://github.com/grpht/TCPClientPlugin/assets/108503849/3495c341-6a8e-4d26-879f-e2b29786b38c)
  - You can use 'CreateSendPacket' method to instantiate SendPacket, and set member after casting it.
  - Call 'SendPacket' method of Session. Then packet is sended to server that relative with session 
</details>

> ## Step.4 Connecting Session
<details>
  <summary> Expand </summary>
  
  ### 4-1) Call 'ConnectSession' in TCPClientSubsystem, and set the session to the one you have created.
  ![image](https://github.com/grpht/TCPClientPlugin/assets/108503849/71836ef4-131c-4d3c-bd60-170ac2b7a5b6)
  
  ### 4-2) Through the Connected Delegate, ensure that the next steps can be taken upon connection.
  ![image](https://github.com/grpht/TCPClientPlugin/assets/108503849/cd44a314-568c-4046-a8c9-f8caaa5c8cc1)
  -  The same applies to the Disconnected Delegate.
</details>

> ## Step.5 Get Session
<details>
  <summary> Expand </summary>
    
  #### 5-1) Now, you can obtain the session connected to the server anytime and anywhere through the TCPClientSubsystem's 'GetSession' method.
  ![image](https://user-images.githubusercontent.com/108503849/200349650-70b56a92-ba03-4aad-9351-2523c65b50b6.png)
  - Communicate with the server through this connected session
    
  #### 5-2) If you need to obtain a session whose connection status is uncertain, use the 'GetSessionLazy' method
  ![image](https://github.com/grpht/TCPClientPlugin/assets/108503849/a5cf836a-371e-4de9-a733-5bb5ce7dcbaa)
</details>

# 3. Life Cycle of Session 
![image](https://user-images.githubusercontent.com/108503849/200497378-5dfc9123-c077-4a71-8d7d-7e119b843b2f.png)



# 4. Policy
## 4-1. Default Header Policy
![packetPolicy](https://user-images.githubusercontent.com/108503849/200338239-f9c90ad4-9d46-40e6-812d-13c37dc71914.png)<br>
Inherit from [TCPHeaderComponent.h](https://github.com/grpht/TCPClientPlugin/blob/master/Source/TCPClientPlugin/Public/TCPHeaderComponent.h) to configure a header suitable for your server environment.<br>
Refer to the comments in the code for more details.

## 4-2. String Policy
### 1) Length-Prefixed method for Write strings. (WriteStringUTF8)
Serialize the 2-byte number representing the byte length of the string, followed by the string itself.<br>
![stringPolicy](https://user-images.githubusercontent.com/108503849/200338328-4ec15dec-2a46-4d3c-879f-a4597cd45889.png)<br>

### 2) Raw bytes of string (WriteStringUTF8Raw)
![image](https://github.com/grpht/TCPClientPlugin/assets/108503849/3f65fdff-a09e-4921-b8b6-8af91f1a6d73)<br>

### 3) Null-Terminated method for Write strings. (WriteStringUTF8NT)
null character at the end of the string.<br>
![image](https://github.com/grpht/TCPClientPlugin/assets/108503849/54fde2b5-0891-4662-a6ef-890cbf41b250)<br>


