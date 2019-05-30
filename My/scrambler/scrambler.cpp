#include <iostream>
#include <boost/program_options.hpp>
#include <sstream>

namespace boostPO = boost::program_options;

namespace
{
	auto const ShortDescription = "This program performs encrypting and decrypting of sources";
	auto const DefaultPathValue = "./";
	unsigned short const DefaultKeyLength = 32;

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

int main(int argc, char* argv[])
{
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
	}

	return 0;
}
