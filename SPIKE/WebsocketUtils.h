#pragma once
#include <windows.h>
#include <wincrypt.h>
#include <string>
#pragma comment(lib, "Crypt32.lib")

// inputs: clientKey is the Sec-WebSocket-Key header value (ASCII)
// mechanism: SHA-1 hash of (clientKey + GUID), base64-encoded
// accept = Base64Encode(SHA1( clientKey + "258EAFA5-E914-47DA-95CA-C5AB0DC85B11" ))
inline std::string ComputeWebSocketAccept(const std::string& clientKey)
{
    static const char* GUID = "258EAFA5-E914-47DA-95CA-C5AB0DC85B11";
    std::string concat = clientKey + GUID;

    HCRYPTPROV hProv = 0;
    HCRYPTHASH hHash = 0;
    BYTE sha1Digest[20] = { 0 };
    DWORD cbDigest = sizeof(sha1Digest);

    if (!CryptAcquireContextA(&hProv, NULL, NULL, PROV_RSA_FULL, CRYPT_VERIFYCONTEXT)) 
    {
        // handle error
        return {};
    }
    if (!CryptCreateHash(hProv, CALG_SHA1, 0, 0, &hHash)) 
    {
        CryptReleaseContext(hProv, 0);
        return {};
    }
    if (!CryptHashData(hHash, reinterpret_cast<const BYTE*>(concat.data()), (DWORD)concat.size(), 0)) 
    {
        CryptDestroyHash(hHash);
        CryptReleaseContext(hProv, 0);
        return {};
    }
    if (!CryptGetHashParam(hHash, HP_HASHVAL, sha1Digest, &cbDigest, 0)) 
    {
        CryptDestroyHash(hHash);
        CryptReleaseContext(hProv, 0);
        return {};
    }

    CryptDestroyHash(hHash);
    CryptReleaseContext(hProv, 0);

    // Base64 encode
    DWORD base64Len = 0;
    if (!CryptBinaryToStringA(sha1Digest, cbDigest, CRYPT_STRING_BASE64 | CRYPT_STRING_NOCRLF, NULL, &base64Len)) 
    {
        return {};
    }
    std::string base64;
    base64.resize(base64Len);
    if (!CryptBinaryToStringA(sha1Digest, cbDigest, CRYPT_STRING_BASE64 | CRYPT_STRING_NOCRLF, &base64[0], &base64Len)) 
    {
        return {};
    }
    // CryptBinaryToStringA returns a null-terminated string length in base64Len; trim if needed
    if (!base64.empty() && base64.back() == '\0') base64.pop_back();
    return base64;
}