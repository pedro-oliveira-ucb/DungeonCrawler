#pragma once
#include <string>
#include	<vector>

enum rBaseResourceType {
	SPRITE,
	IMAGE,
	VIDEO
};

struct fileScanResult {
	std::string fileName; // Name of the file without extension
	std::string filePath; // ull path to the file
	std::string rawFileName; // Name of the file with extension
	std::string recursiveFileName; // ResursivePath with filename
	std::string rawRecursivePath; // RecursivePath without filename
	std::string folderPath;
};

class rBaseResource 
{
	std::string Path;
	std::vector<fileScanResult> getRecursiveFiles( std::string path , std::string extension  );
public:
	rBaseResource( std::string path );
	std::string getPath( );
	std::vector<std::vector<fileScanResult>> recursiveGetFiles( std::string path , std::string extension );

	virtual bool initialize( ) = 0;


};

