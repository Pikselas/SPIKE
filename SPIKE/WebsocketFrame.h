#pragma once
#pragma once
#include <span>
#include <vector>
#include <windows.h>
#include <string_view>

class WebsocketFrame
{
public:
    enum class OPCODE
    {
        CONTINUATION = 0x0,
        TEXT = 0x1,
        BINARY = 0x2,
        CLOSE = 0x8,
        PING = 0x9,
        PONG = 0xA
    };
private:
    OPCODE opcode;
    bool final_frame;
    bool masked;
    std::vector<char> payload;
public:
    OPCODE getOpcode() const { return opcode; }
    bool isFinalFrame() const { return final_frame; }
    const std::vector<char>& getPayload() const { return payload; }
public:
    WebsocketFrame() : opcode(OPCODE::CONTINUATION), final_frame(false), masked(false) {}
    WebsocketFrame(OPCODE op, bool fin, bool mask, std::vector<char> data)
        : opcode(op), final_frame(fin), masked(mask), payload(std::move(data))
    {
    }
public:
    void SetPayload(std::vector<char> data)
    {
        payload = std::move(data);
    }
    void SetFinalFrame(bool fin)
    {
        final_frame = fin;
    }
    void SetOpcode(OPCODE op)
    {
        opcode = op;
    }
    void SetMasked(bool mask)
    {
        masked = mask;
    }
public:
    std::vector<char> ToBytes() const
    {
        std::vector<char> frame;
        char first_byte = 0;
        if (final_frame) first_byte |= 0b10000000; // set MSB if final frame
        first_byte |= static_cast<char>(opcode) & 0b00001111; // set opcode in last 4 bits
        frame.push_back(first_byte);
        char second_byte = 0;
        if (masked) second_byte |= 0b10000000; // set MSB if masked
        std::uint64_t payload_len = payload.size();
        if (payload_len <= 125)
        {
            second_byte |= static_cast<char>(payload_len) & 0b01111111; // set payload length in last 7 bits
            frame.push_back(second_byte);
        }
        else if (payload_len <= 65535)
        {
            second_byte |= 126; // 126 indicates that the next 2 bytes are the payload length
            frame.push_back(second_byte);
            std::uint16_t len16 = htons(static_cast<std::uint16_t>(payload_len));
            frame.push_back((len16 >> 8) & 0xFF); // high byte
            frame.push_back(len16 & 0xFF); // low byte
        }
        else
        {
            second_byte |= 127; // 127 indicates that the next 8 bytes are the payload length
            frame.push_back(second_byte);
            std::uint64_t len64 = htonl(payload_len);
            for (int i = 7; i >= 0; --i)
            {
                frame.push_back((len64 >> (i * 8)) & 0xFF);
            }
        }
        char masking_key[4] = { 0, 0, 0, 0 };
        if (masked)
        {
            // Generate a random masking key
            for (int i = 0; i < 4; ++i)
            {
                masking_key[i] = static_cast<char>(rand() % 256);
                frame.push_back(masking_key[i]);
            }
        }
        // Append payload data, masking if necessary
        for (size_t i = 0; i < payload.size(); ++i)
        {
            char byte = payload[i];
            if (masked)
            {
                byte ^= masking_key[i % 4]; // mask the byte
            }
            frame.push_back(byte);
        }
        return frame;
    }
public:
    static WebsocketFrame ConstructPongFrame(std::vector<char> payload)
    {
        return WebsocketFrame(OPCODE::PONG, true, false, std::move(payload));
    }
    static WebsocketFrame ConstructFrom(std::span<char> raw_data)
    {
        WebsocketFrame frame;
        frame.final_frame = (raw_data[0] & 0b10000000); // MSB 7th bit is masked out using 10000000
        // 6-4 bit is not needed
        frame.opcode = static_cast<WebsocketFrame::OPCODE>(raw_data[0] & 0b00001111); // last 4 bits are masked out using 00001111

        frame.masked = (raw_data[0] & 0b10000000);
        std::uint64_t payload_len = (raw_data[1] & 0b01111111); // last 7 bits are payload length
        unsigned int offset = 2; // starts from 3rd byte 

        // if <= 125, that is the length
        if (payload_len <= 125)
        {

        }
        else if (payload_len == 126)
        {
            // next 2 bytes are the length
            // inorder to avoid sign extension, cast to unsigned char first
            // payload_len = (static_cast<unsigned char>(raw_data[2]) << 8) | static_cast<unsigned char>(raw_data[3]);
            std::uint16_t p_len = 0;
            std::memcpy(&p_len, &raw_data[2], sizeof(p_len));
            payload_len = ntohs(p_len);
            offset += 2; // total 16 bits of payload length
        }
        else if (payload_len == 127)
        {
            // next 8 bytes are the length
            std::memcpy(&payload_len, &raw_data[2], sizeof(payload_len));
            payload_len = ntohs(payload_len);
            offset += 8; // total 64 bits of payload length
        }

        char masking_key[4] = { raw_data[offset] ,  raw_data[offset + 1] , raw_data[offset + 2] , raw_data[offset + 3] };

        raw_data = raw_data.subspan(offset + 4, payload_len); // move to payload data
        frame.payload.resize(payload_len);

        for (int i = 0; i < payload_len; ++i)
        {
            // bytes are masked using: payload_byte[i] = original_byte[i] XOR masking_key[i % 4]
            frame.payload[i] = raw_data[i] ^ masking_key[i % 4];
        }

        return frame;
    }
    static WebsocketFrame ConstructTextFrame(std::string_view str)
    {
        return WebsocketFrame(OPCODE::TEXT, true, false, std::vector<char>(str.begin(), str.end()));
    }
};