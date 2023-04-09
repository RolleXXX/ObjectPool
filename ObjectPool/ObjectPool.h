#include <queue>
#include <mutex>
#include <iostream>
#include <stdlib.h>
#include <list>
#include <algorithm>
#include <functional>
using namespace std;





//还可以优化为：自定义两条链表，当有对象要使用，就从free_list的头部给出去一个，用use_list指向free_list头部
//当要释放时，用use_list的链表指针把free_list里的对象指回来就行 
//这样避免了stl list里的push_back的构造和pop_front的析构，效率可以提高

template <class T>
class ObjectPool {
public:
	ObjectPool(size_t chunk_size=default_size);
	~ObjectPool();

	T* acquire_object();
	void release_object(T* obj);
private:
	void allocate_chunk();
	void destroy(T* obj);
	T* construct();
private:
	mutex m_mutex;
	list<T*>free_list;
	list<T*>use_list;

	static const size_t max_size = 1000;
	
	static const size_t default_size = 50;
	
};

template<class T>
ObjectPool<T>::ObjectPool(size_t chunk_size)
{
	if (chunk_size <= 0||chunk_size>max_size) {
		cout << "size is invalid" << endl;
		exit(1);
	}
	else {
		for (size_t i = 0; i < chunk_size; i++) {
			allocate_chunk();
		}
	}
}

template<class T>
ObjectPool<T>::~ObjectPool()
{
	//得把正在使用的收回，没使用的也得清除
	
	for (auto it = free_list.begin(); it != free_list.end(); it++) {
		destroy(*it);
	}


	for (auto it = use_list.begin(); it != use_list.end(); it++) {
		destroy(*it);
	}
}

template<class T>
T * ObjectPool<T>::acquire_object()
{
	//加锁保证安全
	lock_guard<mutex> lock(m_mutex);
	if (free_list.empty()) {
		allocate_chunk();
	}
	T* obj = free_list.front();
	free_list.pop_front();
	use_list.emplace_back(obj);//加入到使用链表里面
	return obj;
}

template<class T>
void ObjectPool<T>::release_object(T * obj)
{
	lock_guard<mutex> lock(m_mutex);
	auto it = find(use_list.begin(), use_list.end(), obj);
	free_list.emplace_back(*it);
	use_list.erase(it);

}

template<class T>
void ObjectPool<T>::allocate_chunk()
{
	T* new_obj = construct();
	free_list.push_back(new_obj);
}

template<class T>
void ObjectPool<T>::destroy(T * obj)
{
	delete obj;
}

template<class T>
T * ObjectPool<T>::construct()
{
	return new T;
}


