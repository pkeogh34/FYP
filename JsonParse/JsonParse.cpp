//
// Created by Patrick Ross Keogh on 17/01/2024.
//
#include "JsonParse.h"
#include <fstream>
#include <sstream>

std::unique_ptr<Protein> parseJson(const std::string &protein)
{
	auto data = std::make_unique<Protein>();
	std::stringstream ss(protein);
	std::string temp, key, value;

	while (getline(ss, temp, ','))
	{
		std::stringstream pairStream(temp);
		getline(pairStream, key, ':');
		getline(pairStream, value, ':');
		value.erase(remove(value.begin(), value.end(), '"'), value.end());
		value.erase(remove(value.begin(), value.end(), '}'), value.end());
		value.erase(remove(value.begin(), value.end(), '{'), value.end());
		value.erase(remove(value.begin(), value.end(), ' '), value.end());

		if (key.find("id") != std::string::npos)
			data->setId(value);
		else if (key.find("sequence") != std::string::npos)
			data->setSequence(value);
		else if (key.find("reference") != std::string::npos)
			data->setReference(value);
	}
	return data;
}

std::vector<std::unique_ptr<Protein>> parseJsonArray(const std::string &jsonString)
{
	std::vector<std::unique_ptr<Protein>> dataArray;
	std::stringstream ss(jsonString);
	std::string token;

	while (getline(ss, token, '}'))
	{
		if (token.find('{') != std::string::npos)
		{
			token += '}';
			dataArray.push_back(parseJson(token));
		}
	}
	return dataArray;
}

std::string readFileIntoString(const std::string &path)
{
	std::ifstream input_file(path);
	if (!input_file.is_open())
	{
		std::cerr << "Could not open the file - '" << path << "'" << std::endl;
		return "";
	}
	return {(std::istreambuf_iterator<char>(input_file)), std::istreambuf_iterator<char>()};
}
