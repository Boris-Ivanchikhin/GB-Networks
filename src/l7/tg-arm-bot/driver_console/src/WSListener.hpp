#pragma once

#include <oatpp-websocket/ConnectionHandler.hpp>
#include <oatpp-websocket/WebSocket.hpp>
#include <oatpp/web/protocol/http/Http.hpp>


// WebSocket listener listens on incoming WebSocket events.
class WSListener : public oatpp::websocket::WebSocket::Listener
{
public:
    WSListener()
    {}

    // Called on "ping" frame.
    void onPing(const WebSocket& socket, const oatpp::String& message) override;

    // Called on "pong" frame.
    void onPong(const WebSocket& socket, const oatpp::String& message) override;

    // Called on "close" frame.
    void onClose(const WebSocket& socket, v_uint16 code, const oatpp::String& message) override;

    // Called on each message frame. After the last message will be called once-again with size == 0 to designate end of the message.
    void readMessage(const WebSocket& socket, v_uint8 opcode, p_char8 data, oatpp::v_io_size size) override;

private:
    static constexpr const char* TAG = "Client_WSListener";

private:
    // Buffer for messages. Needed for multi-frame messages.
    oatpp::data::stream::BufferOutputStream message_buffer_;
};

