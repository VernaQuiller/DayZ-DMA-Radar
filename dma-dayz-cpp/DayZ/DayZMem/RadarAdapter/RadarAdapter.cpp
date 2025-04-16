#pragma once
#include "RadarAdapter.h";
#include "../../Structs/Scoreboard.h"
#include <unordered_set>

extern ImVec2 g_mainPlayerScreenPos;
extern ImVec2 g_mainPlayerScreenPosBuffered;


// Constructor
DayZ::RadarAdapter::RadarAdapter(std::shared_ptr<DayZ::MemoryUpdater> memUpdater, std::shared_ptr<DMARender::RenderBridge> renderBridge)
	: memUpdater(memUpdater), renderBridge(renderBridge) {
	loadFavoriteSteamIDs("steamids.txt"); // Read "steamids.txt" for your favorite admins
	DayZ::Entity::initializeRareItems("rareitems.txt"); // Read "rareitems.txt" for rare itemlist, use TypeName e.g. CarBattery
}

// draw loot and dead entities with filters
void DayZ::RadarAdapter::drawLoot(DMARender::IGameMap* curMap, const DMARender::MapTransform& mTransform, const std::vector<std::shared_ptr<DayZ::Entity>>& entities)
{
	
	if (!renderBridge) return; // If the render bridge is not initialized, return

	bool showVehicles = renderBridge->shouldShowVehicles();
	bool showBoats = renderBridge->shouldShowBoats();
	bool showGrounditems = renderBridge->shouldShowGrounditems();
	bool showDeadAnimals = renderBridge->shouldShowDeadAnimals();
	bool showDeadPlayers = renderBridge->shouldShowDeadPlayers();
	bool showWeapons = renderBridge->shouldShowWeapons();
	bool showClothing = renderBridge->shouldShowClothing();
	bool showBackpacks = renderBridge->shouldShowBackpacks();
	bool showProxyMagazines = renderBridge->shouldShowProxyMagazines();
	bool showFood = renderBridge->shouldShowFood();
	bool showAmmo = renderBridge->shouldShowAmmo();
	bool showRare = renderBridge->shouldShowRare();



	for (auto const item : entities) {
		if (!item->isValid())
			continue;

		if (
			!(item->isWeapon() && showWeapons) &&
			!(item->isProxyMagazines() && showProxyMagazines) &&
			!(item->isBackpack() && showBackpacks) &&
			!(item->isClothing() && showClothing) &&
			!(item->isFood() && showFood) &&
			!(item->isAmmo() && showAmmo) &&
			!(item->isRare() && showRare) &&
			!(item->isGroundItem() && showGrounditems) &&
			!(item->isPlayer() && item->isDead && showDeadPlayers) &&
			!(item->isCar() && showVehicles) &&
			!(item->isBoat() && showBoats) &&
			!(item->isAnimal() && item->isDead && showDeadAnimals)
			) {
			continue;
		}

		std::string postFix = "";

		ImU32 textCol;

		// Color for Items and Dead players/animals
		if (item->isPlayer()) {
			textCol = IM_COL32(0, 255, 255, 255);
			postFix = " (Dead)";
		} else if (item->isAnimal()) {
			textCol = IM_COL32(0, 255, 0, 100);
			postFix = " (Dead)";
		} else if (item->isCar()) {
			textCol = IM_COL32(255, 0, 245, 200);
		} else if (item->isBoat()) {
			textCol = IM_COL32(255, 0, 245, 200);
		} else if (item->isRare()) {
			textCol = IM_COL32(23, 100, 128, 200);
		} else if (item->isBackpack()) {
			textCol = IM_COL32(175, 255, 0, 100);
		} else if (item->isClothing()) {
			textCol = IM_COL32(255, 255, 0, 200);
		} else if (item->isWeapon()) {
			textCol = IM_COL32(255, 117, 50, 180);
		} else if (item->isProxyMagazines()) {
			textCol = IM_COL32(255, 117, 50, 180);
		} else if (item->isFood()) {
			textCol = IM_COL32(174, 197, 191, 200);
		} else if (item->isAmmo()) {
			textCol = IM_COL32(255, 117, 50, 180);
		} else if (item->isGroundItem()) {
			textCol = IM_COL32(255, 255, 255, 200);

		}
		else
		{
			continue;
		}

		auto screenPos = WorldToRadar(curMap, mTransform, item->FutureVisualStatePtr->position);

		auto bestName = item->EntityTypePtr->getBestString();

		if (bestName) {
			drawBlip(screenPos, 4, textCol, 12, 1, { bestName->value + postFix });
		}

	}
}


