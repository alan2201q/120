#include <Geode/Geode.hpp>
#include <Geode/modify/CCApplication.hpp>
#include <Geode/modify/MenuLayer.hpp>

using namespace geode::prelude;

static double targetInterval() {
	auto fps = Mod::get()->getSettingValue<int64_t>("fps");
	if (fps < 1) fps = 60;
	return 1.0 / static_cast<double>(fps);
}

// El juego llama a setAnimationInterval(1/60) en varios puntos:
// lo interceptamos y forzamos nuestro valor.
class $modify(FPSApplication, CCApplication) {
	void setAnimationInterval(double interval) {
		CCApplication::setAnimationInterval(targetInterval());
	}
};

// Aplica el valor una vez al llegar al menu principal.
class $modify(FPSMenuLayer, MenuLayer) {
	bool init() {
		if (!MenuLayer::init()) return false;
		static bool applied = false;
		if (!applied) {
			applied = true;
			CCApplication::sharedApplication()->setAnimationInterval(targetInterval());
		}
		return true;
	}
};

$on_mod(Loaded) {
	// Reaplica al cambiar el ajuste, sin reiniciar.
	listenForSettingChanges<int64_t>("fps", [](int64_t) {
		CCApplication::sharedApplication()->setAnimationInterval(targetInterval());
	});
}
