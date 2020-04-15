#include <fstream>
#include "myexcept/myexcept.h"

#define bioexport _declspec(dllexport)

namespace bio {

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

		struct defaults {
			static size_t buffer_size;
		};

		bioexport BinaryBuffer();
		bioexport BinaryBuffer(size_t buffer_size);
		bioexport ~BinaryBuffer();

		char*			buffer;		
		const size_t	max_buffer_size;
		bioexport void	open(MODE mode, const char* location);
		bioexport void 	close();
		bioexport void 	finish();
		inline void		push(char c);
		inline char		pull();
	};

	inline void BinaryBuffer::push(char c) {
		buffer[current_index] = c;
		current_index++;
		if (current_index == max_buffer_size){
			try { write_next(); }
			catch(myexcept::myexcept& e)
			{
				e(__FUNCTION__, "Cannot write next byte section.");
				throw;
			}
		}
	}

	inline char BinaryBuffer::pull() {
		char c = buffer[current_index];
		current_index++;
		if (current_index == max_buffer_size){
			try { read_next(); }
			catch(myexcept::myexcept& e)
			{
				e(__FUNCTION__, "Cannot read next byte section.");
				throw;
			}
			
		}
	}

	size_t BinaryBuffer::defaults::buffer_size = 5000;

	class BinarIObject {
	public:
		virtual void serialize(BinaryBuffer& buffer) = 0;
		virtual void deserialize(BinaryBuffer& buffer) = 0;
	};

	template <typename T> inline void serialize(BinaryBuffer& buffer, T t){
		union{
			char buff[sizeof(T)];
			T t;
		} u;
		u.t = t;

		for(size_t i = 0; i < sizeof(T); i++){
			try {
				buffer.push(u.buff[i]);
			} catch (myexcept::myexcept& e) {
				e(__FUNCTION__, "Cannot push char into buffer");
				throw;
			}
		}
	}

	template <typename T> inline T deserialize(BinaryBuffer& buffer){
		union{
			char buff[sizeof(T)];
			T t;
		} u;

		for(size_t i = 0; i < sizeof(T); i++){
			try {
				u.buff[i] = buffer.pull();
			} catch (myexcept::myexcept& e) {
				e(__FUNCTION__, "Cannot pull char from buffer");
				throw;
			}
		}

		return u.t;
	}

}