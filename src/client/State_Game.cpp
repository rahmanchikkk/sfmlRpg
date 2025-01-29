#include "State_Game.h"
#include "StateManager.h"
#include "EntitySnapshot.h"

State_Game::State_Game(StateManager* l_stateMgr) : BaseState(l_stateMgr), m_player(-1), m_client(m_stateManager->GetContext()->m_client),
    m_gameMap(nullptr) {}

State_Game::~State_Game() {}

void State_Game::OnCreate() {
    m_client->Setup(&State_Game::HandlePacket, this);
    if (m_client->Connect()) {
        m_stateManager->GetContext()->m_systemManager->GetSystem<S_Network>(System::Network)->SetClient(m_client);
        EventManager* evMgr = m_stateManager->GetContext()->m_eventManager;
        evMgr->AddCallback(StateType::Game, "Key_Escape", &State_Game::MainMenu, this);
        evMgr->AddCallback(StateType::Game, "Key_O", &State_Game::ToggleOverlay, this);
        evMgr->AddCallback(StateType::Game, "Player_MoveLeft", &State_Game::PlayerMove, this);
        evMgr->AddCallback(StateType::Game, "Player_MoveRight", &State_Game::PlayerMove, this);
        evMgr->AddCallback(StateType::Game, "Player_MoveUp", &State_Game::PlayerMove, this);
        evMgr->AddCallback(StateType::Game, "Player_MoveDown", &State_Game::PlayerMove, this);
        evMgr->AddCallback(StateType::Game, "Player_Attack", &State_Game::PlayerAttack, this);

        sf::Vector2u size = m_stateManager->GetContext()->m_window->GetWindowSize();
        m_view.setSize(sf::Vector2f(size));
        m_view.setCenter(size.x / 2, size.y / 2);
        m_view.zoom(0.6f);

        m_gameMap = new Map(m_stateManager->GetContext());
        m_gameMap->LoadMap("media/Maps/map1.map");

        m_stateManager->GetContext()->m_systemManager->GetSystem<S_Movement>(System::Movement)->SetMap(m_gameMap);
        m_stateManager->GetContext()->m_systemManager->GetSystem<S_Collision>(System::Collision)->SetMap(m_gameMap);
        m_stateManager->GetContext()->m_soundManager->PlayMusic("TownTheme", 50.f, true);
    } else {
        std::cout << "Failed to connect to server." << std::endl;
        m_stateManager->Remove(StateType::Game);
        m_stateManager->SwitchTo(StateType::MainMenu);
    }
}

void State_Game::OnDestroy() {
    m_client->Disconnect();
    m_client->UnregisterPacketHandler();
    S_Network* net = m_stateManager->GetContext()->m_systemManager->GetSystem<S_Network>(System::Network);
    net->SetClient(nullptr);
    net->SetPlayerId((int)Network::NullID);
    net->ClearSnapshots();
    m_stateManager->GetContext()->m_entityManager->Purge();
    EventManager* evMgr = m_stateManager->GetContext()->m_eventManager;
    evMgr->RemoveCallback(StateType::Game, "Key_Escape");
    evMgr->RemoveCallback(StateType::Game, "Key_O");
    evMgr->RemoveCallback(StateType::Game, "Player_MoveUp");
    evMgr->RemoveCallback(StateType::Game, "Player_MoveDown");
    evMgr->RemoveCallback(StateType::Game, "Player_MoveLeft");
    evMgr->RemoveCallback(StateType::Game, "Player_MoveRight");
    evMgr->RemoveCallback(StateType::Game, "Player_Attack");
    if (m_gameMap) {
        delete m_gameMap;
        m_gameMap = nullptr;
    }
}

void State_Game::Update(const sf::Time& l_time) {
    if (!m_client->IsConnected()) {
        m_stateManager->Remove(StateType::Game);
        m_stateManager->SwitchTo(StateType::MainMenu);
        return;
    }
    SharedContext* context = m_stateManager->GetContext();
    UpdateCamera();
    {
        sf::Lock lock(m_client->GetMutex());
        context->m_systemManager->Update(l_time.asSeconds());
    }
}

void State_Game::UpdateCamera() {
    if (m_player == (int)Network::NullID) return;
    SharedContext* context = m_stateManager->GetContext();
    C_Position* pos = context->m_entityManager->GetComponent<C_Position>(m_player, Component::Position);
    m_view.setCenter(pos->GetPosition());
    sf::FloatRect viewSpace = context->m_window->GetViewSpace();
    if (viewSpace.left <= 0) {
        m_view.setCenter(viewSpace.width / 2, m_view.getCenter().y);
        context->m_window->GetRenderWindow()->setView(m_view);
    } else if (viewSpace.left + viewSpace.width >= (m_gameMap->GetMapSize().x * Sheet::Tile_Size)) {
        m_view.setCenter((m_gameMap->GetMapSize().x * Sheet::Tile_Size) - (viewSpace.width / 2), m_view.getCenter().y);
        context->m_window->GetRenderWindow()->setView(m_view);
    }
    if (viewSpace.top <= 0) {
        m_view.setCenter(m_view.getCenter().x, viewSpace.height / 2);
        context->m_window->GetRenderWindow()->setView(m_view);
    } else if (viewSpace.top + viewSpace.height >= (m_gameMap->GetMapSize().y * Sheet::Tile_Size)) {
        m_view.setCenter(m_view.getCenter().x, (m_gameMap->GetMapSize().y * Sheet::Tile_Size) - (viewSpace.height / 2));
        context->m_window->GetRenderWindow()->setView(m_view);
    }
    if (context->m_debugOverlay->Debug()) {
        sf::Text* d = new sf::Text();
        d->setFont(*context->m_fontManager->GetResource("Main"));
        d->setString("Player position: " + std::to_string((long long)pos->GetPosition().x) + " " + 
            std::to_string((long long)pos->GetPosition().y));
        d->setCharacterSize(9);
        d->setPosition(context->m_window->GetRenderWindow()->getView().getCenter() - sf::Vector2f(
            context->m_window->GetRenderWindow()->getView().getSize().x, context->m_window->GetRenderWindow()->getView().getSize().y
        ));
        context->m_debugOverlay->Add(d);
    }
}

