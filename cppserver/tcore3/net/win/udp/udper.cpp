#include "udper.h"
namespace tcore {
    tlib::tpool<udper> g_udper_pool;

    udper::udper(iUdpSession * session, const std::string & ip, const s32 port)
        : _session(session),
        _socket(INVALID_SOCKET),
        _is_cache(false), _is_recving(false), _recv_ex(eCompletion::recved, this) {
        _recv_ex._wbuf.buf = _recv_temp;
        _recv_ex._wbuf.len = sizeof(_recv_temp);
    }

    udper * udper::create(iUdpSession * session, const std::string & ip, const s32 port) {
        BOOL bNewBehavior = FALSE;
        DWORD dwBytesReturned = 0;

        DWORD ul = 1;
        int len = 0;
         LINGER linger = { 1,0 };
        int reuse = 1;
        u32 socket = INVALID_SOCKET;

        if (INVALID_SOCKET == (socket = WSASocketW(AF_INET, SOCK_DGRAM, IPPROTO_UDP, nullptr, 0, WSA_FLAG_OVERLAPPED))
            || SOCKET_ERROR == setsockopt(socket, SOL_SOCKET, SO_REUSEADDR, (const char *)&reuse, sizeof(reuse))
            || SOCKET_ERROR == setsockopt(socket, SOL_SOCKET, SO_SNDBUF, (const char *)&len, sizeof(len))
            || SOCKET_ERROR == setsockopt(socket, SOL_SOCKET, SO_RCVBUF, (const char *)&len, sizeof(len))
            || SOCKET_ERROR == ioctlsocket(socket, FIONBIO, &ul)
            || SOCKET_ERROR == WSAIoctl(socket, SIO_UDP_CONNRESET, &bNewBehavior, sizeof(bNewBehavior), nullptr, 0, &dwBytesReturned, nullptr, nullptr)) {
            close_socket(socket);
            tassert(false, "wtf");
            return nullptr;
        }

        udper * udp = create_from_pool(g_udper_pool, session, ip, port);
        udp->_socket = socket;

        sockaddr_in binder;
        if (0 > inet_pton(AF_INET, ip.c_str(), (void *)&binder.sin_addr.s_addr)) {
            tassert(false, "udper inet_pton remote ip %s error", ip.c_str());
            recover_to_pool(g_udper_pool, udp);
            return nullptr;
        }

        binder.sin_family = AF_INET;
        binder.sin_port = htons(port);

        if (0 != ::bind(udp->_socket, (const sockaddr *)&binder, sizeof(binder))) {
            tassert(false, "udper bind port %d error %d", port, ::GetLastError());
            return false;
        }

        if (g_complate_port != CreateIoCompletionPort((HANDLE)udp->_socket, g_complate_port, udp->_socket, 0)) {
            tassert(false, "udper CreateIoCompletionPort error %d", ::GetLastError());
            recover_to_pool(g_udper_pool, udp);
            return nullptr;
        }

        if (!udp->asyncRecv()) {
            tassert(false, "udper asyncRecv error");
            recover_to_pool(g_udper_pool, udp);
            return nullptr;
        }

        return udp;
    }

    void udper::release() {
        recover_to_pool(g_udper_pool, this);
    }

    void udper::cache() {
        _is_cache = true;
    }

    void udper::load() {
        _is_cache = false;
    }

    void udper::close() {
        close_socket(_socket);
        _socket = INVALID_SOCKET;
        if (!_is_recving) {
            tools::memery::safeMemset((void *)&_session->_pipe, sizeof(_session->_pipe), 0, sizeof(_session->_pipe));
            _session->onClose(core::getInstance());
            release();
        }
    }

    void udper::sendto(const char * ip, const s32 port, const void * data, const s32 size) {
        sockaddr_in remote;
        remote.sin_family = AF_INET;
        remote.sin_port = htons(port);
        inet_pton(AF_INET, ip, (void *)&remote.sin_addr.s_addr);
        ::sendto(_socket, (const char *)data, size, 0, (const sockaddr *)&remote, sizeof(remote));
    }

    void udper::connect(const char * ip, const s32 port) {
        struct sockaddr_in remote;
        remote.sin_family = AF_INET;
        remote.sin_port = htons(port);
        remote.sin_addr.s_addr = inet_addr(ip);
        ::connect(_socket, (struct sockaddr*)&remote, sizeof(remote));
    }

    void udper::onCompleter(overlappedex * ex, const eCompletion type, const s32 code, const s32 size) {
        //tassert(type == eCompletion::recved, "udper oncompleter type error %d", type);
        switch (type) {
        case eCompletion::recved: {
            _is_recving = false;
            if (0 == code) {
                _session->onRecv(core::getInstance(), inet_ntoa(_recv_ex._remote.sin_addr), ntohs(_recv_ex._remote.sin_port), _recv_temp, size);

                if (!asyncRecv()) {
                    close();
                }
            } else {
                close();
            }

            break;
        }
        }
    }

    bool udper::asyncRecv() {
        WSASetLastError(0);
        DWORD bytes = 0, flag = 0;
        s32 addrlen = sizeof(sockaddr);
        tools::memery::safeMemset(&_recv_ex._ol, sizeof(_recv_ex._ol), 0, sizeof(_recv_ex._ol));
        if (WSARecvFrom(_socket, &_recv_ex._wbuf, 1, &bytes, &flag, (sockaddr*)&_recv_ex._remote, &addrlen, (LPWSAOVERLAPPED)&_recv_ex, nullptr) == SOCKET_ERROR) {
            s32 res = WSAGetLastError();
            if (res != WSA_IO_PENDING) {
                return false;
            }
        }

        _is_recving = true;
        return true;
    }
}
