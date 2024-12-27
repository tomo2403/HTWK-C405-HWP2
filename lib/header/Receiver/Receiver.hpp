#pragma once

#include "../AtomicQueue.hpp"
#include "../InterthreadNotification.hpp"
#include "../Decoder.hpp"
#include "../DataPacketDisassembler.hpp"
#include "../IDecoderObserver.hpp"
#include "../IControlPacketDisassemblerObserver.hpp"
#include "../ControlPacketDisassembler.hpp"


class ControlPanel;

class Receiver final : public IDecoderObserver, public IControlPacketDisassemblerObserver
{
    AtomicQueue<uint8_t>* datastreamQueue_incoming;
    AtomicQueue<InterthreadNotification>* notificationQueue_outgoing;
    std::atomic<bool>* running;
    ControlPanel* cp;

    Decoder decoder;
    DataPacketDisassembler dataPacketDisassembler;
    ControlPacketDisassembler controlPacketDisassembler;

    bool connectionEstablished;
    uint32_t nextPacketToBeReceived_id;

    void controlBlockReceived(const std::vector<uint8_t> &dataVector) const;
    void dataBlockReceived(const std::vector<uint8_t> &dataVector);

public:
    Receiver(AtomicQueue<uint8_t>* datastreamQueue_incoming, AtomicQueue<InterthreadNotification>* notificationQueue_outgoing, std::atomic<bool>* running, ControlPanel* cp);

    [[nodiscard]] std::vector<uint8_t> getOutputData();

    // IDecoderObserver

    void beginBlockReceived(const BlockType &blockType) override;
	void endBlockReceived(const BlockType &blockType, const std::vector<uint8_t> &dataVector) override;

    // IControlPacketDisassemblerObserver

    void on_received_received(const uint32_t &id) override;
    void on_resend_received(const uint32_t &id) override;
    void on_transferFinished_received() override;
    void on_closeConnection_received() override;
    void on_connect_received() override;
    void on_packetCorrupt() override;

    // to start in another thread
    void receive();
};