void DayZ::RadarAdapter::drawAliveEntities(DayZ::Camera* camera, DMARender::IGameMap* curMap, const DMARender::MapTransform& mTransform, const std::vector<std::shared_ptr<DayZ::Entity>>& entities, Scoreboard* scoreboard)
{

	// neutralTransform for my followplayer fuction
	DMARender::MapTransform neutralTransform = mTransform;
	neutralTransform.dragOffsetX = 0;
	neutralTransform.dragOffsetY = 0;
	static ImVec2 lastValidScreenPos = ImVec2(0, 0);
	std::vector<std::string> MainPlayerPlayingNames = { "Jon" };


	for (auto ent : entities) {
		if (!ent->isValid())
			continue;
		if (ent->isDead)
			continue;

		ImU32 blipColor;
		float blipSize;

		bool ismainplayPlayer = false;
		
		// Check for Main player names
		std::vector<std::string> infoText;

		if (ent->isPlayer()) {
			auto ident = ent->getPlayerIdentity(scoreboard);
			if (ident && ident->PlayerName) {
				ismainplayPlayer = std::find(MainPlayerPlayingNames.begin(), MainPlayerPlayingNames.end(), ident->PlayerName->value) != MainPlayerPlayingNames.end();
			}
		}

		// ALIFE ENTITIES colors
		if (ismainplayPlayer) {
			blipColor = IM_COL32(255, 0, 255, 255); 
			blipSize = 6;
		} else if (ent->isPlayer()) {
			blipColor = IM_COL32(255, 0, 0, 255);
			blipSize = 6;
		} else if (ent->isAnimal()) {
			blipColor = IM_COL32(0, 255, 0, 130);
			blipSize = 4;
		}
		else {
			continue;
		}


		float dist = camera->InvertedViewTranslation.Dist(ent->FutureVisualStatePtr->position);


		// neutral MainPlayerPos
		ImVec2 neutralPos = WorldToRadar(curMap, neutralTransform, ent->FutureVisualStatePtr->position);
		// final "MainPlayerScreenPos"
		ImVec2 screenPos = WorldToRadar(curMap, mTransform, ent->FutureVisualStatePtr->position);

		// update global (static) Pos if MainPlayer
		if (ismainplayPlayer) {
			ImVec2 newPos = WorldToRadar(curMap, mTransform, ent->FutureVisualStatePtr->position);

			// try for smooth interpolation
			float smoothFactor = 0.3f;  // Anpassbar
			g_mainPlayerScreenPos = ImVec2(
				lastValidScreenPos.x + (neutralPos.x - lastValidScreenPos.x) * smoothFactor,
				lastValidScreenPos.y + (neutralPos.y - lastValidScreenPos.y) * smoothFactor
			);

			lastValidScreenPos = g_mainPlayerScreenPos;
		}
		if (ent->isAnimal()) {
			auto entBestStr = ent->EntityTypePtr->getBestString();
			if (entBestStr) {
				auto entName = std::string(ent->EntityTypePtr->getBestString()->value);
				infoText.push_back(entName);
			}
			/// infoText.push_back(std::format("{:.0f}m", dist)); show distance for animals
		}
		if (ent->isPlayer()) {
			auto ident = ent->getPlayerIdentity(scoreboard);
			if (ident && ident->PlayerName && !ismainplayPlayer) {
				infoText.push_back(ident->PlayerName->value);
			}
			//Add ArmaStringID
			//if (ident->SteamID && !ismainplayPlayer) {
			//infoText.push_back(std::format("SteamID: {}", ident->SteamID->value));
			//}

			if (ent->InventoryPtr->isHandItemValid && !ismainplayPlayer) {
				auto bestHandStr = ent->InventoryPtr->handItem->EntityTypePtr->getBestString();
				if (bestHandStr) {
					infoText.push_back(bestHandStr->value);
				}
				else {
					infoText.push_back("(Unknown)");
				}
			}
			if (!ismainplayPlayer) {
				infoText.push_back(std::format("{:.0f}m", dist));
			}
		}

		if (ent->isPlayer()) {
			drawBlipDirectional(screenPos, blipSize, blipColor, 14, 1, infoText, ent->FutureVisualStatePtr->getRotationCorrected());
		}
		else {
			drawBlip(screenPos, blipSize, blipColor, 13, 1, infoText);

		}
	}

}

