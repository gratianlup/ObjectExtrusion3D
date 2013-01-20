// Copyright (c) Gratian Lup. All rights reserved.
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are
// met:
//
//     * Redistributions of source code must retain the above copyright
//       notice, this list of conditions and the following disclaimer.
//     * Redistributions in binary form must reproduce the above
//       copyright notice, this list of conditions and the following
//       disclaimer in the documentation and/or other materials provided
//       with the distribution.
//     * Neither the name of Google Inc. nor the names of its
//       contributors may be used to endorse or promote products derived
//       from this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
// OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

#ifndef LIST_HPP
#define LIST_HPP

#include "ISerializable.hpp"
#include "Stream.hpp"
#include <cstdlib>
#include <cassert>
#include <algorithm>

template <class T>
class List : public ISerializable {
private:
	static const size_t DEFAULT_CAPACITY;

	//
	// Membrii.
	//
	T* array_;
	size_t count_;
	size_t capacity_;
	
public:
	//
	// Constructors / destructor.
	//
	List() : array_(new T[DEFAULT_CAPACITY]), count_(0),
			 capacity_(DEFAULT_CAPACITY) {}

	List(size_t capacity) : array_(new T[capacity]), count_(0),
							capacity_(capacity) {}
	
	List(T* items, size_t count) : array_(new T[count]),
									count_(count), capacity_(count) {
		assert(items != null);
		// --------------------------------
		memcpy(array_, items, count * sizeof(T));
	}
	
	List(const List &other) : array_(new T[other.count_]),
							   count_(other.count_), 
							   capacity_(other.count_) {
		memcpy(array_, other.array_, count_ * sizeof(T));
	}
	
	~List() {
		delete[] array_;
	}

	//
	// Public methods.
	//
	void Add(const T &item) {
		EnsureSpace(count_ + 1);
		array_[count_++] = item;
	}
	
	void Add(T* items, int count) {
		assert(items != NULL);
		// --------------------------------
		EnsureSpace(count_ + count);
		memcpy(&array_[count_], items, count * sizeof(T));
		count_ += count;
	}
	
	void Add(const List &other) {
		Add(other.array_, other.count_);
	}
	
	size_t Capacity() const { 
		return capacity_; 
	}
	
	void Clear() {
		count_ = 0;
	}
	
	bool Contains(const T &other) const {
		for(size_t i = 0; i < count_; i++) {
			if(array_[i] == other) return true;
		}
		
		return false;
	}
	
	size_t Count() const { 
		return count_; 
	}
	
	void Insert(const T &item, size_t index) {
		assert(index <= count_);
		// ---------------------------------
		EnsureSpace(count_ + 1);
		
		if(index == count_) {
			// Inserare la sfarsit.
			array_[count_++] = item;
		}
		else {
			// Deplaseaza la dreapta cu 1 pozitie punctele
			// de dupa pozitia de inserare.
			memmove(&array_[index + 1], &array_[index], (count_ - index) * sizeof(T));
			array_[index] = item;
			count_++;
		}
	}
	
	void Remove(size_t index) {
		assert(index < count_);
		// --------------------------------
		if(index == (count_ - 1)) {
			// Ultimul punct.
			count_--;
		}
		else {
			memmove(&array_[index], &array_[index + 1], (count_ - index) * sizeof(T));
			count_--;
		}
	}
	
	void Remove(const T &item) {
		for(size_t i = 0; i < count_; i++) {
			if(array_[i] == item) {
				Remove(i);
				break;
			}
		}
	}

	//
	// Serialization.
	//
	virtual void Serialize(Stream &stream) const {
		// Scrie numarul de element, apoi fiecare element in parte.
		stream.Write(count_);

		for(size_t i = 0; i < count_; i++) {
			stream.Write(array_[i]);
		}
	}

	virtual void Deserialize(Stream &stream) {
		delete[] array_;

		stream.Read(count_);
		if(count_ == 0) {
			array_ = new T[DEFAULT_CAPACITY];
			capacity_ = DEFAULT_CAPACITY;
		}
		else {
			array_ = new T[count_];
			capacity_ = count_;

			for(size_t i = 0; i < count_; i++) {
				stream.Read(array_[i]);
			}
		}
	}
	
	//
	// Operatori supraincarcati.
	//
	T &operator [](size_t index) const {
		assert(index < count_);
		// --------------------------------
		return array_[index];
	}
	
	List operator =(const List &other) {
		if(&other == this) return *this;
		
		T* newArray = new T[other.count_];
		std::copy(other.array_, other.array_ + other.count_, other.count_);
		
		count_ = other.count_;
		capacity_ = other.count_;
		delete[] array_;
		array_ = newArray_;
		return *this;
	}
	
private:
	//
	// Metode private.
	//
	void EnsureSpace(size_t newCount) {
		// Realoca vectorul daca este prea mic.
		if(newCount > capacity_) {
			T* oldArray = array_;
			array_ = new T[capacity_ * 2];
			
			memcpy(array_, oldArray, count_ * sizeof(T));
			capacity_ *= 2;
		}
	}
};

// Definitii pentru constante.
template<class T>
const size_t List<T>::DEFAULT_CAPACITY = 16;

#endif