void State_Game::Draw() {
    if (!m_gameMap) return;
    sf::Lock lock(m_client->GetMutex());
    for (int i = 0; i < Sheet::Num_Layers; ++i) {
        m_gameMap->Draw(i);
        m_stateManager->GetContext()->m_systemManager->Draw(m_stateManager->GetContext()->m_window, i);
    }
}

void State_Game::MainMenu(EventDetails* l_details) {
    m_stateManager->SwitchTo(StateType::MainMenu);
}

void State_Game::Pause(EventDetails* l_details) {
    m_stateManager->SwitchTo(StateType::Paused);
}

void State_Game::Activate() {}
void State_Game::Deactivate() {}

void State_Game::PlayerMove(EventDetails* l_details) {
    Message msg((MessageType)EntityMessage::Move);
    if (l_details->m_name == "Player_MoveLeft") {
        msg.m_int = (int)Direction::Left;
    } else if (l_details->m_name == "Player_MoveRight") {
        msg.m_int = (int)Direction::Right;
    } else if (l_details->m_name == "Player_MoveUp") {
        msg.m_int = (int)Direction::Up;
    } else if (l_details->m_name == "Player_MoveDown") {
        msg.m_int = (int)Direction::Down;
    }
    msg.m_receiver = m_player;
    m_stateManager->GetContext()->m_systemManager->GetMessageHandler()->Dispatch(msg);
}

void State_Game::PlayerAttack(EventDetails* l_details) {
    Message msg((MessageType)EntityMessage::Attack);
    msg.m_receiver = m_player;
    m_stateManager->GetContext()->m_systemManager->GetMessageHandler()->Dispatch(msg);
}

void State_Game::ToggleOverlay(EventDetails* l_details) {
    m_stateManager->GetContext()->m_debugOverlay->SetDebug(!m_stateManager->GetContext()->m_debugOverlay->Debug());
}

void State_Game::HandlePacket(const PacketID& l_pid, sf::Packet& l_packet, Client* l_client) {
    ClientEntityManager* emgr = m_stateManager->GetContext()->m_entityManager;
    PacketType pType = (PacketType)l_pid;
    if (pType == PacketType::Connect) {
        sf::Int32 eid;
        sf::Vector2f pos;
        if (!(l_packet >> eid) || !(l_packet >> pos.x) || !(l_packet >> pos.y)) {
            std::cout << "Failed reading a connect packet!" << std::endl;
            return;
        }
        std::cout << "Adding entity: " << eid << std::endl;
        m_client->GetMutex().lock();
        emgr->AddEntity("Player", eid);
        emgr->GetComponent<C_Position>(eid, Component::Position)->SetPosition(pos);
        m_player = eid;
        m_client->GetMutex().unlock();
        m_stateManager->GetContext()->m_systemManager->GetSystem<S_Network>(System::Network)->SetPlayerId(m_player);
        emgr->AddComponent(eid, Component::SoundListener);
        return;
    }
    if (!m_client->IsConnected()) { return; }
    switch (pType) {
    case PacketType::Snapshot:
    {
        sf::Int32 entityCount;
        if (!(l_packet >> entityCount)) return;
        sf::Lock lock(m_client->GetMutex());
        sf::Int32 t = m_client->GetTime().asMilliseconds();
        for (int i = 0; i < entityCount; ++i) {
            sf::Int32 eid;
            EntitySnapshot snapshot;
            if (!(l_packet >> eid) || !(l_packet >> snapshot)) {
                std::cout << "Snapshot extraction failed!" << std::endl;
                return;
            }
            m_stateManager->GetContext()->m_systemManager->GetSystem<S_Network>(System::Network)->AddSnapshot(eid, t, snapshot);
        }
    }
    break;
    case PacketType::Hurt:
    {
        sf::Int32 eid;
        if (!(l_packet >> eid)) return;
        Message msg((MessageType)EntityMessage::Hurt);
        msg.m_receiver = eid;
        m_stateManager->GetContext()->m_systemManager->GetMessageHandler()->Dispatch(msg);
    }
    break;
    case PacketType::Disconnect:
    {
        std::cout << "Disconnected by a server." << std::endl;
        m_stateManager->Remove(StateType::Game);
        m_stateManager->SwitchTo(StateType::MainMenu);
    }
    break;
    }
}