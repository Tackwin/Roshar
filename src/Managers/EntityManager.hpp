#pragma once

#include <deque>

#define LIST_ENTITIES\
	X(Rock,               rocks)\
	X(Door,               doors)\
	X(Block,              blocks)\
	X(Torch,              torches)\
	X(Dry_Zone,           dry_zones)\
	X(Key_Item,           key_items)\
	X(Kill_Zone,          kill_zones)\
	X(Next_Zone,          next_zones)\
	X(Dispenser,          dispensers)\
	X(Projectile,         projectiles)\
	X(Moving_Block,       moving_blocks)\
	X(Trigger_Zone,       trigger_zones)\
	X(Prest_Source,       prest_sources)\
	X(Flowing_Water,      flowing_waters)\
	X(Friction_Zone,      friction_zones)\
	X(Auto_Binding_Zone,  auto_binding_zones)


#define X(T, t) struct T;
LIST_ENTITIES
#undef X

struct EntityManager {

	std::unordered_map<size_t, std::string> entity_debug_names;
	

#define X(T, t) std::deque<T> t;
LIST_ENTITIES
#undef X


	template<typename F>
	void for_every(F&& f) noexcept {
		#define X(T, t) f(t);
		LIST_ENTITIES;
		#undef X
	}


};