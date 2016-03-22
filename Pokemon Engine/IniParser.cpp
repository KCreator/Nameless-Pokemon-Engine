#include "stdafx.h"
#include <string>
#include <map>

#include "IniParser.h"

INI::INI( std::string filename )
{
	file = fopen( filename.c_str(), "r" );
	if( file != NULL )
		Parse();
	else
	{
		error = true;
		errorOutput = "ERROR: File not found.";
	}
}

void INI::Parse()
{
	//Read the file, sending relevent data to our datamap.
	//Init critical info
	char* buf;
	std::string section;
	std::string key, value;

	// obtain file size:
	long lSize;
	fseek (file , 0 , SEEK_END);
	lSize = ftell(file);
	rewind(file);

	//Allocate memory
	buf = (char*)malloc(sizeof(char)*lSize);
    if (buf == NULL)
	{
		error = true;
		errorOutput = "ERROR: Memory not allocated";
		return;
	}

	int eof = fread( buf, 1, lSize, file );

	int pos = 0;
	bool comment = false;
	bool heading = false;
	bool bkey = false;
	bool bvalue = false;
	while( pos < eof )
	{
		//Handle comments:
		if( buf[pos] == ';' && comment == false )
			comment = true;
		if( buf[pos] == '\n' && comment == true )
			comment = false;
		if(  comment == true )
		{
			pos++;
			continue;
		}
		//Parse headings
		if( heading == false &&  buf[pos] == '[' )
		{
			bvalue = false;
			bkey = false;
			heading = true;
			pos++;
		}
		if( heading == true )
		{
			if( buf[pos] == ']' )
			{
				heading = false;
				bkey = true;
				pos++;
			}
			else
				section += buf[pos];
		}

		if( bkey == true )
		{
			if( buf[pos] == '=' )
			{
				bkey = false;
				bvalue = true;
				pos++;
			}
			else
			{
				key += buf[pos];
			}
		}
		if( bvalue == true )
		{
			if( buf[pos] == '\n' )
			{
				sections[section][key] = value;
				bkey = true;
				bvalue = false;
				section = "";
				key = "";
				value = "";
			}
			else
			{
				value += buf[pos];
			}
		}

		pos++;
	}

	free( buf );
}