#pragma once
#include <memory>
#include "../../IOverlay/IOverlay.h"
#include "../../IRadar/IRadar.h"
#include "MapManager/MapManager.h"
#include <d3d11.h>
namespace DMARender {
	class RenderBridge {
		std::shared_ptr<IOverlay> overlayPtr = nullptr;
		std::shared_ptr<IRadar> radarPtr = nullptr;
		std::shared_ptr<MapManager> mapManager;
		ID3D11Device** g_pd3dDevicePtr;

		bool showDeadPlayers = true;
		bool showVehicles = false;
		bool showGrounditems = true;
		bool showBoats = true;
		bool showDeadAnimals = true;
		bool showClothing = false;
		bool showWeapons = true;
		bool showProxyMagazines = true;
		bool showBackpacks = true;
		bool showRare = true;
		bool showFood = true;
		bool showAmmo = true;
		bool showPlayerList = true;
		bool showServerPlayerList = true;



	public:
		RenderBridge(ID3D11Device** g_pd3dDevice);
		void createFonts();
		void setOverlay(std::shared_ptr<IOverlay> ptr);
		std::shared_ptr<IOverlay> getOverlay();
		void setRadar(std::shared_ptr<IRadar> ptr);
		std::shared_ptr<IRadar> getRadar();
		void addMap(std::shared_ptr<IGameMap> map);
		std::shared_ptr<MapManager> getMapManager();

		// getter and setter methods for visibility settings
		bool shouldShowDeadPlayers() const { return showDeadPlayers; }
		bool shouldShowVehicles() const { return showVehicles; }
		bool shouldShowBoats() const { return showBoats; }
		bool shouldShowGrounditems() const { return showGrounditems; }
		bool shouldShowDeadAnimals() const { return showDeadAnimals; }
		bool shouldShowClothing() const { return showClothing; }
		bool shouldShowWeapons() const { return showWeapons; }
		bool shouldShowProxyMagazines() const { return showProxyMagazines; }
		bool shouldShowBackpacks() const { return showBackpacks; }
		bool shouldShowRare() const { return showRare; }
		bool shouldShowFood() const { return showFood; }
		bool shouldShowAmmo() const { return showAmmo; }
		bool shouldShowPlayerList() const { return showPlayerList; }
		bool shouldShowServerPlayerList() const { return showServerPlayerList; }





		void setShowPlayerList(bool value) { showPlayerList = value; }
		void setShowServerPlayerList(bool value) { showServerPlayerList = value; }
		void setShowDeadPlayers(bool value) { showDeadPlayers = value; }
		void setShowVehicles(bool value) { showVehicles = value; }
		void setShowBoats(bool value) { showBoats = value; }
		void setShowGrounditems(bool value) { showGrounditems = value; }
		void setShowDeadAnimals(bool value) { showDeadAnimals = value; }
		void setShowClothing(bool value) { showClothing = value; }
		void setShowWeapons(bool value) { showWeapons = value; }
		void setShowProxyMagazines(bool value) { showProxyMagazines = value; }
		void setShowBackpacks(bool value) { showBackpacks = value; }
		void setShowRare(bool value) { showRare = value; }
		void setShowFood(bool value) { showFood = value; }
		void setShowAmmo(bool value) { showAmmo = value; }



	};
}