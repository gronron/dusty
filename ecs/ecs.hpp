#pragma once

#include <cstdint>
#include <functional>
#include <type_traits>
#include <vector>
#include "pdqsort.hpp"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template<typename...>
struct  ComponentsList final
{
};

template<typename, typename>
struct  ConcatComponentsList final
{
};

template<typename... Types1, typename... Types2>
struct  ConcatComponentsList<ComponentsList<Types1...>, ComponentsList<Types2...>> final
{
    using type = ComponentsList<Types1..., Types2...>;
};

template<typename, typename...>
struct  ComponentsListHasType final
{
    static constexpr bool VALUE = false;
};

template<typename T, typename... Types>
struct  ComponentsListHasType<T, ComponentsList<Types...>> final
{
    static constexpr bool VALUE = ((std::is_same_v<T, Types> || std::is_same_v<std::remove_const_t<T>, Types>) || ...);
};

template <typename, typename>
struct ComponentIndex;

template <typename T, typename... Types>
struct ComponentIndex<T, ComponentsList<T, Types...>>
{
    static constexpr uint64_t INDEX = 0;
};

template <typename T, typename U, typename... Types>
struct ComponentIndex<T, ComponentsList<U, Types...>>
{
    static constexpr uint64_t INDEX = ComponentIndex<T, ComponentsList<Types...>>::INDEX + 1;
};

