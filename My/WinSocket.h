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
		//������� �������� ����� ������
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

		// �������� ������ � "����� ������"
		void bind(const WinSocketAddress &address) throw (WinSocketException);
		
		// ��������� ��������� (������������ len ���� � data �� ������ address)
		// ���������� ���������� ��������� ����
		int sendto(const WinSocketAddress &address, const char * data, const int &len) throw (WinSocketException);
		
		// ������� ��������� ��������� ����� len
		// ������ ���������� ���������� ����.
		int recvfrom(WinSocketAddress &address, char * data, const int &len) throw (WinSocketException);
		int recvfrom(char * data, const int &len) throw (WinSocketException);

		// ��������� len ���� (����� ��� ������ ���� �������� � �������� ������)
		// ���������� ���������� ��������� ����
		int send(const char * data, const int &len) throw (WinSocketException);
		
		// ������� ����� (�� ������ len)
		// ������ ���� ���������� ���������� ����, ���� 0; 0 ��������, ��� ���������� �������.
		int recv(char * data, const int &len) throw (WinSocketException);

		// ������� ��� send. �������� ������ ��� len ���� �� data
		int write(const char * data, const int &len) throw (WinSocketException);

		// ������� ��� recv. �������� ������ ��� len ���� � data
		// ������ ���� ���������� ���������� ����, ���� 0; 0 ��������, ��� ���������� �������.
		int read(char * data, const int &len) throw (WinSocketException);

		// �������� ������ � ����������� "����� ������" (� ����)
		void connect(const WinSocketAddress &address) throw (WinSocketException);
		
		// ����� ������� ������ ��� ������� ����� ������ bind � ����� ������� accept
		// ��������� ����� � ��������� listen
		// �������� backlog ������ ������������ ����� ����������.
		void listen(const int &backlog = SOMAXCONN) throw (WinSocketException);
		
		// ���������� �����, ������� ����������� � ���������� ������� �� ������������� � �������, �� ������� �������� ���� �����.
		// � address ������������ ������, �� ���� �������� ������
		WinSocket accept(WinSocketAddress &address) throw (WinSocketException);
		WinSocket accept() throw (WinSocketException);


#define TIMEOUT_FUNC_GEN(FUNC_NAME, READ_WRITE_VALUE)
		/*������ - 1 - timeout. ��� ���������, ��� ��� ������� �������.*/
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