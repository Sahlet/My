/*
if you want run it on Windows, you should have these utilities:
 -openssl for Windows http://gnuwin32.sourceforge.net/packages/openssl.htm
 -zip unzip utilities for Windows http://stahlworks.com/dev/index.php?tool=zipunzip
 -ls utility for Windows https://u-tools.com/msls
*/

#include <iostream>
#include <fstream>
#include <sstream>
#include <functional>
#include <future>
#include <atomic>
#include <list>
#include <vector>
#include <boost/program_options.hpp>
#include <boost/filesystem/operations.hpp>
#include <My/Guard.h>

namespace boostPO = boost::program_options;
namespace boostFS = boost::filesystem;

namespace
{
	auto const ShortDescription = "This program performs encrypting and decrypting of sources";
	auto const DefaultPathValue = "./";
	auto const LexicographicalTimeFormat = "YYYY-MM-DD-HH-MM-SS";
	auto const SrcDirName = "src";
	auto const SrcStorageDirName = "src_storage";
	auto const CryptFileName = "src.crypt";
	auto const CryptDirName = "crypt";
	auto const CryptStorageDirName = "crypt_storage";

	unsigned short const DefaultKeyLength = 32;
	unsigned short const MinKeyLength = 1;
	unsigned short const MaxKeyLength = 256;
	std::chrono::milliseconds AccompanyWithConsoleProcessingDelay(200); //ms

	int errCodeOptionsError = 1;
	int errCodeKeyLengtError = 2;
	int errCodeSomeError = 3;
	int errCodeIncompatibleOptionsError = 4;
	int errCodeIncorrectDirectoryStructError = 5;
	int errCodeNoCryptFileError = 6;

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
			<< std::string(usageLen, ' ')
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

		sstream << "-for encryping, the program takes folder <path>/" << SrcDirName << "/" << " and converts it into" << std::endl
			<< " encrypted file <path>/" << CryptStorageDirName << "/" << CryptDirName << "_" << LexicographicalTimeFormat << "/" << CryptFileName << std::endl
			<< " and creates hard link to encrypted file <path>/" << CryptDirName << "/" << CryptFileName << std::endl;

		sstream << "-for decryping, the program takes encrypted file <path>/" << CryptDirName << "/" << CryptFileName << std::endl
			<< " and converts it to sources files into folder" << std::endl
			<< " <path>/" << SrcStorageDirName << "/" << SrcDirName << "_" << LexicographicalTimeFormat << "/" << std::endl;

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
		((Path + "," + Path.m_cShortName).c_str(), boostPO::value<std::string>(), "Path where to execute encrypting or decrypting")
		((GenKey + "," + GenKey.m_cShortName).c_str(), boostPO::value<unsigned short>()->implicit_value(DefaultKeyLength), "Generates new key (arg is length of key)");

		return std::move(desc);
	}

	int onIncompatibleOptionsError()
	{
		std::cerr << "incompatible options" << std::endl
			<< getUsageHelp();
		return errCodeIncompatibleOptionsError;
	}

	int onKeyLengthError()
	{
		std::cerr << "key length should be in range [" << MinKeyLength << ", " << MaxKeyLength << "]" << std::endl;
		return errCodeKeyLengtError;
	}

	int onIncorrectDirectoryStructError()
	{
		std::cerr << "incorrect directory structure;" << std::endl
			<<"use '" << Help << "' option for more information" << std::endl;
		return errCodeIncorrectDirectoryStructError;
	}

	int onNoCryptFileError()
	{
		std::cerr << "'" << CryptFileName << "' file is not in '" << CryptDirName << "' directory" << std::endl;
		return errCodeNoCryptFileError;
	}

	std::string toLexicographicalTimeFormat(const std::chrono::system_clock::time_point& timePoint)
	{
		time_t tt = std::chrono::system_clock::to_time_t(timePoint);
		
		tm utc_tm = *gmtime(&tt);

		char buf[200];
		sprintf(buf,
			"%.4d-%.2d-%.2d-%.2d-%.2d-%.2d",
			utc_tm.tm_year + 1900,
			utc_tm.tm_mon + 1,
			utc_tm.tm_mday,
			utc_tm.tm_hour,
			utc_tm.tm_min,
			utc_tm.tm_sec
		);

		return buf;
	}

	void accompanyWithConsoleProcessing(std::function<void()> task)
	{
		if (!task)
		{
			return;
		}

		std::atomic<bool> taskIsFinished(false);

		auto accompany = [&]()
		{
			std::list<char> symblos{ '-', '\\', '|', '/' };
			auto iter = symblos.begin();

			std::chrono::system_clock::time_point start;

			while (!taskIsFinished.load(std::memory_order_consume))
			{
				auto now = std::chrono::system_clock::now();
				if (now - start >= AccompanyWithConsoleProcessingDelay)
				{
					start = now;
					std::cout << '\r' << *iter;
					std::cout.flush();
					if (++iter == symblos.end())
					{
						iter = symblos.begin();
					}
				}

				std::this_thread::yield();
			}

			std::cout << "\r \r";
			std::cout.flush();
		};

		auto future = std::async(std::launch::async, accompany);

		My::Guard finally([&]()
		{
			taskIsFinished.store(true, std::memory_order_release);
			future.get();
		});

		task();
	}
} // namespace

