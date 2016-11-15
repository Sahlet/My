#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include "WinSocket.h"
#pragma comment(lib, "ws2_32.lib")//подключение библиотеки win_sock_2.0 (как я думаю, для 32 бит)

namespace My {
	
	WinSocketException::WinSocketException(long long errorCode, std::string errorCause, const char* what) : Exception(errorCode, std::move(errorCause), what) {}
	WinSocketException::WinSocketException(long long errorCode, std::string errorCause) : Exception(errorCode, std::move(errorCause), "WinSocketException") {}
	WinSocketException::WinSocketException(std::string errorCause) : Exception(WSAGetLastError(), std::move(errorCause), "WinSocketException") {}

	WinSocketException get_WinSocketException(const std::string& user_cause) {
		auto error_code = WSAGetLastError();
		char *system_cause = nullptr;
		struct ptr_guard {
			char *&ptr;
			ptr_guard(char *&ptr) : ptr(ptr) {}
			~ptr_guard() {
				if (ptr) LocalFree(ptr);
			}
		} system_cause_guard(system_cause);
		FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
			NULL, error_code,
			LANG_SYSTEM_DEFAULT,
			(LPTSTR)&system_cause, 0, NULL
		);

		if (system_cause) {
			if (user_cause.size()) return WinSocketException(error_code, user_cause + " | " + system_cause);
			else return WinSocketException(error_code, std::string(system_cause));
		} else return WinSocketException(error_code, user_cause);
	}

	std::string WinSocketAddress::get_ip() const {
		char* res_ptr = inet_ntoa(ad.sin_addr);

		if (res_ptr) return std::string(res_ptr);
		return std::string();
	}
	unsigned short WinSocketAddress::get_port() const { return ntohs(ad.sin_port); }
	bool WinSocketAddress::operator == (const WinSocketAddress& obj) {
		return (get_port() == obj.get_port()) && (get_ip() == obj.get_ip());
	}
	bool WinSocketAddress::operator < (const WinSocketAddress& obj) {
		return (get_port() < obj.get_port()) || ((get_port() == obj.get_port()) && (get_ip() < obj.get_ip()));
	}

	inline static void init_winSocket() throw (WinSocketException) {
		WORD winSock_version = MAKEWORD(2, 0); // запрашиваем winsock версии 2.0
		WSADATA winSock_data;	// сюда будут помещены данные об инициализированном интерфейсе winsock

		int error = WSAStartup(winSock_version, &winSock_data);	// непосредственно инициализция
		if (error != 0)
			throw get_WinSocketException("initSock: could not initialize winSock.");
	}

	inline static void cleanup_winSocket() throw (WinSocketException) {
		int error = WSACleanup();
		if (error != 0)
			throw get_WinSocketException("cleanSock: could not perform cleaning up.");
	}

	inline static sockaddr_in make_sockaddr_in(const char * const &ip, const unsigned short &port) {
		sockaddr_in ad;
		ad.sin_family = AF_INET;
		if (strcmp(ip, "") == 0) ad.sin_addr.s_addr = INADDR_ANY;
		else ad.sin_addr.s_addr = inet_addr(ip);
		if (ad.sin_addr.s_addr == INADDR_NONE) throw get_WinSocketException("WinSocketAddress: the provided IP address seems to be invalid.");
		ad.sin_port = htons(port);
		return ad;
	}
	
	WinSocketAddress::WinSocketAddress(const char * const &ip, const unsigned short &port) throw (WinSocketException) : ad(make_sockaddr_in(ip, port)) {}
	
	inline static sockaddr_in make_sockaddr_in(const sockaddr_in &addr) {
		if (addr.sin_addr.s_addr == INADDR_NONE) throw get_WinSocketException("WinSocketAddress: the provided IP address seems to be invalid.");
		return addr;
	}
	WinSocketAddress::WinSocketAddress(const sockaddr_in &addr) throw (WinSocketException) : ad(make_sockaddr_in(addr)) {}
	WinSocketAddress::WinSocketAddress(const sockaddr& addr) throw (WinSocketException) : ad(make_sockaddr_in(*(const sockaddr_in*)&addr)) {}


	std::atomic<unsigned long> WinSocket::n(0);
	WinSocket::WinSocket(const int &type) throw (WinSocketException) {
		if (n++ == 0) init_winSocket();
		socket = std::make_shared< SOCKET >(::socket(PF_INET, type, 0));
		if (*socket == SOCKET_ERROR) throw get_WinSocketException("WinSocket: could not create a WinSocket.");
	}
	WinSocket::~WinSocket() {
		if (!socket) return;
		if (this->socket.use_count() == 1) {
			int error = closesocket(*socket); // закрываем сокет
			if (--n == 0) cleanup_winSocket();
		}
	}
	void WinSocket::bind(const WinSocketAddress &address) throw (WinSocketException) {
		int error = ::bind(*socket, address, sizeof(sockaddr_in));
		if (error == SOCKET_ERROR) throw get_WinSocketException("WinSocket::bind: could not bind socket to a provided address.");
	}
	int WinSocket::sendto(const WinSocketAddress &address, const char * data, const int &len) throw (WinSocketException) {
		int error = ::sendto(*socket, data, len, 0, address, sizeof(sockaddr_in));
		if (error == SOCKET_ERROR) throw get_WinSocketException("WinSocket::sendto: could not send data.");
		return error;
	}
	int WinSocket::recvfrom(WinSocketAddress &address, char* data, const int &len) throw (WinSocketException) {
		int address_len = sizeof(sockaddr_in);
		sockaddr addr;
		int error = ::recvfrom(*socket, data, len, 0, &addr, &address_len);
		if (error == SOCKET_ERROR || address_len != sizeof(sockaddr_in)) throw get_WinSocketException("WinSocket::recvfrom: could not receive data.");
		address = WinSocketAddress(addr);
		return error;
	}
	int WinSocket::recvfrom(char * data, const int &len) throw (WinSocketException) {
		int error = ::recvfrom(*socket, data, len, 0, nullptr, nullptr);
		if (error == SOCKET_ERROR) throw get_WinSocketException("WinSocket::recvfrom: could not receive data.");
		return error;
	}
	int WinSocket::send(const char * data, const int &len) throw (WinSocketException) {
		int error = ::send(*socket, data, len, 0);
		if (error == SOCKET_ERROR) throw get_WinSocketException("WinSocket::send: could not send data.");
		return error;
	}
	int WinSocket::recv(char * data, const int &len) throw (WinSocketException) {
		int error = ::recv(*socket, data, len, 0);
		if (error == SOCKET_ERROR) throw get_WinSocketException("WinSocket::recv: could not receive data.");
		return error;
	}
	int WinSocket::write(const char * data, const int &len) throw (WinSocketException) {
		int _len = len, error = 0;
		char *ptr = (char*)data;
		do { error = ::send(*socket, (ptr += error), _len, 0); }
		while (
			(error > 0 && (_len -= error) > 0) ||
			(error == SOCKET_ERROR && WSAGetLastError() == WSAEINTR)
		);

		if (error == SOCKET_ERROR) throw get_WinSocketException("WinSocket::write: could not write data.");
		return len - _len;
	}
	int WinSocket::read(char * data, const int &len) throw (WinSocketException) {
		int _len = len, error = 0;
		char *ptr = data;
		do { error = ::recv(*socket, (ptr += error), _len, MSG_WAITALL); }
		while (
			(error > 0 && (_len -= error) > 0) ||
			(error == SOCKET_ERROR && WSAGetLastError() == WSAEINTR)
		);
		if (error == SOCKET_ERROR) throw get_WinSocketException("WinSocket::read: could not read data.");
		return len - _len;
	}
	void WinSocket::connect(const WinSocketAddress &address) throw (WinSocketException) {
		int error = ::connect(*socket, address, sizeof(sockaddr_in));
		if (error == SOCKET_ERROR) throw get_WinSocketException("WinSocket::connect: could not connect socket to a provided address.");
	}
	void WinSocket::listen(const int &backlog) throw (WinSocketException) {
		int error = ::listen(*socket, backlog);
		if (error == SOCKET_ERROR) throw get_WinSocketException("WinSocket::listen: could not make listen socket.");
	}

	WinSocket::WinSocket(std::shared_ptr< SOCKET > socket) : socket(socket) {
		if (socket.use_count() == 1) {
			if (n++ == 0) init_winSocket();
		}
	}

	WinSocket WinSocket::accept(WinSocketAddress &address) throw (WinSocketException) {
		int address_len = sizeof(sockaddr_in);
		sockaddr addr;
		std::shared_ptr< SOCKET > sock (new SOCKET(::accept(*socket, &addr, &address_len)));
		if (*sock == INVALID_SOCKET || address_len != sizeof(sockaddr_in)) throw get_WinSocketException("WinSocket::accept: could not accept connection.");
		address = WinSocketAddress(addr);
		return WinSocket(std::move(sock));
	}
	WinSocket WinSocket::accept() throw (WinSocketException) {
		std::shared_ptr< SOCKET > sock(new SOCKET(::accept(*socket, nullptr, nullptr)));
		if (*sock == INVALID_SOCKET) throw get_WinSocketException("WinSocket::accept: could not accept connection.");
		return WinSocket(std::move(sock));
	}

	bool WinSocket::wait_for(const READ_WRITE rw, const std::chrono::milliseconds& wait_duration) throw (WinSocketException) {
		if (rw != READ_WRITE::READ && rw != READ_WRITE::WRITE) throw WinSocketException("WinSocket::wait_for: unknown READ_WRITE parameter value.");
		/*
		fd_set rwset;
		timeval timeout;

		FD_ZERO(&rwset);
		FD_SET(*socket, &rwset);

		timeout.tv_sec = wait_duration.count() / long(1e3);
		timeout.tv_usec = (wait_duration.count() % long(1e3)) * long(1e3);

		int res;

		if (rw == READ_WRITE::READ)
			res = ::select(*socket + 1, &rwset, NULL, NULL, &timeout);
		else
			res = ::select(*socket + 1, NULL, &rwset, NULL, &timeout);

		if (res == SOCKET_ERROR) throw get_WinSocketException("WinSocket::wait_for.");

		return bool(res);
		*/

		WSAPOLLFD descriptor = { *socket , ((rw == READ_WRITE::READ) ? POLLIN : POLLOUT /*POLLRDNORM : POLLWRNORM*/) , 0};

		int res = ::WSAPoll(&descriptor, 1, wait_duration.count());

		if (res == SOCKET_ERROR) throw get_WinSocketException("WinSocket::wait_for.");

		return bool(res);
	}
}