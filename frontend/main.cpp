#include <cstdio>
#include <cstdlib>
#include <csignal>
#include <concepts>
#include <unistd.h>

#include "../pipette/pipette.cpp"

// PIPE OVERLOADS //

int operator >> (const pipette::fifo& pipe, std::integral auto& T)
{
	return pipe.read(&T, sizeof(T));
}

int operator << (const pipette::fifo& pipe, const std::integral auto& T)
{
	return pipe.write(&T, sizeof(T));
}


int main()
{
	char     key      = 'Z';
	uint8_t  bits     =  0;
	uint8_t  rnd      =  0;
	uint32_t num_pnts =  0;
	
	pipette::fifo fin("/home/tmp_inb", 'w'); // send to backend
	pipette::fifo fout("/home/tmp_outb", 'r'); // recieve from backend
	
	fout >> bits;
	std::printf("<F> Bits/Coord : %u\n", bits);
	
	auto main2 = [&]<typename mint>()
	{
		mint px, py, pz; // too lazy to implement dynamic size
		
		fout >> px; fout >> py; fout >> pz;
		std::printf("<F> World Size : (%d, %d, %d)\n", px, py, pz);
		
		while (true)
		{
			sleep(1);
			key = ++rnd % 2 == 0 ? 'X' : 'Y';
			
			if (!(fin << key))
			{
				std::puts("<F> Error Sending Key...\n");
			}
			else
			{
				std::printf("<F> Sent Key '%c'...\n", key);
			}
			
			fout >> num_pnts;
			std::printf("<F> Total Points : %u\n", num_pnts);
			
			fout >> px; fout >> py; fout >> pz;
			std::printf("<F> Food : (%d, %d, %d)\n\n", px, py, pz);
	
			sleep(20);
			std::printf("<F> Snek : [");
			for (int i=0 ; i < num_pnts-1 ; ++i)
			{
				sleep(1);
				fout >> px; fout >> py; fout >> pz;
				std::printf("(%d, %d, %d)", px, py, pz);
			}
			std::printf("]\n\n");
		}
		return 0;
	};

	switch (bits)
	{
	case 8:
		return main2.operator()<uint8_t>();
		break;
	
	case 16:
		return main2.operator()<uint16_t>();
		break;
	
	case 32:
		return main2.operator()<uint32_t>();
		break;
	
	case 64:
		return main2.operator()<uint64_t>();
		break;
	
	default:
		std::puts("\e[31;1m<F> Invalid Bits/Coord !\e[0m\n");
		return -1;
	}
}