#include <iostream>
#include <boost/program_options.hpp>
#include <sstream>

namespace boostPO = boost::program_options;

namespace
{
	auto const ShortDescription = "This program performs encrypting and decrypting of sources";
	auto const DefaultPathValue = "./";
	auto const EncriptionAlgorithmName = "...";

	struct ProgramOption
	{
	public:
		ProgramOption(const char* cFullName = nullptr, char cShortName = 0) :
			m_cFullName(cFullName),
			m_cShortName(cShortName)
		{}
	public:
		const char* m_cFullName;
		char m_cShortName;
	};

	const ProgramOption
		Help{"help", 'h'},
		Encrypt{"encryp", 'e'},
		Decrypt{"decrypt", 'd'},
		Key{"key", 'k'},
		Path{"path", 'p'};

	std::string getUsageHelp()
	{
		auto usage = "usage: scrambler ";
		auto usageLen = strlen(usage);
		std::stringstream sstream;
		sstream << usage << "[--" << Help.m_cFullName << "]" << "[--" << Encrypt.m_cFullName << "]"
			<< "[--" << Decrypt.m_cFullName << "]" << std::endl
			<< std::string(usageLen, ' ') << "[--" << Key.m_cFullName << "=<" << Key.m_cFullName << ">]"
			<< "[--" << Path.m_cFullName << "=<" << Path.m_cFullName << ">]" << std::endl;
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

		sstream << "-if you need more security - use key option;" << std::endl
			<< " encryption algorithm is " << EncriptionAlgorithmName << std::endl
			<< " so key is asymmetric for encrypting and decrypting" << std::endl;

		return sstream.str();
	}
}

int main(int argc, char* argv[])
{
	try
	{
		boostPO::options_description desc{"Options"};
		desc.add_options()
		((Help.m_cFullName + std::string(",") + Help.m_cShortName).c_str(), "Help screen")
		((Encrypt.m_cFullName + std::string(",") + Encrypt.m_cShortName).c_str(), "Indicates that sources should be encrypted")
		((Decrypt.m_cFullName + std::string(",") + Decrypt.m_cShortName).c_str(), "Indicates that the encrypted file should be decrypted back to the sources")
		((Key.m_cFullName + std::string(",") + Key.m_cShortName).c_str(), boostPO::value<std::string>(), "Key for encrypting or decrypting")
		((Path.m_cFullName + std::string(",") + Path.m_cShortName).c_str(), boostPO::value<std::string>()->default_value(DefaultPathValue), "Path where to execute encrypting or decrypting");

		boostPO::variables_map vm;
		boostPO::store(boostPO::parse_command_line(argc, argv, desc), vm);
		boostPO::notify(vm);

		if (vm.count("help"))
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
