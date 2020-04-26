#pragma once
#include <fstream>
#include "myexcept/myexcept.h"
#include <string>
#include <type_traits>

#define bioexport _declspec(dllexport)

#define POINTERS_FORBIDDEN(T) static_assert(!std::is_pointer<T>::value, "pointers are forbidden in this function")

namespace bio {


	class BinaryBuffer;

	/**
	 * @brief Abstract class for (de-)serializing an object
	 */
	class BinarIObject {
	public:
		/**
		 * @brief serialize derived object into buffer
		 * @param buffer buffer to serialize into
		 */
		virtual void serialize(BinaryBuffer& buffer) = 0;
		/**
		 * @brief deserialize derived object from buffer
		 * @param buffer buffer to deserialize from
		 */
		virtual void deserialize(BinaryBuffer& buffer) = 0;
	};

	/**
	 * @brief serialize basic data type into buffer
	 */
	template <typename T> inline void serialize(BinaryBuffer&, T);
	/**
	 * @brief deserilialize basic data type from buffer
	 */
	template <typename T> inline T deserialize(BinaryBuffer&);
	
	template <> inline void serialize<char>(BinaryBuffer&, char);
	template <> inline char deserialize<char>(BinaryBuffer&);
	template <> inline void serialize<std::string>(BinaryBuffer&, std::string);
	template <> inline std::string deserialize<std::string>(BinaryBuffer&);
	
	/**
	 * @brief serailize array into buffer
	 */
	template <typename T> inline void serializeArray(BinaryBuffer&, T*, size_t);
	/**
	 * @brief deserialize array from buffer
	 */
	template <typename T> inline T* deserializeArray(BinaryBuffer&);

	/**
	 * @brief Mode necessary to open buffer in read or write mode
	 */
	enum class MODE {
		READ, WRITE
	};


	/**
	 * @brief Buffer storing constant amount bytes. Can be opened in read or write mode to enable automatic reading or writing 
	 */
	class BinaryBuffer {
	protected:
		size_t			current_index		{0};
		std::fstream*	file				{nullptr};
		std::streamsize	size				{0};
		MODE			current_mode;

		/**
		 * @brief read next chunk of bytes.
		 */
		bioexport void	read_next();
		/**
		 * @brief write next chunk of bytes and clear buffer.
		 */
		bioexport void	write_next();
		
	public:

		/*Operators*/
		template<typename T> inline void operator<<(T t);
		template<typename T> inline void operator>>(T& t);

		/**
		 * @brief default values for BinaryBuffer class
		 */
		struct defaults {
			static size_t buffer_size;
		};
		
		/**
		 * @brief construct BinaryBuffer with default values.
		 */
		bioexport BinaryBuffer	();
		/**
		 * @brief construct BinaryBuffer with custom buffer size
		 * @param buffer_size size of buffer. 
		 */
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