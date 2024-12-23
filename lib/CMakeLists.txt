add_library(lib STATIC lib.cpp
        implementation/CRC.cpp
        implementation/DataStorage.cpp
        implementation/Decoder.cpp
        implementation/Encoder.cpp
        implementation/Logger.cpp
        implementation/NibbleCompressor.cpp
        implementation/DataPacketAssembler.cpp
        implementation/DataPacketDisassembler.cpp
        implementation/ControlPacketAssembler.cpp
        implementation/ControlPacketDisassembler.cpp
        implementation/InterthreadNotification.cpp
        implementation/Timer.cpp
        implementation/AtomicBoolean.cpp

        implementation/Sender/Sender.cpp
        implementation/Sender/SenderState.cpp
        implementation/Sender/SenderState_ReadyToConnect.cpp
        implementation/Sender/SenderState_Sending.cpp
        implementation/Sender/SenderState_AwaitingResponse.cpp
        implementation/Sender/SenderState_ReadyToCloseConnection.cpp

        implementation/Receiver/Receiver.cpp
        utilities.cpp
)
