#include <iostream>
#include <string>
#include <filesystem> 
#include <Windows.h>

namespace fs = std::filesystem;

bool CheckIfReadOnly(fs::path filePath)
{
	fs::file_status status = fs::status(filePath);
	return (status.permissions() & fs::perms::owner_write) == fs::perms::none;
}

bool CheckIfHidden(fs::path filePath)
{
	DWORD attributes = GetFileAttributesA(filePath.generic_string().c_str());
	return attributes & FILE_ATTRIBUTE_HIDDEN;
}

bool CheckIfArchive(fs::path filePath)
{
	DWORD attributes = GetFileAttributesA(filePath.generic_string().c_str());
	return attributes & FILE_ATTRIBUTE_ARCHIVE;
}

bool CheckExtension(fs::path filePath, fs::path extension)
{
	if(extension.empty())
	{
		return true;
	}

	return filePath.extension() == extension;
}

void MoveAllFilesFromCatalog(fs::path sourcePath, fs::path destPath, fs::path extension)
{
	for(fs::directory_entry const& entry : fs::directory_iterator(sourcePath))
	{
		fs::path filePath = entry.path();
		
		if(entry.is_regular_file() && !CheckIfReadOnly(filePath) 
								   && !CheckIfHidden(filePath) 
								   && !CheckIfArchive(filePath)
								   && CheckExtension(filePath, extension))
		{
			fs::path fileDestPath = destPath / filePath.filename();
			fs::copy_options copyOptions = fs::copy_options::overwrite_existing;

			if(fs::copy_file(filePath, fileDestPath, copyOptions))
			{
				fs::remove(filePath);
			}
			else
			{
				std::cout << "Failed to move " << filePath.generic_string() << std::endl;
			}
		}
	}
}

int main(int argc, char *argv[])
{
	std::cout << "Input source folder path: ";

	fs::path sourcePath;
	std::cin >> sourcePath;
	
	if(fs::exists(sourcePath))
	{
		std::cout << "Do you want to copy files with specific extensions [Y/N]? ";

		std::string answer;
		std::cin >> answer;

		fs::path extension = "";
		if(answer == "Y" || answer == "y")
		{
			std::cout << "Input file extension in this format .exe: ";
			std::cin >> extension;
		}

		std::cout << "Input output folder path: ";

		fs::path destPath;
		std::cin >> destPath;

		if(fs::exists(destPath))
		{
			MoveAllFilesFromCatalog(sourcePath, destPath, extension);
		}
		else
		{
			std::cout << "Output folder doesn't exist. Do you want to copy files into the current folder [Y/N]? ";

			std::cin >> answer;

			if(answer == "Y" || answer == "y")
			{
				fs::path currentPath = fs::current_path();
				MoveAllFilesFromCatalog(sourcePath, currentPath, extension);
			}
		}
	}	
	else
	{
		std::cout << "Source folder doesn't exist" << std::endl;
	}

	return 0;
}