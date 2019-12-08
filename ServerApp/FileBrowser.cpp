#include"FileBrowser.h"
#include <filesystem>

bool FileBrowser::setWorkingDirectory(const std::string path)
{
	bool result = pathValid(path);
	if (result)
		m_WorkingDirectory = path;

	return result;
}

std::vector<std::string> FileBrowser::listOfFilesAndFolders() const
{
	std::vector<std::string> result;

	for (const auto& entry : std::filesystem::directory_iterator(m_WorkingDirectory))
		result.push_back(entry.path().string());

	return result;
}


bool FileBrowser::pathValid(const std::string& path)
{
	bool result = std::filesystem::exists(path);
	return result;
}