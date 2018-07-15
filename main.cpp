
#include <iostream>
#include <type_traits>
#include <vector>
#include <map>
#include <string>



//pre c++14 void_t definition:
template<class... Ts> struct make_void { typedef void type; };
template<class... Ts> using void_t = typename make_void<Ts...>::type;

//check for member helper structures

template<class, class = void>
struct hasMethodSize : std::false_type { };

template<class T>
struct hasMethodSize<T, void_t<decltype(&T::size)>> : std::true_type { };

template<class, class = void>
struct hasMethodPrintAllReservedItems : std::false_type { };

template<class T>
struct hasMethodPrintAllReservedItems<T, void_t<decltype(&T::printAllReservedItems)>> : std::true_type { };


template < template <typename...> class Template, typename T >
struct is_instantiation_of : std::false_type {};

template < template <typename... Arg> class Template, typename... Args >
struct is_instantiation_of< Template, Template<Args...> > : std::true_type {};

template <template <class T, T > class Templ, class P >
struct is_instantiation_of1 : std::false_type {};

template < template <typename T, T > class Template, class T, T val>
struct is_instantiation_of1 < Template, Template < T, val > > : std::true_type {};

template < template <typename...> class Template, typename T >
constexpr auto is_same_type() {
	return is_instantiation_of<Template, T>::value;
}
template <template <typename T, T > class Template, typename T>
constexpr auto is_same_type() {
	return is_instantiation_of1<Template, T>::value;
}

template<class T, class ...Args >
class Wrapper;

template <class T>
class SparseArray;

template <class PrewLevel, class ContainerType, class ContainedValueT>
class Wrapper<PrewLevel, ContainerType, ContainedValueT>
{
	template<class T, class ...Args>
	friend class Wrapper;
public:
	using size_type = size_t;
	Wrapper(PrewLevel* const prevLevel, ContainerType *const val,
		size_type index, bool isElementExist, ContainedValueT* cvoptype) :containerPointer(val), index(index),
		prevLevel(prevLevel), containedValue(cvoptype), isPrevLevelElementExist(isElementExist) {}
	template<class P>
	auto operator[](P index) {
		if constexpr(is_same_type<SparseArray, ContainerType>()
			&& is_same_type<SparseArray, ContainedValueT>()) {
			using nextLevelContainedType = decltype(containedValue->initialValue);
			bool elExist = false;
			nextLevelContainedType* nexLevelVal = &containedValue->initialValue;
			if (isPrevLevelElementExist&&containerPointer->cont[this->index].cont.find(index) != containerPointer->cont[this->index].cont.end()) {
				elExist = true;
				nexLevelVal = &containedValue->cont[index];
			}
			return Wrapper<Wrapper<PrewLevel, ContainerType, ContainedValueT>, ContainedValueT, nextLevelContainedType>(this, containedValue, index, elExist, nexLevelVal);
		}
		else {
			return 0;
		}
	}
	Wrapper<PrewLevel, ContainerType, ContainedValueT>& operator=(ContainedValueT const& rhs) {
		if (isPrevLevelElementExist) {
			if (containerPointer->initialValue!=rhs) {
				*containedValue = rhs;
			}
			else {
				//удаление элемента
				containerPointer->cont.erase(index);
				*containedValue = containerPointer->initialValue;
			}
		}
		else {
			if (rhs!= containerPointer->initialValue) {
				containerPointer = prevLevel->assignment();
				isPrevLevelElementExist = true;
				if constexpr (is_same_type<SparseArray, ContainerType>()) {
					containerPointer->cont[index] = rhs;
					containedValue = &containerPointer->cont[index];
				}
				else {
					(*containerPointer)[index] = rhs;
				}
			}
		}
		return *this;
	}
	ContainedValueT& operator*() const {
		return *containedValue;
	}

private:
	ContainedValueT * const assignment() {
		if (!isPrevLevelElementExist) {
			containerPointer = prevLevel->assignment();
			isPrevLevelElementExist = true;
			containerPointer->cont[index] = *containedValue;
		}
		return &containerPointer->cont[index];
	}
	bool isPrevLevelElementExist = false;
	ContainedValueT * containedValue;
	ContainerType * containerPointer;
	PrewLevel * prevLevel;
	size_t index;
};

template <class PrewLevel, class ContainerType, class ContainedValueT>
std::ostream& operator<<(std::ostream& ostream, Wrapper<PrewLevel, ContainerType, ContainedValueT> const& val) {
	ostream << *val;
	return ostream;
}

