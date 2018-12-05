#pragma once

#include <cstdint>
#include <tuple>
#include <type_traits>
#include <vector>

#include "entities_groups.inl"
#include "components.inl"

///////////////////////////////////////////////////////////////////////////////

template <typename, typename>
struct ComponentIndex;

template <typename T, typename... Types>
struct ComponentIndex<T, std::tuple<T, Types...>>
{
    static constexpr uint64_t const INDEX = 0;
};

template <typename T, typename U, typename... Types>
struct ComponentIndex<T, std::tuple<U, Types...>>
{
    static constexpr uint64_t const INDEX = ComponentIndex<T, std::tuple<Types...>>::INDEX + 1;
};

template<typename T>
struct ComponentTraits
{
	static constexpr uint64_t const INDEX = ComponentIndex<T, Components>::INDEX;
	static constexpr uint64_t const MASK = 1 << INDEX;
};

template<typename...>
struct ComponentsMask
{
	static constexpr uint64_t const MASK = 0;
};

template<typename T, typename... Types>
struct ComponentsMask<T, Types...>
{
	static constexpr uint64_t const MASK = ComponentTraits<T>::MASK | ComponentsMask<Types...>::MASK;
};

template<typename...>
struct ComponentsList {};

template<EntitiesGroup...>
struct EntitiesGroupsList {};

template<EntitiesGroup Eg, EntitiesGroup... Egs>
struct	EntitiesGroupIterator
{
	static constexpr EntitiesGroup const CURRENT = Eg;
	using OTHERS = EntitiesGroupsList<Egs...>;
};

///////////////////////////////////////////////////////////////////////////////

class Entity final
{
	static constexpr uint32_t const ENTITIES_GROUP_OFFSET = 24;
	static constexpr uint32_t const VERSION_MASK = 0x00FFFFFF;
	static constexpr uint64_t const INVALID_ID = -1;

	union
	{
		uint64_t	_global_id;

		struct
		{
			union
			{
				EntitiesGroup	_entities_group;
				uint32_t	_version;
			};
			uint32_t	_local_id;
		};
	};

public:

	Entity() : _global_id(INVALID_ID)
	{}
	Entity(EntitiesGroup const entities_group, uint32_t const version, uint32_t const local_id)
		: _version((uint32_t)_entities_group << ENTITIES_GROUP_OFFSET | version)
		, _local_id(local_id)
	{}

	Entity(Entity const & entity) : _global_id(entity._global_id) {}
    Entity(Entity && entity) : _global_id(entity._global_id) {}

	Entity &	operator=(Entity const & entity) { _global_id = entity._global_id; return (*this); }
	Entity &	operator=(Entity && entity) { _global_id = entity._global_id; return (*this); }

	bool operator==(Entity const entity) const { return (_global_id == entity._global_id); }
	bool operator!=(Entity const entity) const { return (_global_id != entity._global_id); }

	bool	is_valid() const;

	template<typename... Types>
	bool	has() const;

	template<typename T>
	T & get() const;

    template<typename T>
    T & add() const;

    template<typename T>
    void remove() const;
};

///////////////////////////////////////////////////////////////////////////////

template<typename T>
class	ComponentManager final
{
	class MemoryPool final
	{
		size_t	_size;
		union
		{
			T *		_data;
			void *	_memory;
		};

	public:

		MemoryPool() : _size(0), _memory(nullptr) {}
		~MemoryPool() { ::operator delete(_memory); }

		MemoryPool(MemoryPool const &) = delete;
    	MemoryPool(MemoryPool &&) = delete;

		MemoryPool &	operator=(MemoryPool const &) = delete;
		MemoryPool &	operator=(MemoryPool &&) = delete;

		void resize(size_t const new_size)
		{
			_size = new_size;
			::operator delete(_memory);
			_memory = ::operator new(_size * sizeof(T));
		}

		void create(size_t const index) { _data[index].T(); }
		void destroy(size_t const index) { _data[index].~T(); }

		T & operator[](size_t const index) { return _data[index]; }
		T const & operator[](size_t const index) const { return _data[index]; }
	};


	inline static MemoryPool	_components[(size_t)EntitiesGroup::SIZE];

public:

	ComponentManager() = delete;
	~ComponentManager() = delete;

	ComponentManager(ComponentManager const &) = delete;
	ComponentManager(ComponentManager &&) = delete;

	ComponentManager &	operator=(ComponentManager const &) = delete;
	ComponentManager &	operator=(ComponentManager &&) = delete;

