#include <iostream>
#include <fstream>
#include <cstdint>

const int gScreenWidth = 1024;
const int gScreenHeight = 768;
const int gSpriteSize = 64;


void blitSprite(uint16_t* frameBuffer, uint8_t* sprite, int x, int y);
bool writeSprite(const std::string& path, const uint16_t* src);

int main(int argc, char *args[])
{
	//
	srand(time_t(NULL));

	uint8_t* sprite = new uint8_t[gSpriteSize * gSpriteSize * 4];
	uint16_t* frameBuffer = new uint16_t[gScreenWidth * gScreenHeight];

	//Test Sprite
	for (int i = 0; i < gSpriteSize * gSpriteSize * 4; i += 4)
	{
		sprite[i + 0] = 0x00;//B
		sprite[i + 1] = 0x00;//G
		sprite[i + 2] = 0xFF;//R
		sprite[i + 3] = 0xFF;//A
	}

	for (int i = 0; i < gScreenWidth * gScreenHeight; ++i)
	{
		frameBuffer[i] = 0xFF;
	}
	
	blitSprite(frameBuffer, sprite, gScreenWidth-64, gScreenHeight-64);

	writeSprite("test.pbm", frameBuffer);
	system("pause");
	return 0;
}

//RGB565
//BGRA8888
void blitSprite(uint16_t* frameBuffer, uint8_t* sprite, int _posX, int _posY)
{
	for (int yy = 0; yy < gSpriteSize; ++yy)
	{ 
		if (_posY + yy < 0 || _posY + yy > gScreenWidth)
			continue;
		for (int xx = 0; xx < gSpriteSize; ++xx) //for(int xx = 0; xx < gSpriteSize*4; ++xx) -> int frameBufferIndex = (_posX + (xx/4)) + (_posY + yy) * gScreenWidth; ...
		{
			if (_posX + xx < 0 || _posX + xx > gScreenWidth)
				continue;
			int frameBufferIndex = (_posX + xx) + (_posY + yy) * gScreenWidth;

			uint16_t color = frameBuffer[frameBufferIndex];

			//Framebuffer rgb565
			uint8_t frameRed = ((color >> 11) & 0x1F);
			uint8_t frameGreen = ((color >> 5) & 0x3F); //>> 6?
			uint8_t frameBlue = (color & 0x1F);

			//"Sprite" colors
			uint8_t index = (xx*4) + yy * gSpriteSize;
			uint8_t spriteBlue = (sprite[index + 0]);
			uint8_t spriteGreen = (sprite[index + 1]);
			uint8_t spriteRed = (sprite[index + 2]);
			uint8_t alpha = sprite[index + 3] ;

			//BGRA8888 -> RGB565
			uint16_t b = (spriteBlue >> 3) & 0x1F; //31 -> 2^5-1
			uint16_t g = ((spriteGreen >> 2) & 0x3F) << 5; // 63 -> (2^6)-1
			uint16_t r = ((spriteRed >> 3) & 0x1F) << 11; // 31 -> (2^5)-1

			//Alpha Blending
			r = (alpha * (r - frameRed) + frameRed);
			g = (alpha * (g - frameGreen) + frameGreen);
			b = (alpha * (b - frameBlue) + frameBlue);

			//uint16_t rgb565 = (uint16_t) (r | g | b);

			frameBuffer[frameBufferIndex] = (uint16_t)(r | g | b);
		}
	}
}

bool writeSprite(const std::string& path, const uint16_t* src)
{
	std::ofstream file(path);
	file << "P3" << std::endl;
	file << "# CREATOR: CODA. CONTEST GAME LOOP 2017" << std::endl;
	file << gScreenWidth << " " << gScreenHeight << std::endl;
	file << 255 << std::endl;
	for (int i = 0; i < gScreenWidth * gScreenHeight; ++i)
	{
		unsigned int r = ((src[i] >> 11) & 0x1F);
		unsigned int g = ((src[i] >> 5) & 0x3F);
		unsigned int b = (src[i] & 0x1F);
		//RGB565 -> RGB888
		r = ((r * 527) + 23) >> 6;
		g = ((g * 259) + 33) >> 6;
		b = ((b * 527) + 23) >> 6;
		file << r << std::endl << g << std::endl << b << std::endl;
	}
	file.close();
	return true;
}