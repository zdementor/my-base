#include <stdio.h>
#include <stdlib.h>

int main(int argc, char* argv[])
{
	if (argc==2)
	{
		char *filename = argv[1];

		FILE *f = fopen(filename, "rb");

		if (f)
		{
			fseek(f, 0, SEEK_END);
			fpos_t pos; 				
			fgetpos(f, &pos);

			int size=(int)pos;

			fseek(f, 0, SEEK_SET);

			if (size>0)
			{
				unsigned char *file_data = new unsigned char[size];
					
				fread(file_data, 1, size, f);

				FILE *out = fopen("out.cpp", "wb");

				if (out)
				{
					fprintf(out, "const int FILE_LENGTH = %d;\n", size);
					fprintf(out, "char file_data[FILE_LENGTH] =\n");
					fprintf(out, "{\n");
					for (int i=0; i<size; i++)
					{
						fprintf(out, "0x%02X", file_data[i]);
						
						if (i!=(size-1))
							fprintf(out, ", ", file_data[i]);

						if (i!=0 && i%20==0)
							fprintf(out, "\n", file_data[i]);						
					}
					fprintf(out, "\n};\n");

					fclose(out);
				}

				delete []file_data;
			}			

			fclose(f);
		}
	}

	return 0;
}