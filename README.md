# TCPClientPlugin


## 4 Base Classes
![core4file](https://user-images.githubusercontent.com/108503849/194360136-bee68665-87c6-49b0-9821-4aef28b97047.png)<br>
**TCPClientSubsystem** : This Class is responsible for the life cycle of the session. You can connect and acquire sessions through this.<br>
**TCPClientSessionBase** : This class is a base class of all custom sessions. You can Implement your custom session by inherit this class.<br>
**TCPRecvPacketBase** : This class provides the default form for RecvPacket. The file contains an interface for Native C++ and a UObject for implementing Blueprints.<br>
**TCPSendPacketBase** : This class provides the default form for SendPacket. The file contains an interface for Native C++ and a UObject for implementing Blueprints.<br>

## How to Use TCPClientPlugin

> ## Session
The client connects to the server through a session and sends or receives messages. One session is associated with one connect, but You can have multiple sessions as you want.

### Create Session
1. in order to create custom session, make child class of **TCPSessionBase**<br>
![createSession1](https://user-images.githubusercontent.com/108503849/194365801-208d3303-12b3-46d2-8f44-a4ebe1d95a79.png)

2. You can implement logic in the session you created. (See the example source for more information)
3. Create blueprint class derived from custom class<br>
![66448f73-10b4-4767-a430-e72d7d727a26](https://user-images.githubusercontent.com/108503849/194366701-33f98915-77a7-4359-9712-0993ed61f364.png)
4. Open blueprint class and write down the information in Session Settings. Session names separate multiple sessions. So it has to be the unique key.<br>
![f81e8084-881b-4818-acf6-a905136b0803](https://user-images.githubusercontent.com/108503849/194366925-6a547bf5-94d7-45ed-b933-282398fb796b.png)

### Connect Session
1. All sessions are managed centrally by **TCPClientSubsystem**. So, connect the your session through **TCPClientSubsystem.ConnectSession**<br>
2. Set events that are called when connected and when disconnected
![fee4b06d-52ca-40c8-86fe-730649a57040](https://user-images.githubusercontent.com/108503849/194368694-f15dc811-b72d-49a4-b353-466ddd7e1855.png)


### Aquire Session
1. you can acquire a instance of session through **TPClientSubsystem.GetSession** with the key you previously set. <br>
![a5b8ec56-81ed-455b-b183-d5f9a86c21a6](https://user-images.githubusercontent.com/108503849/194370847-0436a9b2-8b9e-4783-9187-14647eb6ec25.png)
2. However, **TPClientSubsystem.GetSessionLazy** should be used when the session's network connection cannot be guaranteed initially. (We recommend that you also process Unbind for Binded events.)<br>
![9e6ffb6b-f2bf-4d1c-8b7d-f19c94ebc41d](https://user-images.githubusercontent.com/108503849/194371936-23720b04-599e-4cb6-9949-fb378d7f5f7e.png)
