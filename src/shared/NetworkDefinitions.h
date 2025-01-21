#pragma once

using ClientID = int;
using PortNumber = unsigned short;

enum class Network {
    HighestTimestamp = 2147483647, ClientTimeout = 10000, ServerPort = 5600,
    PlayerUpdateDelim = -1, NullID = -1
};