void DayZ::RadarAdapter::drawPlayerList(DayZ::Camera* camera, Scoreboard* scoreboard) {
	static bool includeSlowEntities = false; // Checkbox-Status

	// load small table font
	ImGui::PushFont(tableFont);
	ImGui::Begin("Player List", nullptr, ImGuiWindowFlags_AlwaysAutoResize);

	// add checkbox for SlowEntityTable, experimenal to find invis admins
	ImGui::Checkbox("Include SlowEntityTable", &includeSlowEntities);

	// slow entity table
	std::set<uint32_t> slowEntityIDs;
	if (includeSlowEntities) {
		for (const auto& slowEntity : memUpdater->getSlowEntityTable()->resolvedEntities) {
			if (slowEntity->isPlayer() && slowEntity->isValid()) {
				slowEntityIDs.insert(slowEntity->NetworkID);
			}
		}
	}

	// Imgui table setup
	if (ImGui::BeginTable("PlayerTable", 2, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg)) {
		ImGui::TableSetupColumn("Distance");
		ImGui::TableSetupColumn("Player Name");
		//ImGui::TableSetupColumn("Steam ID");
		//ImGui::TableSetupColumn("Network ID");
		//ImGui::TableSetupColumn("Model Name");
		ImGui::TableHeadersRow();

		// different approach to iterate over entities tables
		auto processEntities = [&](const std::vector<std::shared_ptr<DayZ::Entity>>& entities) {
			for (auto& entity : entities) {
				if (!entity->isPlayer() || !entity->isValid()) continue;

				auto ident = entity->getPlayerIdentity(scoreboard);
				if (!ident) continue;

				// bad working validation
				const char* playerName = (ident->PlayerName && ident->PlayerName->value)
					? ident->PlayerName->value
					: "Unknown";
				const char* steamID = (ident->SteamID && ident->SteamID->value)
					? ident->SteamID->value
					: "Unknown";

				uint32_t networkID = entity->NetworkID;

				// Add Modelname
				//const char* modelName = (entity->EntityTypePtr && entity->EntityTypePtr->ModelName && entity->EntityTypePtr->ModelName->value)
				//	? entity->EntityTypePtr->ModelName->value
				//	: "Unknown";

				float distance = camera->InvertedViewTranslation.Dist(entity->FutureVisualStatePtr->position);

				// different color for SlowEntityTable
				bool isSlowEntity = slowEntityIDs.count(networkID) > 0;
				ImU32 rowColor = isSlowEntity ? IM_COL32(255, 255, 0, 255) : IM_COL32(255, 255, 255, 255);

				ImGui::TableNextRow(ImGuiTableRowFlags_None, 0);
				ImGui::PushStyleColor(ImGuiCol_Text, rowColor);

				// Distance
				ImGui::TableNextColumn();
				ImGui::Text("%dm", static_cast<int>(distance)); // Ohne Nachkommastelle

				// Player Name
				ImGui::TableNextColumn();
				ImGui::Text("%s", playerName);

				// Steam ID with clickable functionality
				//ImGui::TableNextColumn();
				//if (ImGui::Selectable(steamID, false, ImGuiSelectableFlags_SpanAllColumns)) {
				//	std::string steamLink = "https://steamcommunity.com/profiles/";
				//	steamLink += steamID;
				//	ImGui::SetClipboardText(steamLink.c_str()); // copy to clipboard
				//}

				// Network ID
				//ImGui::TableNextColumn();
				//ImGui::Text("%u", networkID);

				// Model Name
				//ImGui::TableNextColumn();
				//ImGui::Text("%s", modelName);

				//ImGui::PopStyleColor();
			}
			};

		
		processEntities(memUpdater->getNearEntityTable()->resolvedEntities);
		processEntities(memUpdater->getFarEntityTable()->resolvedEntities);

		ImGui::EndTable();
	}
	ImGui::PopFont(); // back to default font
	ImGui::End();
}