	static T & get(Entity const entity)
	{
		return (_components[(size_t)entity._entities_group][entity._local_id]);
	}

	static T & _get(uint32_t const local_id, EntitiesGroup const entities_group)
	{
		return (_components[(size_t)entities_group][local_id]);
	}

    static void _create(Entity const entity)
	{
        if constexpr (std::is_class<T>::value || std::is_union<T>::value)
		    _components[(size_t)entity._entities_group][entity._local_id].T();
	}

	static void	_destroy(Entity const entity)
	{
        if constexpr (std::is_class<T>::value || std::is_union<T>::value)
		    _components[(size_t)entity._entities_group][entity._local_id].~T();
	}

	static void	_create(Entity const entity, uint64_t const composition)
	{
        if constexpr (std::is_class<T>::value || std::is_union<T>::value)
            if (composition & ComponentTraits<T>::MASK)
		        _components[(size_t)entity._entities_group][entity._local_id].T();
	}

	static void	_destroy(Entity const entity, uint64_t const composition)
	{
        if constexpr (std::is_class<T>::value || std::is_union<T>::value)
            if (composition & ComponentTraits<T>::MASK)
		        _components[(size_t)entity._entities_group][entity._local_id].~T();
	}

    static void _resize(size_t const new_size, EntitiesGroup const entities_group)
    {
        _components[(size_t)entities_group].resize(new_size);
    }
};

///////////////////////////////////////////////////////////////////////////////

class EntityManager final
{
    template<typename>
    struct ForEachComponent;

    template<typename... Types>
    struct ForEachComponent<std::tuple<Types...>>
    {
        static void resize(size_t const new_size, EntitiesGroup const entities_group)
        {
            (ComponentManager<Types>::_resize(new_size, entities_group), ... );
        }

        static void create(Entity const entity, uint64_t const composition)
        {
            (ComponentManager<Types>::_destroy(entity, composition), ... );
        }

        static void destroy(Entity const entity, uint64_t const composition)
        {
            (ComponentManager<Types>::_destroy(entity, composition), ... );
        }
    };


	inline static uint32_t				_entities_number[(size_t)EntitiesGroup::SIZE];
	inline static std::vector<uint64_t>	_entities_composition[(size_t)EntitiesGroup::SIZE];
	inline static std::vector<uint64_t>	_entities_new_composition[(size_t)EntitiesGroup::SIZE];
	inline static std::vector<uint32_t>	_entities_version[(size_t)EntitiesGroup::SIZE];
	inline static std::vector<uint32_t>	_free_ids[(size_t)EntitiesGroup::SIZE];

public:

	EntityManager() = delete;
	~EntityManager() = delete;

	EntityManager(EntityManager const &) = delete;
	EntityManager(EntityManager &&) = delete;

	EntityManager &	operator=(EntityManager const &) = delete;
	EntityManager &	operator=(EntityManager &&) = delete;


	template<EntitiesGroup Sb>
	static Entity	create()
	{
		static_assert(Sb < EntitiesGroup::SIZE);

		if (!_free_ids[(size_t)Sb].empty())
		{
			uint32_t const local_id = _free_ids[(size_t)Sb].back();
		    uint32_t const version = _entities_version[(size_t)Sb][local_id];

            _free_ids[(size_t)Sb].pop_back();

		    return {Sb, version, local_id};
		}
        else
        {
            if (_entities_number[(size_t)Sb] >= _entities_version[(size_t)Sb].size())
            {
                size_t const new_size = _entities_version[(size_t)Sb].size() << 1;

                _entities_composition[(size_t)Sb].resize(new_size, 0);
                _entities_version[(size_t)Sb].resize(new_size, 0);

                ForEachComponent<Components>::resize(new_size, Sb);
            }

			uint32_t const local_id = _entities_number[(size_t)Sb]++;
		    uint32_t const version = _entities_version[(size_t)Sb][local_id];

		    return {Sb, version, local_id};
        }
	}

	static void	destroy(Entity const entity)
	{
		_entities_new_composition[(size_t)entity._entities_group][entity._local_id] = 0;
		++_entities_version[(size_t)entity._entities_group][entity._local_id];

		if (entity._local_id == _entities_number[(size_t)entity._entities_group] - 1)
			--_entities_number[(size_t)entity._entities_group];
		else
			_free_ids[(size_t)entity._entities_group].push_back(entity._local_id);
	}

