
#include <ios>
#include <iomanip>
#include <string>
#include <iostream>

#include "pole.h"
#include "picosha2.h"


void handleStream(POLE::Storage* storage, std::string name, std::string fullname) {

	POLE::Stream* stream = new POLE::Stream(storage, fullname);
	if (stream) {
		unsigned int len = (unsigned int)(stream->size());
		unsigned char* buffer = new unsigned char[len];
		stream->seek(0);
		unsigned int actualLen = (unsigned int)stream->read(buffer, len);
		delete stream;

		std::string hash_hex_str = picosha2::hash256_hex_string(buffer, buffer + actualLen);

		std::cout << fullname << "\t" << hash_hex_str << std::endl;
	}
}

void walk(POLE::Storage *storage, std::string path, void(*streamCallback)(POLE::Storage*, std::string, std::string)) {
	std::list<std::string> entries;
	entries = storage->entries(path);

	std::list<std::string>::iterator it;
	for (it = entries.begin(); it != entries.end(); ++it)
	{
		std::string name = *it;
		std::string fullname = path + name;

		streamCallback(storage, name, fullname);
		if (storage->isDirectory(fullname))
		{
			walk(storage, fullname + "/", streamCallback);
		}
	}
}

int main(int argc, char* argv[]) {
	if (argc > 1) {
		POLE::Storage *storage = new POLE::Storage(argv[1]);
		storage->open();
		if (storage->result() != POLE::Storage::Ok) {
			storage->close();
			delete storage;
		}
		else {
			walk(storage, "/", handleStream);
		}
	}
}