template <class ContainerType, class ContainedValueT>
class Wrapper<ContainerType, ContainedValueT>
{
	template<class T, class ...Args>
	friend class Wrapper;
public:
	using size_type = size_t;
	Wrapper(ContainerType *const val, size_type index, bool isPrevLevelElementExist, ContainedValueT* a) : val(val),
		index(index), isCurrentLevelElementExist(isPrevLevelElementExist), containedValue(a) {}
	template<typename P>
	auto operator[](P index) {
		if constexpr(is_same_type<SparseArray, ContainedValueT>()) {
			using nextLevelContainedType = decltype(containedValue->initialValue);
			bool nexLevelElementExist = false;
			auto nexLevelVal = &containedValue->initialValue;
			if (isCurrentLevelElementExist&&val->cont[this->index].cont.find(index) != val->cont[this->index].cont.end()) {
				nexLevelElementExist = true;
				nexLevelVal = &containedValue->cont[index];
			}
			return Wrapper<Wrapper<ContainerType, ContainedValueT>,
				ContainedValueT,
				nextLevelContainedType>(this, containedValue, index, nexLevelElementExist, nexLevelVal);
		}
		else {
			std::cout << "err";
			return 0;
		}
	}
	Wrapper<ContainerType, ContainedValueT>& operator=(ContainedValueT const& rhs) {
		if (!isCurrentLevelElementExist) {
			containedValue = val->cont[index];
			return *this;
		}
		if (rhs != val->initialValue) {
			val->cont[index] = rhs;
		}
		else {
			isCurrentLevelElementExist = false;
			val->cont.eraese(index);
		}
		return *this;
	}
	auto operator*() {
		return *containedValue;
	}

private:
	ContainedValueT * const assignment() {
		if (!isCurrentLevelElementExist) {
			val->cont[index] = *containedValue;
			isCurrentLevelElementExist = true;
		}
		return &val->cont[index];
	}
	ContainedValueT* containedValue;
	bool isCurrentLevelElementExist = false;
	ContainerType * val;
	size_t index;
};

template<class T>
class SparseArray {
	using WrapType = Wrapper<SparseArray<T>, T>;
	template<class U, class ...Args>
	friend class Wrapper;
	template<class U>
	friend class SparseArray;
public:
	using size_type = size_t;
	SparseArray(T initialValue = T{}) :initialValue(initialValue) {}
	WrapType operator[](size_type const index) {
		if (cont.find(index) != cont.end())
			return WrapType(this, index, true, &cont[index]);
		else
			return WrapType(this, index, false, &initialValue);
	}
	size_t size() {
		size_t size = 0;
		if constexpr (hasMethodSize<T>::value) {
			for (auto& i : cont) {
				size += i.second.size();
			}
			return size;
		}
		else {
			return cont.size();
		}
	}
	void printAllReservedItems() {
		if constexpr(hasMethodPrintAllReservedItems<T>::value) {
			for (auto &i : cont) {
				auto a = "index " + std::to_string(i.first) + " ";
				i.second.printAllReservedItems1(a);
			}
		}
		else {
			std::cout << "err" << std::endl;
		}

	}
	std::map<size_type, T> cont;
private:
	void printAllReservedItems1(std::string& hlIndex) {
		if constexpr(hasMethodPrintAllReservedItems<T>::value) {
			for (auto &i : cont) {
				hlIndex += std::to_string(i.first) + " ";
				i.second.printAllReservedItems1(hlIndex);
			}
		}
		else {
			for (auto &i : cont)
				std::cout << hlIndex << i.first << " value " << i.second << " " << std::endl;
		}
	}
	T initialValue;
};


using Matrix = SparseArray<SparseArray<int>>;
using Matrix3d = SparseArray<SparseArray<SparseArray<int>>>;
Matrix a{ SparseArray<int>(0) };
Matrix3d b{ Matrix{ SparseArray<int>{10} } };

int main()
{
	for (int i = 0; i < 10; i++) {
		a[i][i] = i;
		a[i][9 - i] = 9 - i;
	}
	std::cout << "matrix: " << std::endl;
	for (int i = 1; i < 9; i++) {
		for (int j = 1; j < 9; j++) {
			std::cout << a[i][j] << " ";
		}
		std::cout << std::endl;
	}
	std::cout << "Count of reserved elements = " << a.size() << std::endl;
	a.printAllReservedItems();
	for (int i = 0; i < 10; i++) {
		a[i][i] = 0;
		a[i][9 - i] = 0;
	}
	std::cout << "matrix: " << std::endl;
	for (int i = 1; i < 9; i++) {
		for (int j = 1; j < 9; j++) {
			std::cout << a[i][j] << " ";
		}
		std::cout << std::endl;
	}
	std::cout << "Count of reserved elements = " << a.size() << std::endl;
	a.printAllReservedItems();
	return 0;
}