void DayZ::RadarAdapter::drawServerPlayerList(std::shared_ptr<DayZ::Scoreboard> scoreboard) {

	std::unordered_set<int> seenNetworkIDs;

		// table setup
	ImGui::PushFont(tableFont);
	ImGui::Begin("Server Player List", nullptr, ImGuiWindowFlags_AlwaysAutoResize);

	if (ImGui::BeginTable("ServerPlayerTable", 3, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg)) {
		ImGui::TableSetupColumn("#"); 
		ImGui::TableSetupColumn("Player Name");
		ImGui::TableSetupColumn("Steam ID");
		/*ImGui::TableSetupColumn("Network ID");*/
		ImGui::TableHeadersRow();

		int rowIndex = 1; // start from 1

		for (const auto& identity : scoreboard->resolvedIdentities) {
			if (!identity) continue;

			// bad working validation
			if (identity->NetworkID <= 9999 || 
				!identity->SteamID || !identity->SteamID->value ||
				strncmp(identity->SteamID->value, "7656", 4) != 0 ||
				!identity->PlayerName || !identity->PlayerName->value || !seenNetworkIDs.insert(identity->NetworkID).second) {
				continue; 
			}

			const char* playerName = identity->PlayerName->value;
			const char* steamID = identity->SteamID->value;
			uint32_t networkID = identity->NetworkID;

			// Check for favorite Steam IDs
			bool isFavorite = favoriteSteamIDs.count(steamID) > 0;
			ImU32 rowColor = isFavorite ? IM_COL32(0, 255, 0, 255) : IM_COL32(255, 255, 255, 255); // green for "admins"

			ImGui::TableNextRow();
			ImGui::PushStyleColor(ImGuiCol_Text, rowColor);

			// numbering
			ImGui::TableNextColumn();
			ImGui::Text("%d", rowIndex++);

			// Player Name
			ImGui::TableNextColumn();
			ImGui::Text("%s", playerName);

			// Steam ID with clickable functionality
			ImGui::TableNextColumn();
			if (ImGui::Selectable(steamID, false, ImGuiSelectableFlags_SpanAllColumns)) {
				std::string steamLink = "https://steamcommunity.com/profiles/";
				steamLink += steamID;
				ImGui::SetClipboardText(steamLink.c_str()); // copy to clipboard
			}

			// Network ID
			//ImGui::TableNextColumn();
			//ImGui::Text("%u", networkID);
		}

		ImGui::EndTable();
	}

	ImGui::PopFont();
	ImGui::End();
}



void DayZ::RadarAdapter::DrawOverlay(DMARender::IGameMap* curMap, const DMARender::MapTransform& mTransform)
{
	auto camera = memUpdater->getCamera();
	ImGui::PushFont(radarFont);

	// draw radar entities/items
	drawLoot(curMap, mTransform, memUpdater->getSlowEntityTable()->resolvedEntities);
	drawLoot(curMap, mTransform, memUpdater->getItemTable()->resolvedEntities);
	drawLoot(curMap, mTransform, memUpdater->getNearEntityTable()->resolvedEntities);
	drawLoot(curMap, mTransform, memUpdater->getFarEntityTable()->resolvedEntities);
	drawAliveEntities(camera.get(), curMap, mTransform, memUpdater->getFarEntityTable()->resolvedEntities, memUpdater->getScoreboard().get());
	drawAliveEntities(camera.get(), curMap, mTransform, memUpdater->getNearEntityTable()->resolvedEntities, memUpdater->getScoreboard().get());

	// draw localplayerlist
	if (renderBridge->shouldShowPlayerList()) {
		drawPlayerList(camera.get(), memUpdater->getScoreboard().get());
	}
	// draw globalplayerlist from scoreboard
	if (renderBridge->shouldShowServerPlayerList()) {
		drawServerPlayerList(memUpdater->getScoreboard());
	}
	ImGui::PopFont();
}

void DayZ::RadarAdapter::createFonts()
{
		ImFontConfig config;
		config.OversampleH = 3;
		config.OversampleV = 1;
		config.GlyphExtraSpacing.x = 1.0f;
		radarFont = ImGui::GetIO().Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\Arial.ttf", 32, &config);

		// add small font
		ImFontConfig configSmall;
		configSmall.SizePixels = 12.0f; // reduce size?
		tableFont = ImGui::GetIO().Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\Arial.ttf", 13, &configSmall);
}


void DayZ::RadarAdapter::loadFavoriteSteamIDs(const std::string& filePath) {
	std::ifstream file(filePath);
	if (!file.is_open()) {
		std::cerr << "Fehler: SteamID-Datei konnte nicht geöffnet werden: " << filePath << std::endl;
		return;
	}

	std::string line;
	while (std::getline(file, line)) {
		if (!line.empty()) {
			favoriteSteamIDs.insert(line); // Add "Admin"SteamIDs
		}
	}

	file.close();
}