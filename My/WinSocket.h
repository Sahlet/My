#pragma once
#include <memory>
#include <WinSock2.h>
#include <atomic>
#include <chrono>
#include <My/Exception.h>

namespace My {
	class WinSocketException : public Exception {
	protected:
		WinSocketException(long long errorCode, std::string errorCause, const char* what);
	public:
		WinSocketException(long long errorCode, std::string errorCause = "");
		WinSocketException(std::string errorCause = "");
		WinSocketException(WinSocketException&&) = default;
	};

	WinSocketException get_WinSocketException(const std::string& user_cause = "");

	class WinSocketAddress {
	private:
		sockaddr_in ad;
	public:
		WinSocketAddress(const char * const &ip = "", const unsigned short &port = 0) throw (WinSocketException);
		WinSocketAddress(const sockaddr& addr) throw (WinSocketException);
		WinSocketAddress(const sockaddr_in& addr) throw (WinSocketException);
		std::string get_ip() const;
		unsigned short get_port() const;
		inline operator sockaddr() const { return *((sockaddr*)&ad); }
		inline operator const sockaddr*() const { return (const sockaddr*)&ad; }
		bool operator == (const WinSocketAddress&);
		bool operator < (const WinSocketAddress&);
	};

	class WinSocket {
		WinSocket(std::shared_ptr< SOCKET > socket);
		//счетчик объектов этого класса
		static std::atomic<unsigned long> n;

		enum class READ_WRITE : int { READ, WRITE };

		bool wait_for(const READ_WRITE rw, const std::chrono::milliseconds& wait_duration) throw (WinSocketException);

	protected:
		std::shared_ptr< SOCKET > socket;
	public:
		WinSocket(const int &type = SOCK_STREAM/*tcp, SOCK_DGRAM udp*/) throw (WinSocketException);
		WinSocket(const WinSocket&) = default;
		WinSocket& operator=(const WinSocket&) = default;
		WinSocket(WinSocket&&) = default;
		WinSocket& operator=(WinSocket&&) = default;
		~WinSocket();

		inline SOCKET get() { return *socket; }

		// привязка сокета к "имени сокета"
		void bind(const WinSocketAddress &address) throw (WinSocketException);
		
		// отправить сообщение (отправляются len байт в data по адресу address)
		// возвращает количество отосланых байт
		int sendto(const WinSocketAddress &address, const char * data, const int &len) throw (WinSocketException);
		
		// принять сообщение ожидаемой длины len
		// вернет количество полученных байт.
		int recvfrom(WinSocketAddress &address, char * data, const int &len) throw (WinSocketException);
		int recvfrom(char * data, const int &len) throw (WinSocketException);

		// отправить len байт (сокет уже должен быль привязан к внешнему адресу)
		// возвращает количество отосланых байт
		int send(const char * data, const int &len) throw (WinSocketException);
		
		// принять байты (не больше len)
		// вернет либо количество полученных байт, либо 0; 0 означает, что соединение закрыто.
		int recv(char * data, const int &len) throw (WinSocketException);

		// обертка над send. пытается писать все len байт из data
		int write(const char * data, const int &len) throw (WinSocketException);

		// обертка над recv. пытается читать все len байт в data
		// вернет либо количество полученных байт, либо 0; 0 означает, что соединение закрыто.
		int read(char * data, const int &len) throw (WinSocketException);

		// привязка сокета к глобальному "имени сокета" (в сети)
		void connect(const WinSocketAddress &address) throw (WinSocketException);
		
		// можно вызвать только для сокетов после вызова bind и перед вызовом accept
		// переводит сокет в состояние listen
		// параметр backlog задает максимальное число соединений.
		void listen(const int &backlog = SOMAXCONN) throw (WinSocketException);
		
		// возвращает сокет, который образовался в результате запроса на подсоединение к серверу, за который отвечает этот сокет.
		// в address записывается адресс, от куда поступил запрос
		WinSocket accept(WinSocketAddress &address) throw (WinSocketException);
		WinSocket accept() throw (WinSocketException);


#define TIMEOUT_FUNC_GEN(FUNC_NAME, READ_WRITE_VALUE)
		/*вернет - 1 - timeout. все остальное, как для ждущего аналога.*/
		template< typename ... Args >
		int FUNC_NAME(const std::chrono::milliseconds& wait_duration, Args ... args) throw (WinSocketException) {
			if (!wait_for(READ_WRITE_VALUE, wait_duration)) return -1;
			return FUNC_NAME(args...);
		}
		
		TIMEOUT_FUNC_GEN(sendto, READ_WRITE::WRITE);
		TIMEOUT_FUNC_GEN(recvfrom, READ_WRITE::READ);
		TIMEOUT_FUNC_GEN(send, READ_WRITE::WRITE);
		TIMEOUT_FUNC_GEN(recv, READ_WRITE::READ);
	};
}