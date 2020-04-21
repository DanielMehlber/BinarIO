#pragma once
#include <fstream>
#include "myexcept/myexcept.h"
#include <string>
#include <type_traits>

#define bioexport _declspec(dllexport)

#define POINTERS_FORBIDDEN(T) static_assert(!std::is_pointer<T>::value, "pointers are forbidden in this function")

namespace bio {


	class BinaryBuffer;

	class BinarIObject {
	public:
		virtual void serialize(BinaryBuffer& buffer) = 0;
		virtual void deserialize(BinaryBuffer& buffer) = 0;
	};

	/*functions for basic data types and pointers, not BinarIObjects*/
	template <typename T> inline void serialize(BinaryBuffer&, T);
	template <typename T> inline T deserialize(BinaryBuffer&);
	/*Specifications of those 2 above for e.g. chars or strings*/
	template <> inline void serialize<char>(BinaryBuffer&, char);
	template <> inline char deserialize<char>(BinaryBuffer&);
	template <> inline void serialize<std::string>(BinaryBuffer&, std::string);
	template <> inline std::string deserialize<std::string>(BinaryBuffer&);
	/*functions for arrays*/
	template <typename T> inline void serializeArray(BinaryBuffer&, T*, size_t);
	template <typename T> inline T* deserializeArray(BinaryBuffer&);

	enum class MODE {
		READ, WRITE
	};


	class BinaryBuffer {
	protected:
		size_t			current_index		{0};
		std::fstream*	file				{nullptr};
		std::streamsize	size				{0};
		MODE			current_mode;
		bioexport void	read_next();
		bioexport void	write_next();
		
	public:

		/*Operators*/
		template<typename T> inline void operator<<(T t);
		template<typename T> inline void operator>>(T& t);

		struct defaults {
			static size_t buffer_size;
		};

		bioexport BinaryBuffer	();
		bioexport BinaryBuffer	(size_t buffer_size);
		bioexport BinaryBuffer  (const BinaryBuffer& cpy);
		bioexport ~BinaryBuffer	();

		char*			buffer;		
		const size_t	buffer_size;
		bioexport void	open(MODE mode, const char* location);
		bioexport void 	close();
		bioexport void 	finish();
		inline void		push(char c);
		inline char		pull();
	};

	
	

	template <typename T> inline void serialize(BinaryBuffer& buffer, T t){
		POINTERS_FORBIDDEN(T);
		union{
			char buff[sizeof(T)];
			T t;
		} u;
		u.t = t;

		for(size_t i = 0; i < sizeof(T); i++){
			try {
				buffer.push(u.buff[i]);
			} catch (myexcept::exception& e) {
				e(__FUNCTION__, "pushing char into buffer");
				throw;
			}
		}
	}

	template <typename T> inline T deserialize(BinaryBuffer& buffer){
		POINTERS_FORBIDDEN(T);
		union{
			char buff[sizeof(T)];
			T t;
		} u;

		for(size_t i = 0; i < sizeof(T); i++){
			try { u.buff[i] = buffer.pull(); } catch (myexcept::exception& e) {
				e(__FUNCTION__, "pulling char from buffer");
				throw;
			}
		}

		return u.t;
	}

	template <> inline void serialize<char>(BinaryBuffer& buffer, char c){
		try{
			buffer.push(c);
		} catch (myexcept::exception& e){
			e(__FUNCTION__, "serializing single char");
			throw;
		}
	}

	template <> inline char deserialize<char>(BinaryBuffer& buffer){
		try
		{
			return buffer.pull();
		}
		catch(myexcept::exception& e)
		{
			e(__FUNCTION__, "deserialing single char");
			throw;
		}
		
	}

	template <typename T> inline void BinaryBuffer::operator<< (T t){
		try
		{
			serialize<T>(*this, t);
		}
		catch(myexcept::exception& e)
		{
			e(__FUNCTION__, "serialization operator");
			throw;
		}
		
	}

	template <typename T> inline void BinaryBuffer::operator>> (T& t){
		try
		{
			t = deserialize<T>(*this);
		}
		catch(myexcept::exception& e)
		{
			e(__FUNCTION__, "deserilization operator");
			throw;
		}
		
	}

	template <typename T> inline void serializeArray(BinaryBuffer& buffer, T* t, size_t length){
		serialize<size_t>(buffer, length);
		for(size_t i = 0; i < length; i++){
			try{ serialize<T>(buffer, t[i]); } catch (myexcept::exception& e){
				e(__FUNCTION__, "serializing to buffer");
				throw;
			} 
		}
	}

	template <typename T> inline T* deserializeArray(BinaryBuffer& buffer){
		auto length = deserialize<size_t>(buffer);
		T* tarr = new T[length];
		for(size_t i = 0; i < length; i++){
			try{ tarr[i] = deserialize<T>(buffer); } catch (myexcept::exception& e) {
				e(__FUNCTION__, "derserializing array");
				throw;
			}; 
		}
		return tarr;
	}

	template <> inline void serialize<std::string>(BinaryBuffer& buffer, std::string t){
		try{ 
			size_t length = t.size();
			serialize<size_t>(buffer, length);
			for(size_t i = 0; i < length; i++){
				serialize<char>(buffer, t[i]); 
			}
		} catch (myexcept::exception& e) {
			e(__FUNCTION__, "serializing string");
			throw;
		}
	}


	template <> inline std::string deserialize<std::string>(BinaryBuffer& buffer){
		try
		{
			const size_t length = deserialize<size_t>(buffer);
			std::string ret;
			for(size_t i = 0; i < length; i++){
				ret.push_back( deserialize<char>(buffer) );
			}
			return ret;
		}
		catch(myexcept::exception& e)
		{
			e(__FUNCTION__, "deserialing string");
			throw;
		}
		
	}
	

	inline void BinaryBuffer::push(char c) {
		buffer[current_index] = c;
		current_index++;
		if (current_index == buffer_size){
			try { write_next(); }
			catch(myexcept::exception& e)
			{
				e(__FUNCTION__, "writing next section");
				throw;
			}
		}
	}

	inline char BinaryBuffer::pull() {
		char c = buffer[current_index];
		current_index++;
		if (current_index == buffer_size){
			try { read_next(); }
			catch(myexcept::exception& e)
			{
				e(__FUNCTION__, "reading next section");
				throw;
			}
			
		}
		return c;
	}

	size_t BinaryBuffer::defaults::buffer_size = 5000;

	


}