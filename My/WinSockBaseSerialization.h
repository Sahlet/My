#pragma once
#include"WinSocket.h"
namespace My {
	typedef WinSocket Socket;
	/**
		������� � ��������:
			-�� ��������� TCP.
			-� �������� ������ ��������� ������� 2 �����, ������� ������������ �� ���� ����� ��������������� ����� [0; 256 * 256 - 1] ��������� � ������� ���� �� �������� � ��������.
		����� ������� ������ ������� ����� �����, ����� ����� ��������� �������.
	**/

	//���������� len ���� �� ������ str, ��������� ����� ���� ���������� ������������ ���� (�� ���� len)
	void writestr(Socket& sock, const char* str, const size_t &len) throw (WinSocketException);
	inline Socket& operator<<(Socket& sock, const std::string &s) {
		writestr(sock, s.c_str(), s.size());
		return sock;
	}
	inline Socket& operator<<(Socket& sock, const char * const &ptr) throw (WinSocketException) {
		writestr(sock, ptr, strlen(ptr));
		return sock;
	}

	//��������� � str, ������������ � ��� ��������� ���������� ����, � ���������� ��� ���������� � len
	std::string readstr(Socket& sock) throw (WinSocketException);
	inline Socket& operator >> (Socket& sock, std::string& s) throw (WinSocketException) {
		s = readstr(sock);
		return sock;
	}
}