	static bool	is_valid(Entity const entity)
	{
		return (entity._entities_group < EntitiesGroup::SIZE
			&& entity._local_id < _entities_number[(size_t)entity._entities_group]
			&& (entity._version & Entity::VERSION_MASK) == _entities_version[(size_t)entity._entities_group][entity._local_id]);
	}

	template<typename... Types>
	static bool	has(Entity const entity)
	{
		uint64_t const composition = _entities_composition[(size_t)entity._entities_group][entity._local_id];

		return ((composition & ComponentsMask<Types...>::MASK) == ComponentsMask<Types...>::MASK);
	}

    template<typename T>
    static T &  add(Entity const entity)
    {
        _entities_new_composition[(size_t)entity._entities_group][entity._local_id] |= ComponentTraits<T>::MASK;
        ComponentManager<T>::_create(entity);
        return (ComponentManager<T>::get(entity));
    }

    template<typename T>
    static void remove(Entity const entity)
    {
        _entities_new_composition[(size_t)entity._entities_group][entity._local_id] &= ~ComponentTraits<T>::MASK;
    }

	static void	update()
	{
		for (size_t eg = 0; eg < (size_t)EntitiesGroup::SIZE; ++eg)
		{
			size_t const en = _entities_number[eg];

			std::vector<uint64_t> const & entities_composition = _entities_composition[eg];
			std::vector<uint64_t> const & entities_new_composition = _entities_new_composition[eg];

			for (size_t i = 0; i < en; ++i)
			{
				uint64_t const diff = entities_composition[i] ^ entities_new_composition[i];
				//uint64_t const added = diff & entities_new_composition[i];
				uint64_t const removed = diff & entities_composition[i];

				//ForEachComponent<Components>::create(entity, added);
				ForEachComponent<Components>::destroy(entity, removed);

				entities_composition[i] = entities_new_composition[i];
			}
		}
	}
};

///////////////////////////////////////////////////////////////////////////////

inline bool	Entity::is_valid() const
{
	return (EntityManager::is_valid(*this));
}

template<typename... Types>
inline bool	Entity::has() const
{
	return (EntityManager::has<Types...>(*this));
}

template<typename T>
inline T &	Entity::get() const
{
	return (ComponentManager<T>::get(*this));
}

template<typename T>
inline T &	Entity::add() const
{
	return (EntityManager::add<T>(*this));
}

template<typename T>
inline void	Entity::remove() const
{
	EntityManager::remove<T>(*this);
}

///////////////////////////////////////////////////////////////////////////////

template<typename T>
struct FunctionTraits : public FunctionTraits<decltype(&T::operator())> {};

template<typename T, typename... Types>
struct	FunctionTraits<void (T::*)(Entity const, Types...) const>
{
	using COMPONENTSLIST = ComponentsList<typename std::remove_reference<Types>::type...>;
};

template<typename, typename>
struct	ForEach;

template<typename... Types>
struct	ForEach<ComponentsList<Types...>, EntitiesGroupsList<>>
{
	template<typename T>
    static void run(T && lambda) {}
};

template<typename... Types, EntitiesGroup... Egs>
struct	ForEach<ComponentsList<Types...>, EntitiesGroupsList<Egs...>>
{
	template<typename T>
	static void run(T && lambda)
	{
		constexpr uint64_t const wanted_composition = ComponentsMask<Types...>::MASK;
		constexpr EntitiesGroup const entities_group = EntitiesGroupIterator<Egs...>::CURRENT;
		static_assert(entities_group < EntitiesGroup::SIZE);

		uint32_t const entities_number = EntityManager::_entities_number[(size_t)entities_group];
		std::vector<uint64_t> const & entities_composition = EntityManager::_entities_composition[(size_t)entities_group];
		std::vector<uint32_t> const & entities_version = EntityManager::_entities_version[(size_t)entities_group];

		for (uint32_t i = 0; i < entities_number; ++i)
		{
			if ((entities_composition[i] & wanted_composition) == wanted_composition)
				lambda(Entity(entities_group, entities_version[i], i), ComponentManager<Types>::_get(i, entities_group)...);
		}

		ForEach<ComponentsList<Types...>, typename EntitiesGroupIterator<Egs...>::OTHERS>::run(std::forward<T>(lambda));
	}
};

template<EntitiesGroup... Egs, typename T>
inline void	for_each(T && lambda)
{
	ForEach<typename FunctionTraits<T>::COMPONENTSLIST, EntitiesGroupsList<Egs...>>::run(std::forward<T>(lambda));
}

///////////////////////////////////////////////////////////////////////////////
