#pragma once
#include "C_Base.h"
#include "NetworkDefinitions.h"

class C_Client : public C_Base {
public:
    C_Client() : C_Base(Component::Client), m_client((ClientID)Network::NullID) {};
    void ReadIn(std::stringstream& l_stream) {};

    ClientID GetClient() { return m_client; };
    void SetClient(const ClientID& l_client) { m_client = l_client; };
private:
    ClientID m_client;
};