namespace Encrypting
{
	auto const OpenSslAesSettings = "openssl aes-256-cbc -salt -a";

	int encryptFile(const boostFS::path& inPath, const boostFS::path& outPath, const std::string& key)
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

	int decryptFile(const boostFS::path& inPath, const boostFS::path& outPath, const std::string& key)
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

	template<typename Generator>
	std::string genKey(unsigned int length, int seed)
	{
		auto getUniformDistributor = [](typename Generator::result_type min, typename Generator::result_type max)
		{
			if (min > max)
			{
				std::swap(min, max);
			}

			return [min, max](typename Generator::result_type randNumber)
			{
				return min + randNumber % (max - min + 1);
			};
		};

		std::string key(length, '\0');
		
		Generator rnd(seed);
		auto sbymolCategoryDistributor = getUniformDistributor(0, 2);
		std::array<std::function<typename Generator::result_type(typename Generator::result_type)>, 3> categories
		{
			getUniformDistributor('0', '9'),
			getUniformDistributor('a', 'z'),
			getUniformDistributor('A', 'Z')
		};

		for (auto& ch : key)
		{
			ch = (char)categories[sbymolCategoryDistributor(rnd())](rnd());
		}

		return std::move(key);
	}

	std::string genKey(unsigned int length)
	{
		return genKey<std::mt19937>(length, (std::random_device())());
	}

	std::string getDefaultKey()
	{
		class SimpleRand
		{
		public:
			typedef uint32_t result_type;

		public:
			SimpleRand(uint32_t seed1 = 0)
			{
				seed(seed1);
			}

		public:
			void seed(uint32_t seed1)
			{
				if (seed1 == 0)
					seed1 = this->max();
				m_iState = seed1;
			}

			uint32_t operator()()
			{
				m_iState ^= (m_iState << 13);
				m_iState ^= (m_iState >> 17);
				m_iState ^= (m_iState << 5);
				return m_iState;
			}

			uint32_t min() const
			{
				return 1;
			}

			uint32_t max() const
			{
				return 0xFFFFFFFFu;
			}

		private:
			uint32_t m_iState;
		};

		return genKey<SimpleRand>(DefaultKeyLength, ::strlen(LexicographicalTimeFormat));
	}
} // namespace Encrypting

namespace Zipping
{
	int zip(const boostFS::path& src, const boostFS::path& zipFile)
	{
		std::stringstream sstream;
		sstream << "zip -r -q"
			<< " " << zipFile
			<< " " << src
			<< " >>/dev/null 2>>/dev/null";
		
		return ::system(sstream.str().c_str());
	}

	int unzip(const boostFS::path& zipFile, const boostFS::path& placePath = "./")
	{
		std::stringstream sstream;
		sstream << "unzip -o -q"
			<< " " << zipFile
			<< " -d" << placePath
			<< " >>/dev/null 2>>/dev/null";
		
		return ::system(sstream.str().c_str());
	}
} // namespace Zipping

