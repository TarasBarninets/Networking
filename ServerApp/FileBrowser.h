#pragma once
#include <string>
#include <vector>

class FileBrowser {
public:
	bool setWorkingDirectory(const std::string path);
	std::vector<std::string> listOfFilesAndFolders() const;
	
private:
	std::string m_WorkingDirectory;
	bool pathValid(const std::string& path);
};
