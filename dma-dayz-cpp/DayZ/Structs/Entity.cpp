#pragma once
#include "Entity.h"
#include <fstream>
#include <iostream>

std::set<std::string> DayZ::Entity::rareItems;

void DayZ::Entity::loadRareItems(const std::string& filePath) {
	rareItems.clear();
	std::ifstream file(filePath);
	if (!file.is_open()) {
		std::cerr << "Warnung: Datei " << filePath << " konnte nicht ge�ffnet werden. Seltene Gegenst�nde werden nicht geladen.\n";
		return;
	}

	std::string line;
	while (std::getline(file, line)) {
		if (!line.empty()) {
			rareItems.insert(line);
		}
	}
	file.close();

	if (rareItems.empty()) {
		std::cerr << "Warnung: Datei " << filePath << " enth�lt keine g�ltigen Eintr�ge.\n";
	}
}