namespace MainFunctionality
{
	auto const ZipFileName = "src.zip";
	auto const ScramblTempDirNamePrefix = "scrambler";

	void scramble(const boostFS::path& path, const std::string& key, boostFS::path& encryptedDataPath)
	{
		if (!boostFS::exists(path))
		{
			throw std::runtime_error("invalid path '" + path.string() + "'");
		}

		if (!boostFS::exists(path/SrcDirName))
		{
			throw std::runtime_error("there is no '" + (path/SrcDirName).string() + "' but supposed to be");
		}

		boostFS::path tmpDir;
		do
		{
			std::string scramblTempDirName = ScramblTempDirNamePrefix + ("_" + boostFS::unique_path().string());
			tmpDir = boostFS::temp_directory_path()/scramblTempDirName;
		} while (boostFS::exists(tmpDir));

		boostFS::create_directories(tmpDir);

		My::Guard tmpDirGuargd([&]
		{
			boostFS::remove_all(tmpDir);
		});

		std::string timestamp = toLexicographicalTimeFormat(std::chrono::system_clock::now());
		std::string srcDirNameWithTimestamp = (SrcDirName + ("_" + timestamp));
		std::string cryptDirNameWithTimestamp = (CryptDirName + ("_" + timestamp));

		boostFS::create_directories(tmpDir/srcDirNameWithTimestamp);
		boostFS::create_symlink(boostFS::system_complete(path/SrcDirName), tmpDir/srcDirNameWithTimestamp/SrcDirName);

		{
			auto curDir = boostFS::current_path();
			::chdir(tmpDir.string().c_str());
			My::Guard curDirGuard([&]()
			{
				::chdir(curDir.string().c_str());
			});

			if(Zipping::zip(srcDirNameWithTimestamp, ZipFileName) != 0)
			{
				throw std::runtime_error("zipping failed");
			}
		}

		auto destFolder = path/CryptStorageDirName/cryptDirNameWithTimestamp;
		if (boostFS::exists(destFolder))
		{
			boostFS::remove_all(destFolder);
		}

		boostFS::create_directories(destFolder);

		if(Encrypting::encryptFile(tmpDir/ZipFileName, destFolder/CryptFileName, key) != 0)
		{
			throw std::runtime_error("encrypting failed");
		}

		auto destDataPath = path/CryptDirName/CryptFileName;

		if (boostFS::exists(destDataPath))
		{
			boostFS::remove_all(destDataPath);
		}

		boostFS::create_directories(path/CryptDirName);

		boostFS::create_hard_link(boostFS::system_complete(destFolder/CryptFileName), destDataPath);

		if (!boostFS::exists(destDataPath))
		{
			throw std::runtime_error("scrambling failed");
		}

		encryptedDataPath = destDataPath;
	}