#include "components.inl"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class   Entity final
{
    union
    {
        uint64_t _unique_id;

        struct
        {
            uint32_t	_version;
            uint32_t    _index;
        };
    };

public:

    static constexpr uint64_t	INVALID_ID = -1;
    static constexpr uint32_t	INVALID_INDEX = -1;

    Entity() : _unique_id(INVALID_ID) { }

    Entity(uint32_t const version, uint32_t const index) : _version(version), _index(index) { }

    Entity(Entity const & entity) : _unique_id(entity._unique_id) {}
    Entity(Entity && entity) : _unique_id(entity._unique_id) {}

    Entity &	operator=(Entity const &entity)
    {
        _unique_id = entity._unique_id;
        return *this;
    }

    Entity &	operator=(Entity &&entity)
    {
        _unique_id = entity._unique_id;
        return *this;
    }

    bool operator==(Entity const entity) const { return _unique_id == entity._unique_id; }
    bool operator!=(Entity const entity) const { return _unique_id != entity._unique_id; }

    uint32_t	version() const { return _version; }
    uint32_t    index() const { return _index; }

    bool	is_valid() const;

    template <typename... Types>
    bool	has() const;

    template <typename T>
    void	add() const;

    template <typename T>
    void	remove() const;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template<typename T>
class   MemoryPool final
{
    size_t	_size;
    union
	{
        T *		_data;
        void *	_memory;
    };

public:

    MemoryPool() : _size(0), _memory(nullptr) { }
    ~MemoryPool() { ::operator delete(_memory); }

    MemoryPool(MemoryPool const &) = delete;
    MemoryPool(MemoryPool &&) = delete;

    MemoryPool &	operator=(MemoryPool const &) = delete;
    MemoryPool &	operator=(MemoryPool &&) = delete;

	size_t	size() const { return _size; }

    void	resize(size_t const new_size)
    {
        _size = new_size;
        ::operator delete(_memory);
        _memory = ::operator new(_size * sizeof(T));
    }

    void    create(size_t const index) { new (_data + index) T(); }
    void    destroy(size_t const index) { _data[index].~T(); }

    T &			operator[](size_t const index) { return _data[index]; }
    T const &	operator[](size_t const index) const { return _data[index]; }
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class   EntitiesRegistry;

template<bool, typename>
struct  ForEach;

template<typename T>
class   ComponentsRegistry final
{
    friend class EntityRegistry;

    template<bool, typename>
    friend struct ForEach;

    friend Entity;
    friend EntitiesRegistry;

    inline static std::vector<uint32_t> _entities_to_components;
    inline static std::vector<uint32_t> _components_to_entities;
    inline static MemoryPool<T>         _components;

    inline static std::vector<uint32_t> _components_to_create;
    inline static std::vector<uint32_t> _components_to_destroy;


    ComponentsRegistry() = delete;
    ~ComponentsRegistry() = delete;

    ComponentsRegistry(ComponentsRegistry const &) = delete;
    ComponentsRegistry(ComponentsRegistry &&) = delete;

    ComponentsRegistry &operator=(ComponentsRegistry const &) = delete;
    ComponentsRegistry &operator=(ComponentsRegistry &&) = delete;

    static bool has(uint32_t const entity_index)
    {
        return _entities_to_components[entity_index] != Entity::INVALID_INDEX;
    }

	static T &	get(uint32_t const entity_index)
    {
        return _components[_entities_to_components[entity_index]];
    }

    static void	create(uint32_t const entity_index)
    {
        _components_to_create.push_back(entity_index);
    }

    static void destroy(uint32_t const entity_index)
    {
		_components_to_destroy.push_back(entity_index);
    }

	static void checked_destroy(uint32_t const entity_index)
	{
		if (has(entity_index))
			_components_to_destroy.push_back(entity_index);
	}

    static void resize(size_t const new_size)
    {
		_entities_to_components.resize(new_size, Entity::INVALID_INDEX);
    }

    static void end_frame_process()
    {
        if (!_components_to_destroy.empty())
        {
            pdqsort(_components_to_destroy.begin(), _components_to_destroy.end(), std::less<uint32_t>());

            size_t size = _components_to_entities.size();
            size_t offset = 0;
            size_t j = 0;
            size_t i = _components_to_entities[_components_to_destroy[0]];
			do
            {
                if (_components_to_entities[i] == _components_to_destroy[j])
                {
                    _components.destroy(i);
                    _entities_to_components[i] = Entity::INVALID_INDEX;
                    ++offset;
                    --size;

                    size_t const source = i + offset;
                    _components_to_entities[i] = _components_to_entities[source];
                    _components[i] = _components[i + offset];
                }
                else
                {
                    size_t const source = i + offset;
                    _components_to_entities[i] = _components_to_entities[source];
                    _components[i] = _components[source];
                    ++i;
                }
            }
			while (i < size);

            _components_to_entities.resize(_components_to_entities.size() - _components_to_destroy.size());
            _components_to_destroy.clear();
        }

        if (!_components_to_create.empty())
        {
			size_t const old_size = _components_to_entities.size();
			_components_to_entities.resize(_components_to_entities.size() + _components_to_create.size(), Entity::INVALID_INDEX);
			if (_components.size() < _components_to_entities.size())
				_components.resize(_components_to_entities.size());

			pdqsort(_components_to_create.begin(), _components_to_create.end(), std::less<uint32_t>());

            size_t offset = _components_to_create.size();
            size_t j = offset - 1;
            size_t i = old_size;
			do
            {
                if (_components_to_entities[i] > _components_to_create[j])
                {
                    size_t const destination = i + --offset;

                    _entities_to_components[_components_to_create[j]] = static_cast<uint32_t>(destination);
                    _components_to_entities[destination] = _components_to_create[j];
                    _components.create(destination);

                    --j;
                }
                else
                {
                    size_t const destination = i - offset - 1;
                    _components_to_entities[destination] = _components_to_entities[i];
                    _components[destination] = _components[i];
                    ++i;
                }
            }
			while (offset > 0);

			_components_to_create.clear();
        }
    }
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class   System
{
public:
    virtual void    creation_callback(Entity const creator, Entity const created) { }
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class   EntitiesRegistry final
{
    template<typename>
    friend class ComponentsRegistry;

    template<bool, typename>
    friend struct ForEach;

    template<typename>
    struct ForEachComponent;

    template<typename... Types>
    struct ForEachComponent<ComponentsList<Types...>> final
    {
        static void resize(size_t const new_size)
        {
            (ComponentsRegistry<Types>::resize(new_size), ...);
        }

        static void create(uint32_t const entity_index)
        {
            (ComponentsRegistry<Types>::create(entity_index), ...);
        }

        static void destroy(uint32_t const entity_index)
        {
            (ComponentsRegistry<Types>::checked_destroy(entity_index), ...);
        }
        
        static void end_frame_process()
        {
            (ComponentsRegistry<Types>::end_frame_process(), ...);
        }
    };

    struct  CreationCallback final
    {
        System * const  system;
        Entity          creator;
    };

    inline static uint32_t                      _entities_number = 0;
    inline static std::vector<uint32_t>         _entities_version;
    inline static std::vector<uint32_t>         _free_ids;
    inline static std::vector<CreationCallback> _entities_to_create;
    inline static std::vector<uint32_t>         _entities_to_destroy;

public:
	EntitiesRegistry() = delete;
    ~EntitiesRegistry() = delete;

    EntitiesRegistry(EntitiesRegistry const &) = delete;
    EntitiesRegistry(EntitiesRegistry &&) = delete;

    EntitiesRegistry &operator=(EntitiesRegistry const &) = delete;
    EntitiesRegistry &operator=(EntitiesRegistry &&) = delete;

    static void create(System * const system, Entity const creator)
    {
        _entities_to_create.push_back({ system, creator });
    }

    static void destroy(Entity const entity)
    {
        _entities_to_destroy.push_back(entity.index());
    }

    static bool is_valid(Entity const entity)
    {
        return entity.index() < _entities_number
            && entity.version() == _entities_version[entity.index()];
    }

    template<typename... Types>
	static bool has(uint32_t const entity_index)
    {
        return (ComponentsRegistry<Types>::has(entity_index) & ...);
    }

    template<typename... Types>
    static bool has(Entity const entity)
    {
        return has<Types...>(entity.index());
    }

    template<typename T>
    static void add(Entity const entity)
    {
        ComponentsRegistry<T>::create(entity);
    }

    template<typename T>
    static void remove(Entity const entity)
    {
        ComponentsRegistry<T>::destroy(entity);
    }

    static void end_frame_process()
    {
		size_t const to_destroy_size = _entities_to_destroy.size();
        for (size_t i = 0; i < to_destroy_size; ++i)
        {
            uint32_t const index = _entities_to_destroy[i];
            ForEachComponent<Components>::destroy(index);
            ++_entities_version[index];

            _free_ids.push_back(index);
        }
        _entities_to_destroy.clear();

		size_t const to_create_size = _entities_to_create.size();

		if (to_create_size > _free_ids.size())
		{
			uint32_t new_size = static_cast<uint32_t>(_entities_version.size() + to_create_size);

            --new_size;
			new_size |= new_size >> 1 | new_size >> 2 | new_size >> 4 | new_size >> 8 | new_size >> 16;
			++new_size;

			_entities_version.resize(new_size, 0);

			for (uint32_t i = 0; i < new_size; ++i)
				_free_ids.push_back(i);

			_entities_number = new_size;

			ForEachComponent<Components>::resize(new_size);
		}

		pdqsort(_free_ids.begin(), _free_ids.end(), std::less<uint32_t>());

        for (size_t i = 0; i < to_create_size; ++i)
        {
            CreationCallback cb = _entities_to_create[i];

            uint32_t const local_id = _free_ids[i];
            uint32_t const version  = _entities_version[local_id];

            cb.system->creation_callback(cb.creator, { version, local_id });
        }
		_free_ids.erase(_free_ids.begin(), _free_ids.begin() + _entities_to_create.size());
        _entities_to_create.clear();

        ForEachComponent<Components>::end_frame_process();
    }
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

inline bool	Entity::is_valid() const
{
    return EntitiesRegistry::is_valid(*this);
}

template<typename... Types>
inline bool Entity::has() const
{
    return EntitiesRegistry::has<Types...>(*this);
}

template<typename T>
inline void Entity::add() const
{
    ComponentsRegistry<T>::create(this->index());
}

template<typename T>
inline void Entity::remove() const
{
    ComponentsRegistry<T>::destroy(this->index());
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template<typename... Types>
class   Accessor final
{
    using Dependencies = ComponentsList<Types...>;

    template<bool, typename>
    friend ForEach;

    Accessor();
    ~Accessor();

    Accessor(Accessor const &) = delete;
    Accessor(Accessor &&) = delete;

    Accessor &operator=(Accessor const &) = delete;
    Accessor &operator=(Accessor &&) = delete;

public:

    template<typename T>
    T * get(Entity const & entity)
    {
        static_assert(ComponentsListHasType<T, Dependencies>::VALUE);
        return ComponentsRegistry<typename std::remove_const<T>::type>::get(entity);
    }

    template<typename T>
    T const * get(Entity const & entity) const
    {
        static_assert(ComponentsListHasType<T, Dependencies>::VALUE);
        return ComponentsRegistry<typename std::remove_const<T>::type>::get(entity);
    }
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct Dependency final
{
    bool        _is_for_writting : 1;
    uint32_t    _components_index : 31;
};



template<typename... Types>
class   DependenciesRegisterer final
{
    class   Oui final
    {
    public:

        Oui()
        {
            constexpr Dependency dependencies[] = { { std::is_const_v<Types>, ComponentIndex<Types, Components>::INDEX }, ... };
            constexpr size_t size = sizeof(dependencies) / sizeof(int);

            DependenciesSolver::register_dependencies(size, dependencies);
        }

        Oui(Oui const &) = delete;
        Oui(Oui &&) = delete;

        Oui &operator=(Oui const &) = delete;
        Oui &operator=(Oui &&) = delete;
    };

    static inline Oui;

public:

    DependenciesRegisterer() = default;

    DependenciesRegisterer(DependenciesRegisterer const &) = delete;
    DependenciesRegisterer(DependenciesRegisterer &&) = delete;

    DependenciesRegisterer &operator=(DependenciesRegisterer const &) = delete;
    DependenciesRegisterer &operator=(DependenciesRegisterer &&) = delete;
};

class   DependenciesSolver final
{
    struct  Dependencies final
    {
        int                     _system_index;
        std::vector<Dependency> _components;
    };

    inline static std::vector<Dependencies> _dependencies;

    static void register_dependencies(size_t const size, Dependency const * const dependencies)
    {
        for (size_t i = 0; i < size; ++i)
        {

        }
    }

    static bool    are_independent(Dependencies const & x, Dependencies const & y)
    {
        size_t const size_x =  x._components.size();
        size_t const size_y =  y._components.size();

        for (size_t j = 0; j < size_y; ++j)
        {
            for (size_t i = 0; i < size_x; ++i)
            {
                if (x._component[i]._components_index == y._component[j]._components_index
                    && (x._component[i]._is_for_writting || y._component[j]._is_for_writting))
                    return false;
            }
        }
        return true;
    }

    void    print_dependencies() const
    {

    }
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template<typename T>
struct  FunctionTraits : public FunctionTraits<decltype(&T::operator())>
{
};

template<typename T, typename... Types>
struct  FunctionTraits<void (T::*)(Types...) const>
{
    using DEPENDENCIES = ComponentsList<std::remove_reference_t<Types>...>;
    using COMPONENTSLIST = ComponentsList<std::decay_t<Types>::type...>;
    static constexpr bool   HAS_ENTITY_AS_PARAM = false;
};

template<typename T, typename... Types>
struct  FunctionTraits<void (T::*)(Entity const, Types...) const>
{
    using DEPENDENCIES = ComponentsList<std::remove_reference_t<Types>...>;
    using COMPONENTSLIST = ComponentsList<std::decay_t<Types>...>;
    static constexpr bool	HAS_ENTITY_AS_PARAM = true;
};

template<typename T, typename... Types>
struct  FunctionTraits<void (T::*)(Accessor<Types2...> const, Types...) const>
{
    using DEPENDENCIES = ConcatComponentsList<>;
    using COMPONENTSLIST = ComponentsList<std::decay_t<Types>::type...>;
    static constexpr bool	HAS_ENTITY_AS_PARAM = false;
};

template<typename T, typename... Types>
struct  FunctionTraits<void (T::*)(Accessor<Types2...> const, Entity const, Types...) const>
{
    using DEPENDENCIES = ComponentsList<std::remove_reference_t<Types>...>;
    using COMPONENTSLIST = ComponentsList<std::decay_t<Types>...>;
    static constexpr bool	HAS_ENTITY_AS_PARAM = true;
};

template<bool HAS_ENTITY_AS_PARAM, typename... Types>
struct  ForEach<HAS_ENTITY_AS_PARAM, ComponentsList<Types...>>
{
    template <typename T>
	static void run(T && lambda)
    {
        std::vector<uint32_t> const * const components_to_enties_by_type[] = { &ComponentsRegistry<Types>::_components_to_entities... };

        size_t	best = 0;
        size_t	minimum = -1;
        for (size_t i = 0; i < sizeof...(Types); ++i)
        {
            if (components_to_enties_by_type[i]->size() < minimum)
            {
                minimum = components_to_enties_by_type[i]->size();
                best = i;
            }
        }

        std::vector<uint32_t> const &	entities = *components_to_enties_by_type[best];

		size_t const size = entities.size();
        for (uint32_t i = 0; i < size; ++i)
        {
            uint32_t const index = entities[i];
            if (EntitiesRegistry::has<Types...>(index))
            {
                if constexpr (HAS_ENTITY_AS_PARAM)
                    lambda(Entity(EntitiesRegistry::_entities_version[index], index), ComponentsRegistry<Types>::get(index)...);
                else
                    lambda(ComponentsRegistry<Types>::get(index)...);
            }
        }
    }
};

template<typename T>
inline void for_each(T && lambda)
{
    using TRAITS = FunctionTraits<T>;

    DependenciesRegisterer<TRAITS::DEPENDENCIES> dr;

    ForEach<TRAITS::HAS_ENTITY_AS_PARAM, typename TRAITS::COMPONENTSLIST>::run(std::forward<T>(lambda));
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
