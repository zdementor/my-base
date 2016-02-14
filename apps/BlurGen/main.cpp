#include <windows.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>

int main(int argc, char** argv)
{
    
    if (argc!=3)
    {   
        printf("Usage:\n");
        printf("  BlurGen.exe Num Radius\n");
        return 0;
    }
    
    int N  =atof(argv[1]);
    float R  =atof(argv[2]);

    FILE* out=fopen("blur.h", "w");

    float _2PI = 2*3.1415;
    float dA =_2PI/(float)N; 
    float A=0;
    float N_1 = N-1;

    float X=0, Y=0;
    
    for(int i=0; i<N; i++)
    {
        A = i*dA;
        Y = sin(A)*R;
        X = cos(A)*R;   
        
        if (out) 
        {
            if (i==0)            
            {
                fprintf(out, "const s32 blur_num = %d;\n",N);
                fprintf(out, "f32 blur[blur_num][2] = {\n");
            }
            if (i==N_1)
            {   
                fprintf(out,"    {%.4f, %.4f}\n",X,Y);
                fprintf(out,"};\n",X,Y);
            }            
            else
            {
                fprintf(out,"    {%.4f, %.4f},\n",X,Y);
            }
        }
    }

    if (out) fclose(out);

    return 0;
}
