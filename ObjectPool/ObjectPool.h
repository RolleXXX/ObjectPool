#include <queue>
#include <mutex>
#include <iostream>
#include <stdlib.h>
#include <list>
#include <algorithm>
#include <functional>
using namespace std;





//�������Ż�Ϊ���Զ��������������ж���Ҫʹ�ã��ʹ�free_list��ͷ������ȥһ������use_listָ��free_listͷ��
//��Ҫ�ͷ�ʱ����use_list������ָ���free_list��Ķ���ָ�������� 
//����������stl list���push_back�Ĺ����pop_front��������Ч�ʿ������

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
	//�ð�����ʹ�õ��ջأ�ûʹ�õ�Ҳ�����
	
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
	//������֤��ȫ
	lock_guard<mutex> lock(m_mutex);
	if (free_list.empty()) {
		allocate_chunk();
	}
	T* obj = free_list.front();
	free_list.pop_front();
	use_list.emplace_back(obj);//���뵽ʹ����������
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


