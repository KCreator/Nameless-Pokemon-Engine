#pragma once

//Ini Parser.
struct INI
{
	INI::INI( std::string filename );

	FILE *file;

	std::map< std::string, std::map< std::string, std::string > > sections;
	void Parse();

private:
	bool error;
	std::string errorOutput;
};