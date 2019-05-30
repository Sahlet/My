#include <iostream>
#include <boost/program_options.hpp>
#include <sstream>

namespace boostPO = boost::program_options;

namespace
{
	auto const ShortDescription = "This program performs encrypting and decrypting of sources";
	auto const DefaultPathValue = "./";
	unsigned short const DefaultKeyLength = 32;
	unsigned short const MinKeyLength = 1;
	unsigned short const MaxKeyLength = 256;

	struct ProgramOption
	{
	public:
		ProgramOption(const char* cFullName = nullptr, char cShortName = 0) :
			m_cFullName(cFullName),
			m_cShortName(cShortName)
		{}

	public:
		operator const char*() const
		{
			return m_cFullName;
		}

		operator std::string() const
		{
			return m_cFullName;
		}

		std::string operator + (const std::string& right) const
		{
			return m_cFullName + right;
		}

	public:
		const char* m_cFullName;
		char m_cShortName;
	};

	const ProgramOption
		Help{"help", 'h'},
		Encrypt{"encryp", 'e'},
		Decrypt{"decrypt", 'd'},
		Key{"key", 'k'},
		Path{"path", 'p'},
		GenKey{"gen-key", 'g'};

	std::string getUsageHelp()
	{
		auto usage = "usage: scrambler";
		auto usageLen = strlen(usage);
		std::stringstream sstream;
		sstream << usage
			<< " [--" << Help << "]"
			<< " [--" << Encrypt << "]"
			<< " [--" << Decrypt << "]"
			<< std::endl
			<< std::string(std::max(0, int(usageLen) - 1), ' ')
			<< " [--" << Key << "=<" << Key << ">]"
			<< " [--" << Path << "=<" << Path << ">]"
			<< " [--" << GenKey << " <key length>]"
			<< std::endl;
		return sstream.str();
	}

	std::string getWorkflowInfo()
	{
		std::stringstream sstream;

		sstream << "Workflow info:" << std::endl;

		sstream << "-for encryping, the program takes contents of folder <path>/src/" << std::endl
			<< " and converts it into encrypted file which puts into folders <path>/crypt/" << std::endl
			<< " and <path>/crypt_storage/crypt_YYYY-MM-DD-HH-MM-SS/" << std::endl;

		sstream << "-for decryping, the program takes encrypted file from the folder <path>/crypt/" << std::endl
			<< " and converts it to sources files into folder" << std::endl
			<< " <path>/src_encrypt/src_YYYY-MM-DD-HH-MM-SS/" << std::endl;

		sstream << "-if you need more security - use '" << Key << "' option;" << std::endl
			<< " to generate new key use '" << GenKey << "' option" << std::endl;

		return sstream.str();
	}

	boostPO::options_description getProgramOptionsDescription()
	{
		boostPO::options_description desc("Options");
		desc.add_options()
		((Help + "," + Help.m_cShortName).c_str(), "Help screen")
		((Encrypt + "," + Encrypt.m_cShortName).c_str(), "Indicates that sources should be encrypted")
		((Decrypt + "," + Decrypt.m_cShortName).c_str(), "Indicates that the encrypted file should be decrypted back to the sources")
		((Key + "," + Key.m_cShortName).c_str(), boostPO::value<std::string>(), "Symmetric key for encrypting or decrypting")
		((Path + "," + Path.m_cShortName).c_str(), boostPO::value<std::string>()->default_value(DefaultPathValue), "Path where to execute encrypting or decrypting")
		((GenKey + "," + GenKey.m_cShortName).c_str(), boostPO::value<unsigned short>()->implicit_value(DefaultKeyLength), "Generates new key (arg is length of key)");

		return std::move(desc);
	}
}

namespace Encrypting
{
	auto const OpenSslAesSettings = "openssl aes-256-cbc -salt -a";

	int encryptFile(const std::string& inPath, const std::string& outPath, const std::string& key)
	{
		std::stringstream sstream;
		sstream << OpenSslAesSettings
			<< " -e"
			<< " -in " << inPath
			<< " -out " << outPath
			<< " -pass pass:" << key
			<< " >>/dev/null 2>>/dev/null";
		
		return ::system(sstream.str().c_str());
	}

	int decryptFile(const std::string& inPath, const std::string& outPath, const std::string& key)
	{
		std::stringstream sstream;
		sstream << OpenSslAesSettings
			<< " -d"
			<< " -in " << inPath
			<< " -out " << outPath
			<< " -pass pass:" << key
			<< " >>/dev/null 2>>/dev/null";
		
		return ::system(sstream.str().c_str());
	}

	std::string genKey(unsigned int length)
	{
		std::string key(length, '\0');
		
		std::mt19937 mt((std::random_device())());
		std::uniform_int_distribution<int> sbymolCategoryDistributor(0, 2);
		std::array<std::uniform_int_distribution<char>, 3> categories
		{
			std::uniform_int_distribution<char>('0', '9'),
			std::uniform_int_distribution<char>('a', 'z'),
			std::uniform_int_distribution<char>('A', 'Z')
		};

		for (auto& ch : key)
		{
			ch = categories[sbymolCategoryDistributor(mt)](mt);
		}

		return std::move(key);
	}
}

int main(int argc, char* argv[])
{
	int errCodeOptionsError = 1;
	int errCodeKeyLengtError = 2;

	auto onKeyLengthError = [&]()
	{
		std::cerr << "key length should be in range [" << MinKeyLength << ", " << MaxKeyLength << "]" << std::endl;
		return errCodeKeyLengtError;
	};

	try
	{
		auto desc = getProgramOptionsDescription();

		boostPO::variables_map vm;
		boostPO::store(boostPO::parse_command_line(argc, argv, desc), vm);
		boostPO::notify(vm);

		if (vm.count(Help))
		{
			std::cout << ShortDescription << std::endl << std::endl
				<< desc << std::endl
				<< getWorkflowInfo() << std::endl;
		}
		else if (vm.count(GenKey))
		{ 
			unsigned short length = vm[GenKey].as<unsigned short>();
			if (length < MinKeyLength || length > MaxKeyLength)
			{
				return onKeyLengthError();
			}
			std::cout << Encrypting::genKey(length) << std::endl;
		}
		else if (vm.count("age"))
		{
			std::cout << "Age: " << vm["age"].as<int>() << '\n';
		}
		else if (vm.count("pi"))
		{
			std::cout << "Pi: " << vm["pi"].as<float>() << '\n';
		}
		else
		{
			std::cout << getUsageHelp();
		}
	}
	catch (const boostPO::error &ex)
	{
		std::cerr << ex.what() << std::endl
			<< getUsageHelp();
		return errCodeOptionsError;
	}

	return 0;
}
