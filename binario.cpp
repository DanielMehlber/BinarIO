#include "binario.h"

bio::BinaryBuffer::BinaryBuffer() : max_buffer_size{bio::BinaryBuffer::defaults::buffer_size}{
	buffer = new char[max_buffer_size];
};

bio::BinaryBuffer::BinaryBuffer(size_t size) : max_buffer_size{size}{
	buffer = new char[max_buffer_size];
};

bio::BinaryBuffer::~BinaryBuffer()
{
	delete[] buffer;
};

void bio::BinaryBuffer::open(MODE mode, const char* location)
{
	current_index = 0;

	if(mode == bio::MODE::READ)
		file = new std::fstream(location, std::ios::in | std::ios::binary);
	else
		file = new std::fstream(location, std::ios::out | std::ios::binary | std::ios::trunc);
	
	current_mode = mode;

	if(!file)
		throw myexcept::myexcept(__FUNCTION__, "Cannot open file. Maybe it doesn't exist");
	file->seekp(std::ios::end);
	size = file->tellp();
	file->seekp(std::ios::beg);
	file->seekg(std::ios::beg);
}

void bio::BinaryBuffer::read_next()
{
	if (!file)
		throw myexcept::myexcept(__func__, "No file open to read from");

	if(!file->is_open())
		throw myexcept::myexcept(__func__, "No file open to write to");
	
	size_t delta = size - file->tellg();
	if (delta <= 0)
		throw myexcept::myexcept(__func__, "No bytes left to read. Reached end of file");
	auto r = max_buffer_size < delta ? max_buffer_size : delta;
	file->read(buffer, r);

	if(!file->good())
		throw myexcept::myexcept(__func__, "Failed writing to file");

	current_index = 0;
}

void bio::BinaryBuffer::write_next()
{
	if(!file)
		throw myexcept::myexcept(__FUNCTION__, "No file to write to or creation failed");

	if(!file->is_open())
		throw myexcept::myexcept(__func__, "No file open to write to");
	
	file->write(buffer, current_index);

	if(!file->good())
		throw myexcept::myexcept(__func__, "Failed writing to file");

	current_index = 0;

}

void bio::BinaryBuffer::finish(){
	if(!file)
		return;

	if(file->is_open() && current_index > 0 && current_mode == bio::MODE::WRITE){
		write_next();
	}
}

void bio::BinaryBuffer::close(){
	if(!file)
		return;
	if(!file->is_open())
		return;

	finish();
	file->close();
	delete file;
}