#include "WinSockBaseSerialization.h"

namespace My {
	//отправл€ет len байт по адресу str, отправл€€ перед ними количество отправл€емых байт (то есть len)
	void writestr(Socket& sock, const char* str, const size_t &len) throw (WinSocketException) {
		unsigned char len_[4] = { (len >> 24), (len >> 16) % (1 << 8) , (len >> 8) % (1 << 16), len % (1 << 24) };
		sock.write((char*)len_, sizeof(len_));
		if (len != 0) sock.write(str, len);
	}

	//принимает в str, отправленное к нам некоторое количество байт, и записывает это количество в len
	std::string readstr(Socket& sock) throw (WinSocketException) {
		unsigned char len_[4] = {0, 0, 0, 0};
		size_t len;
		if (sock.read((char*)len_, sizeof(len_)) == sizeof(len_)) len = (len_[0] << 24) + (len_[1] << 16) + (len_[2] << 8) + (len_[3]);
		else return std::string();

		std::string res;
		res.resize(len);
		unsigned short read_len = sock.read(const_cast< char* >(res.data()), len);
		if (read_len != len) throw(My::get_WinSocketException("readstr: could not read as much as wanted"));
		return res;
	}
}