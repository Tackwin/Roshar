#pragma once
#include <unordered_map>
#include <functional>
#include <memory>
#include <string>
#include <thread>
#include <mutex>
#include <cmath>

#include "../Common.hpp"
#include "../Time/Clock.hpp"
#include "../Utils/UUID.hpp"

class TimerManager {
public:
	struct Function;
	using Callback = std::function<bool(double)>;

	struct Function {

		Clock clock;

		bool toRemove = false;
		bool paused = false;

		double timer;
		double time;

		double error = 0.;

		Callback f;

		Function() {};
		Function(double timer, const Callback& f)
			: clock(timer), timer(timer), time(timer), f(f) {
		};
		Function(double timer, const Callback&& f)
			: clock(timer), timer(timer), time(timer), f(f) {
		};
	};
private:

	static std::unordered_map<
		UUID, 
		Function
	> _functions;


public:

	static u32 getNFunction();

	static UUID addFunction(double timer, const Callback&& f);

	static void resetTimerFunction(UUID key);
	static void restartFunction(UUID key);
	static void removeFunction(UUID key);
	static void resumeFunction(UUID key);
	static void pauseFunction(UUID key);
	static void callFunction(UUID key);

	static bool functionsExist(UUID key);

	//ASSUREZ VOUS QUE LE POINTEUR RESTE VALIDE TOUT AU LONG DU PROCESSUS !!!!!!
	template<typename T = double>
	static UUID addPowerEase(double t, T* v, T min, T max, float p);
	template<typename T = double>
	static UUID addPowerOEase(
		double t, T* v, T min, T max, float p
	);
	template<typename T = double>
	static UUID addPowerIOEase(
		double t, T* v, T min, T max, float p
	);
	template<typename T = double>
	static UUID addCustomEase(
		double t, T* v, T min, T max, std::function<double(double)> f
	);
	template<typename T = double>
	static UUID addLinearEase(double t, T* v, T min, T max) {
		return TimerManager::addPowerEase<T>(t, v, min, max, 1);
	}
	template<typename T = double>
	static UUID addSquaredEase(double t, T* v, T min, T max){
		return TimerManager::addPowerEase<T>(t, v, min, max, 2);
	}
	template<typename T = double>
	static UUID addSquaredIOEase(double t, T* v, T min, T max){
		return TimerManager::addPowerIOEase<T>(t, v, min, max, 2);
	}
	template<typename T = double>
	static UUID addsinEase(double t, T* v, T min, T max);
	template<typename T = double>
	static UUID addsinOEase(double t, T* v, T min, T max);
	template<typename T = double>
	static UUID addsinIOEase(double t, T* v, T min, T max);

	static void update(double dt);
	static void updateInc(double dt, uint32_t n);
};

template<typename T>
UUID TimerManager::addPowerEase(
	double t, T* v, T min, T max, float p
) {
	return addCustomEase<T>(t, v, min, max, [p](double x)->double {return pow(x, p);});
}
template<typename T>
UUID TimerManager::addPowerOEase(
	double t, T* v, T min, T max, float p
) {
	return addCustomEase<T>(
		t, v, min, max, [p](double x)->double {return 1 - pow(1-x, p)}
	);
}
template<typename T>
UUID TimerManager::addPowerIOEase(
	double t, T* v, T min, T max, float p
) {
	return addCustomEase<T>(t, v, min, max, [p](double x)->double {
		return x < 0.5 ? pow(2*x, p) / 2. : 1 - pow(2 * (1 - x), p) / 2.;
	});
}
template<typename T>
UUID TimerManager::addsinEase(double t, T* V, T mmin, T max) {
	return TimerManager::addCustomEase<T>(t, v, min, max, [](double x)->double {
		return 1 - std::sin(C::PI * (1 - x) / 2.);
	});
}
template<typename T>
UUID TimerManager::addsinOEase(double t, T* V, T mmin, T max) {
	return TimerManager::addCustomEase<T>(t, v, min, max, [](double x)->double {
		return std::sin(C::PI * x / 2.);
	});
}
template<typename T>
UUID TimerManager::addsinIOEase(double t, T* V, T mmin, T max) {
	return TimerManager::addCustomEase<T>(t, v, min, max, [](double x)->double {
		return 0.5f * (x < 0.5 
			? (1 - std::sin(C::PI * (1 - 2 * x) / 2.f)) 
			: std::sin((x - 0.5) * C::PI));
	});
}
template<typename T>
UUID TimerManager::addCustomEase(
	double t, T* v, T min, T max, std::function<double(double)> pf
) {
	std::shared_ptr<Clock> clock = std::make_shared<Clock>(t);
	auto f = [t, v, min, max, pf, clock, intT = 0.f](double)mutable->bool {
		*v = static_cast<T>(min + (max - min) * pf(clock->elapsed() / t));
		if(clock->isOver()) {
			*v = max;
			return true;
		}
		return false;
	};
	return TimerManager::addFunction(0., f);
}

using TM = TimerManager;