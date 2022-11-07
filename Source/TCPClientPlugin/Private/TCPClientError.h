#pragma once

enum TCPClientError
{
    None = 0,
    Connect_AddressisNotValid,
    Connect_Closed,
    Connect_AlreadyConnected,
    Connect_CallbackIsNull,
    Connect_SockeSubsystemIsNull,
    Send_NotConnected,
    Send_CallbackIsNull,
    Recv_NotConnected,
    Recv_CallbackIsNull,
    Recv_ProhibitDoubleReceiving
};