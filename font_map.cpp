#include <fstream>

namespace FontMap {

	struct Font {
		unsigned char height, width;
		unsigned char* symbol[95];
		bool isLoaded;
		int length;
	};

	unsigned char* getCharacterSheet(const Font& font, char character) {
		if (0x20 >= character || character >= 0xff) return font.symbol[0];
		return font.symbol[character - 0x20];
	}

	void fontAssembler(Font& font, const char* font_file) {

		if (font.isLoaded) for (int i = 0; i < 95; i++) {
			delete[] font.symbol[i];
		}

		font.isLoaded = false;

		std::ifstream file(font_file, std::ios::in | std::ios::binary);
		if (!file) {
			file.open("assets/default.ftmp", std::ios::in | std::ios::binary);
			if (!file) return;
		}

		file.seekg(-6, file.end);
		int length = (int)file.tellg();

		unsigned char size[2];
		file.seekg(4, file.beg);
		file.read((char*)size, 2);

		unsigned char* buffer = new unsigned char[length];
		file.read((char*)buffer, length);

		int font_size = (int)(size[0] * size[1]);
		int font_bytes = (font_size - 1) / 8 + 1;

		int file_cursor = 0;
		char byte_cursor = 7;

		for (int i = 0; i < 95; i++) {
			font.symbol[i] = new unsigned char[font_bytes];

			for (int j = 0; j < font_bytes; j++) font.symbol[i][j] = 0;

			for (int j = 0; j < font_size; j++) {
				if (byte_cursor == -1) { file_cursor++; byte_cursor = 7; }

				font.symbol[i][j / 8] += (((buffer[file_cursor] >> byte_cursor)) % 2) << (7 - (j % 8));
				byte_cursor--;
			}
		}

		font.width = size[1];
		font.height = size[0];
		font.length = font_size;
		font.isLoaded = true;

		delete[] buffer;

		file.close();
	}
};