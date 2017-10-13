#pragma once

#include <string>

namespace remoteFileExplorer
{
namespace message
{
///////////////////////////////////////////////////////////////////////////////
int SerializeString(
    std::uint8_t*& buffer,
    std::size_t& remainedBufferSize,
    const std::string& str);

///////////////////////////////////////////////////////////////////////////////
int DeserializeString(
    const std::uint8_t*& buffer,
    std::size_t& remainedBufferSize,
    std::string& str);

///////////////////////////////////////////////////////////////////////////////
// Data Representaion / Memory Endian �� ���� �ý��۳����� ��밡��.
template <typename T>
inline int SerializeWithMemcpy(
    std::uint8_t*& buffer,
    std::size_t& remainedBufferSize,
    const T& data)
{
    if (remainedBufferSize < sizeof(data))
        return -1;

    std::memcpy(buffer, &data, sizeof(data));
    buffer += sizeof(data);
    remainedBufferSize -= sizeof(data);

    return 0;
}

///////////////////////////////////////////////////////////////////////////////
// Data Representaion / Memory Endian �� ���� �ý��۳����� ��밡��.
template <typename T>
inline int DeserializeWithMemcpy(
    const std::uint8_t*& buffer,
    std::size_t& remainedBufferSize,
    T& data)
{
    if (remainedBufferSize < sizeof(data))
        return -1;

    std::memcpy(&data, buffer, sizeof(data));
    buffer += sizeof(data);
    remainedBufferSize -= sizeof(data);

    return 0;
}

} // namespace message
} // namespace remoteFileExplorer