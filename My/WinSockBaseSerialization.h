#pragma once
#include"WinSocket.h"
namespace My {
	typedef WinSocket Socket;
	/**
		общение с клиентом:
			-по протоколу TCP.
			-в заглавие каждго сообщения ложется 2 байта, которые представляют из себя целое неотрицательное число [0; 256 * 256 - 1] записаное в порядке байт от старшего к младшему.
		таким образом каждая сторона будет знать, какой длины сообшение ожидать.
	**/

	//отправляет len байт по адресу str, отправляя перед ними количество отправляемых байт (то есть len)
	void writestr(Socket& sock, const char* str, const size_t &len) throw (WinSocketException);
	inline Socket& operator<<(Socket& sock, const std::string &s) {
		writestr(sock, s.c_str(), s.size());
		return sock;
	}
	inline Socket& operator<<(Socket& sock, const char * const &ptr) throw (WinSocketException) {
		writestr(sock, ptr, strlen(ptr));
		return sock;
	}

	//принимает в str, отправленное к нам некоторое количество байт, и записывает это количество в len
	std::string readstr(Socket& sock) throw (WinSocketException);
	inline Socket& operator >> (Socket& sock, std::string& s) throw (WinSocketException) {
		s = readstr(sock);
		return sock;
	}
}