	void unscramble(const boostFS::path& path, const std::string& key, boostFS::path& decryptedDataPath)
	{
		if (!boostFS::exists(path))
		{
			throw std::runtime_error("invalid path '" + path.string() + "'");
		}

		auto cryptFile = path/CryptDirName/CryptFileName;

		if (!boostFS::exists(cryptFile))
		{
			throw std::runtime_error("there is no crypt file '" + cryptFile.string() + "' but supposed to be");
		}

		boostFS::create_directories(path/SrcStorageDirName);

		boostFS::path tmpDir;
		do
		{
			auto scramblTempDirName = ScramblTempDirNamePrefix + ("_" + boostFS::unique_path().string());
			tmpDir = path/SrcStorageDirName/scramblTempDirName;
		} while (boostFS::exists(tmpDir));

		boostFS::create_directories(tmpDir);

		My::Guard tmpDirGuargd([&]
		{
			boostFS::remove_all(tmpDir);
		});

		if(Encrypting::decryptFile(cryptFile, tmpDir/ZipFileName, key) != 0)
		{
			throw std::runtime_error("decrypting failed");
		}

		auto zipDeploymentDirName = "zipDeploymentDir";

		if(Zipping::unzip(tmpDir/ZipFileName, tmpDir/zipDeploymentDirName) != 0)
		{
			throw std::runtime_error("unzipping failed");
		}

		auto lsResFileName = "lsRes.txt";

		if(::system(("ls " + (tmpDir/zipDeploymentDirName).string() + " > " + (tmpDir/lsResFileName).string()).c_str()) != 0)
		{
			throw std::runtime_error("getting 'ls' failed while unscrambling");
		}

		std::ifstream lsResFile((tmpDir/lsResFileName).string());
		std::string srcDirNameWithTimestamp;
		std::getline(lsResFile, srcDirNameWithTimestamp);

		auto destDataPath = path/SrcStorageDirName/srcDirNameWithTimestamp;

		if (boostFS::exists(destDataPath))
		{
			boostFS::remove_all(destDataPath);
		}

		boostFS::rename(tmpDir/zipDeploymentDirName/srcDirNameWithTimestamp, destDataPath);

		if (!boostFS::exists(destDataPath))
		{
			throw std::runtime_error("unscrambling failed");
		}

		decryptedDataPath = destDataPath;
	}
} // namespace MainFunctionality

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
			if (vm.size() > 1)
			{
				return onIncompatibleOptionsError();
			}

			std::cout << ShortDescription << std::endl << std::endl
				<< desc << std::endl
				<< getWorkflowInfo() << std::endl;
		}
		else if (vm.count(GenKey))
		{
			if (vm.size() > 1)
			{
				return onIncompatibleOptionsError();
			}

			unsigned short length = vm[GenKey].as<unsigned short>();
			if (length < MinKeyLength || length > MaxKeyLength)
			{
				return onKeyLengthError();
			}

			std::cout << Encrypting::genKey(length) << std::endl;
			return 0;
		}
		else if (vm.count(Encrypt) || vm.count(Decrypt))
		{
			if(vm.count(Encrypt) && vm.count(Decrypt))
			{
				return onIncompatibleOptionsError();
			}

			std::string key = vm.count(Key) ? vm[Key].as<std::string>() : Encrypting::getDefaultKey();
			if (key.size() < MinKeyLength || key.size() > MaxKeyLength)
			{
				return onKeyLengthError();
			}

			boostFS::path path = vm.count(Path) ? vm[Path].as<std::string>() : DefaultPathValue;
			if (!boostFS::is_directory(path))
			{
				return onIncorrectDirectoryStructError();
			}

			try
			{
				if (vm.count(Encrypt))
				{
					if (!boostFS::is_directory(path/SrcDirName))
					{
						return onIncorrectDirectoryStructError();
					}

					boostFS::path encryptedDataPath;

					accompanyWithConsoleProcessing([&]()
					{
						MainFunctionality::scramble(path, key, encryptedDataPath);
					});

					std::cout << "encryption is DONE! encrypted data here:" << std::endl
						<< encryptedDataPath << std::endl;
				}

				if (vm.count(Decrypt))
				{
					if (!boostFS::is_directory(path/CryptDirName))
					{
						return onIncorrectDirectoryStructError();
					}

					if (!boostFS::exists(path/CryptDirName/CryptFileName))
					{
						return onNoCryptFileError();
					}

					boostFS::path decryptedDataPath;

					accompanyWithConsoleProcessing([&]()
					{
						MainFunctionality::unscramble(path, key, decryptedDataPath);
					});

					std::cout << "decryption is DONE! decrypted data here:" << std::endl
						<< decryptedDataPath << std::endl;
				}

				return 0;
			}
			catch (const std::exception& ex)
			{
				std::cerr << ex.what() << std::endl;
				return errCodeSomeError;
			}

			return 0;
		}
		else if (!vm.empty())
		{
			return onIncompatibleOptionsError();
		}
		else
		{
			std::cout << getUsageHelp();
			return 0;
		}
	}
	catch (const boostPO::error& ex)
	{
		std::cerr << ex.what() << std::endl
			<< getUsageHelp();
		return errCodeOptionsError;
	}

	return 0;
}
